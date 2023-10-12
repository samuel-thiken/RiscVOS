# Traps

Sur l'architecture RISC-V, le terme de _trap_ regroupe les notions d'interruption et d'exception.

* Une exception est levée lorsque le processus en cours génère une erreur ou réalise un appel système à l'aide de l'instruction `ecall`;
* les interruptions peuvent provenir de 3 sources différentes : externe, timer ou software.

Les exceptions et les interruptions peuvent être traitées à différents niveaux selon la configuration du processeur.

***Par défaut toutes les interruptions et les exceptions sont traitées par le mode machine***.

Certaines d'entre elles peuvent être 'déléguées' sur demande au mode _supervisor_.

## Déclenchement d'une exception

Une exception est levée par le processeur quand une instruction provoque une faute.
La cause de l'exception est écrite par le processeur dans le **csr** `mcause` ou ` scause` selon le mode.
Pour connaître les différentes cause possible d'une exception allez voir la documentation privilégiée dans la section **csr** `mcause` ou `scause`.

## Déclenchement d'une interruption

Le processeur réagit à une interruption si les conditions suivantes sont réunies:

* les interruptions sont activées dans le processeur pour le mode correspondant dans le **csr** `mstatus` (champ `MIE`, pour le mode machine et `SIE` pour le mode superviseur).
* une requête d'interruption a eu lieu (le champ correspondant des **csr** `mip` ou `sip` est passé à 1, (Machine / Supervisor Interrupt Pending);
* le bit correspondant à cette interruption a préalablement été mis à 1 dans le registre `mie` (machine interrupt enable) ou `sie` (supervisor interrupt enable) pour que le processeur y soit sensible;
* attention, Dans le mode supervisor, le champ `MIE` n'a pas d'importance et les interruptions machine configurées dans `mie` sont toujours activées. De manière équivalente, en mode user les interruption supervisor et machine sont toujours démasquées. En résumé, un mode donné ne peut pas empêcher les modes plus privilégiés de l'interrompre. 

Clarifions tout ceci par un exemple :

Une interruption timer de niveau machine est levée : le bit `MTIP` de `mip` passe à 1.
Le processeur ne propagera l'interruption que si le bit correspondant du registre `mie` est à 1 et si les interruptions machines sont globalement activées dans `mstatus`.
Dans le cas contraire, l'interruption sera ignorée jusqu'à ce que l'ensemble de ces critères soient réunis.

### Activer une interruption

En résumé, voici ce qu'il faut faire pour activer une interruption pour un mode donné :

* s'assurer que les interruptions du mode en question soient belles et bien activées à partir du **csr** `status` ou que le processeur se trouve dans un mode moins privilégié que le mode où l'interruption sera traitée;
* activé l'interruption voulu dans le **csr** `mie`;
* si l'on souhaite que l'interruption soit traitée en mode supervisor ou user au lieu du mode machine, il faudra la déléguer à l'aide du **csr** `mideleg`.

## Traitement des interruptions et des exceptions

Les interruptions et les exceptions sont traitées de la même manière.
Nous les regrouperons comme évoqué sous le terme anglophone de "traps".

Lorsqu'un trap est déclenché sous la responsabilité du mode machine, le processeur utilise le **csr** `mtvec` pour trouver l'adresse du traitant à exécuter.
Lorsque la responsabilité de l'exception ou de l'interruption a été déléguée au mode supervisor, elle sera traitée par la routine pointée par le registre **csr** `stvec`.

Dans la suite, nous utiliserons la notation `xtvec` pour généraliser les explications à `mtvec` et `stvec`.

Lorsqu'un trap est déclenché, le choix du traitant peut s'effectuer de deux manières différentes selon le mode de traitement de trap choisi :

* si les deux derniers bits de `xtvec` valent 0, le processeur saute directement à l'adresse de `xtvec` et exécute le code qu'il y trouve.
* si les deux derniers bits de `xtvec` valent 1, le processeur saute à l'adresse de `xtvec` (avec bits de poids faibles à 0) pour toutes les exceptions et il saute à l'adresse de `xtvec` + 4n où n est le numéro de l'interruption. Cela permet de laisser le matériel déterminer le traitant d'interruption à appeler pour chaque interruption.

Il est conseillé d'utiliser la première solution notamment pour sa facilité de mise en œuvre (et le fait que QEMU ne supportait pas le second lorsque nous avons fait la première version du noyau !).

Il faut donc écrire un traitant en assembleur (chargé de sauvegarder le contexte d’exécution, changer de pile, appeler un traitant écrit en C, et faire le chemin inverse pour retourner à l'instruction qui a trappé) pour chacun des modes.

Remarque : l'adresse du traitant doit être aligné sur 4 octets comme le requiert la spécification.
Une fois réalisé, l'adresse de ce traitant devra être écrite dans `xtvec`.

Au moment du déclenchement d'un trap, un certain nombre de **csr** sont mis à jour par le matériel :

* `xcause` qui contient le numéro de l'interruption ou de l'exception et un 1 sur le bit de poids fort dans le cas d'une interruption
* `xepc` qui contient l'adresse de l'instruction dont l'exécution a déclenché le trap (l'instruction **fautive**)
* `xtval` qui contient des informations supplémentaires spécifiques au trap déclenché

A partir de là, les traps sont traités en logiciel.

### Remarque à propos des interruptions

A la fin du traitant d'interruption, il faut penser à *acquitter* l'interruption en passant le bit correspondant à l'interruption de `xip` à 0 de manière à signaler qu'elle a bien été traitée (autrement on risque de retourner immédiatement en interruption).

### Remarque à propos des traps qui ne produisent pas l'arrêt du système

Dans le cas d'une interruption, une fois le traitant exécuté il suffit de retourner à l'adresse stockée dans `xepc` puisque c'est là que le processeur s'était interrompu. Toutefois dans le cas d'une exception, il faut retourner à l'instruction **suivante** !
En effet, `xepc` pointe sur l'instruction qui a produit l'exception. Y retourner ne ferait que la déclencher à nouveau (sauf dans certains cas très spécifique comme le "page fault" où on pourrait vouloir retourner à la même adresse une fois la page chargée en mémoire... mais dans le projet ce genre de cas ne se produit pas).

## La délégation d'interruptions et d'exceptions

Par défaut, tous les traps sont traités en mode machine. Il est possible de "déléguer" certaines d'entre elles au superviseur grâce aux registres `mideleg` et `medeleg` (machine interruptions/exceptions delegation).

# Gestion du temps

La norme RISC-V décrit comment mesurer le temps et programmer des timers à l'aide d'une horloge interne nommée **CLINT** pour Core Level Interruption.
Chaque core d’exécution a sa propre instance du composant et celui-ci est accessible à l'aide d'un jeu de registres dédié.
Il est important de comprendre que ces registre ne sont pas des **csr** mais des registres d'un composant externe mappés en mémoire.

La norme prévoit un couple de registres `time` et `timecmp` pour chaque mode d'exécution.
L'horloge est donnée par les registres `mtime`, `stime` et `utime` (notés `{m,s,u}time`).
Les registres `mtimecmp`, `stimecmp` et `utimecmp` permettent quand à eux de programmer des interruptions timer.
Précisons que ces registres sont accessibles par n'importe qui ayant accès à leur adresse mémoire (superviseur et utilisateur inclus), la mémoire virtuelle peut-être utilisée à des fin de politique d'accès.
(NB : d'autres registres existent pour le temps comme `{m,s,u}cycle` qui comptent les cycles machines mais ils ne nous intéressent pas ici).

Toutefois, la plupart des implémentations hardware ne disposent que d'un **CLINT** pour le mode machine et font l'impasse sur le timer supervisor et user.
Cette contrainte ne permet donc pas au mode supervisor d'utiliser directement le timer pour gérer le temps du système.
Dans cette configuration, le noyau doit faire appel au mode machine pour programmer et acquitter l'interruption timer à l'aide d'appels systèmes SBI (System Binary Interface).

Comme toutes ces notions sont un peu complexes pour être abordées en début de projet, nous avons ajouté la prise en charge du **CLINT** supervisor et user dans l'implémentation de ma chine cep dans QEMU pour le projet.

## Interruptions timer

Dans la norme RISC-V, les interruptions timer sont nomées `{M,S,U}TI` et les bits de **pending** associés sont notés respectivement `{M,S,M}TIP`.
Les interruptions timer sont déclenchées par la comparaison de `{m,s,u}time` avec `{m,s,u}timecmp`.
Ainsi, quand `{m,s,u}time` >= `{m,s,u}timecmp` le **CLINT** maintient le bit `{M,S,U}TIP` correspondant à 1.

Le passage de `{m,s,u}timecmp` a une valeur supérieure à `{m,s,u}time` a pour effet d'acquitter l'interruption timer correspondante.
Dans ce cas de figure, le **CLINT** repasse l'un des bit `{M,S,U}TIP` à 0.

A noter que les registres `{m,s,u}timecmp` ne peuvent être écrits que par un mode avec des droits supérieurs ou égaux au registre ciblé.
Ainsi, le registre `mtimecmp` n'est accessible que du mode machine, le registre `stimecmp` que des modes supervisor et machine et `utimecmp` est quand à lui accessible de tous les modes.
Il faut donc faire très attention à faire la réécriture de `mtimecmp` en mode machine sinon le bit MTIP ne sera jamais clear (c'est traître : si l'écriture est faite dans un autre mode aucune erreur ne va sortir, juste une boucle infinie dans l'interruption machine timer).
Il est donc conseillé d'utiliser le timer supervisor pour gérer le temps au seins de votre système d'exploitation.

## Etapes conseillées
### Etape 1 : Mode machine - Premier Tic Tic

Dans un premier temps il est conseillé de mettre en place une interruption timer machine pour se faire la main.

Objectif: Afficher un **tic** toute les secondes.

Pour ce faire:
1. Compléter la fonction `set_machine_timer_interrupt()` du ficher `timer.c` qui devra configurer le timer machine delta ms dans le future;
2. Le **handler** d'interruption timer machine de `timer.c` doit afficher un **tic** et reconfigurer le timer 1 seconde dans le futur;
3. Programmer un timer machine 1 seconde dans le futur;
4. Autoriser les interruption timer machine;
5. Activer les interruptions machine du **csr** `mstatus`;
6. Tic tic tic ...

Faite la démarche de comprendre par où passe le processeur lors de l'interruption timer.
Lorsqu'une interruption est levée, le **program counter** est modifié par la valeur contenue dans le **csr** `mtvec`.
Pour rappel, ce registre a été configuré lors du boot (fichier `src/kernel/boot/common/mcrt.S`).

Le point d'entrée se situe ainsi au label `mtrap_entry` du fichier `src/kernel/machine_trap_entry.S`.

### Etape 2 : Mode machine  - Configuration du timer supervisor

Compléter votre code afin d'utiliser le timer supervisor à la place du timer machine.
Si l'interruption timer supervisor n'a pas été déléguée, celle-ci sera traitée en mode machine, ça tombe bien vu qu'on a pas encore de mode supervisor.

### Etape 3 : Gestion de l'interruption en mode supervisor

Votre système d'exploitation s'exécutera en mode supervisor et aura besoin du timer pour orchestrer ses processus.
C'est pourquoi vous allez devoir déléguer l'interruption timer supervisor à votre noyau.

Voici les étapes à réaliser:
1. Écrire un traitant de **traps** supervisor sur le modèle du fichier `src/kernel/traps/machine_trap_entry.S` qui prendra en charge les exceptions et les interruptions qui lui seront déléguées;
2. Déléguer l'interruption timer supervisor au mode supervisor.
3. Configurer votre noyau pour qu'une variable `tic` soit incrémentée toute les 50ms. Ce **tic** sera utilisé pour ordonnancer les processus dans la suite du projet.

## Gérer le temps avec un unique **CLINT** machine

***Si vous ne souhaitez pas utiliser le **CLINT** supervisor ou que vous souhaitiez utiliser une implémentation qui n'en possède pas, cette section est faite pour vous.***

Le challenge dans notre projet est que nous souhaitons effectuer un traitement d'interruption timer en mode superviseur (notamment pour pouvoir effectuer un ordonnancement des processus).
Il nous faut donc une interruption temporelle au niveau du supervisor sans pour autant avoir de **CLINT** dédié.
Le timer machine peut-être utilisé pour déclencher une interruption timer supervisor.
Il suffit de positionner le bit STIP de `mip` à 1 dans le traitant d'interruption du timer machine.
Une fois que l'on quitte le traitant d'interruption machine, l'interruption timer superviseur que nous avons explicitement levée va se déclencher.

Il faut voir cette nouvelle interruption comme une interruption logicielle puisque c'est l'écriture de ce bit en logiciel qui va causer le détournement du processeur.

### Conseil à la mise en place du timer au niveau du supervisor

Pour résumer, voici l'enchaînement des évènements lors d'une interruption levée par le CLINT.

En mode machine :
1. Le registre du **CLINT** `mtime` devient au fil du temps plus grand que le registre `mtimecmp`
2. Le **CLINT** lève L'interruption machine timer → le champ MTIP du registre `mip` passe à 1.
3. Le système d'exploitation tournant en mode supervisor ou user, le processeur n'a pas besoin  de vérifier le champ MIE du registre `mstatus`.
On rappel que les interruptions d'un mode plus privilégié sont toujours globalement activées.
4. Le processeur vérifie que l'interruption timer machine est démasquée à partir du champ MTIE du registre **csr** `mie`.
5. Le processeur fait appel au machine trap handler pour traiter l'interruption timer. Il récupère son adresse à partir du **csr** `mtvec`.
6. Le traitant découvre que la trap provient du timer machine.
7. Le traitant du timer machine met à jour `mtimecmp` pour programmer une nouvelle interruption dans le futur. L'écriture de `mtimecmp` a pour effet d'acquitter l'interruption timer machine.
8. Le traitant lève une interruption à destination du mode supervisor à l'aide du registre `mip`.
9. Fin du traitant timer machine. On rend la main au programme.
10. On attend la prochaine interruption → Étape 1.

En mode supervisor :
1. L'interruption timer supervisor est levée, champ STIP à 1 dans `sip`.
2. Le processeur voit que l'interruption STI est déléguée au mode supervisor.
3. Le processeur vérifie que les interruptions sont globalement activées pour le mode supervisor.
4. Le processeur vérifie que l'interruption supervisor timer est démasquée.
5. Récupération du supervisor trap handler dans le registre `stvec`.
6. On traite l'interruption, ordonnancement, ….
7. On acquitte l'interruption.
8. On rend la main au programme.

### Etapes conseillées

1. Mise en place d'une interruption timer machine toute les 10 ms. Pour tester cela, il faudra activer temporairement les interruption machine dans `mstatus`.
2. Création d'un trap handler supervisor à l'image de celui du mode machine dans le but de récupérer l'interruption supervisor timer.
Dans un premier temps, vous pouvez ignorer la problématique des piles et utiliser la pile courante pour sauvegarder le contexte.
 3. Création d'une interruption timer supervisor dans boot_riscv : démasquage, délégation…
 4. Levée cette interruption lors du traitement de l'interruption timer machine.
 5. Vérifier que le trap handler supervisor traite bien cette interruption en mode supervisor.
