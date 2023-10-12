# Boot

L'objectif du boot est de charger le noyau en mémoire, de lui fournir les informations dont il a besoin pour fonctionner et d'initialiser certains paramètres du processeur.
En dernière étape, le bootloader passe en mode supervisor et donne la main au noyau.

Pour charger un kernel avec Qemu, nous avions utilisé en projet de conception d'OS l'option "-kernel" en lui passant une image.
Celle-ci est soit une image linux soit une image au format [multiboot](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.pdf) (cf man qemu).

Exemple : `$ qemu-system-i386 -kernel kernel.bin`

La norme multiboot ne supporte cependant que les processeurs MIPS et i386.
Dans le cas de l'architecture RISC-V Il faudra donc intégrer un boot à notre image.

Un boot complet peut ce décomposer en deux phase, le "First Stage Bootloader" (FSB) et le boot en lui même.

## démarrage du processeur

Pour connaître l'état du système au démarrage vous pouvez consulter la documentation de LowRiscv [Bootload procedure lowRISC](https://www.lowrisc.org/docs/untether-v0.2/bootload/).

## First stage bootloader

Le processeur démarre à l'adresse `0x1000`, le morceau de code qui si trouve se prénomme couramment "First Stage Bootloader" (FSB).
Celui-ci est couramment placé dans une petite ROM à côté ou directement dans le processeur.
Dans notre cas, cette partie est intégré à Qemu et nous n'avons pas besoin de nous en préoccuper.
Le FBL s'occupe de configurer certain paramètres hardware comme la RAM et saute rapidement à l'adresse `0x80000000` où notre aventure commence.

## Boot mode machine

Comme nous l'avons vue, une fois l'initialisation du processeur faite par le FSB, celui-ci jump à l'adresse mémoire de `0X80000000`.
C'est pourquoi l'ensemble du code du kernel et les données qui le compose sont situés au delà de 2 GiB.
Notons que cette adresse peut varier selon les implémentations, mais celle-ci est couramment utilisée dans la communauté RISC-V.

C'est à partir de cette adresse que le projet commence, bien sur en langage d'assemblage.
Voici les étapes successives qui permettent de configurer le processeur, les périphériques et les interruption pour passer en mode supervisor.
Les chemins des fichiers présentés ici sont donnés à  partir du dossier `./src/kernel/` du projet de base. 

### `boot/common/crtm.S`

Ici le travail à déjà été fait mais il est très important de comprendre chaque ligne pour pouvoir avancer sereinement dans le projet.

1. (Optionnel) initialiser les 32 registres du processeur à zéro ainsi que le csr mscratch.
2. Configurer le gestionnaire de trap
* Le gestionnaire de trap traite les exception et les interruption,
* Ce traitant a déjà été écrit pour le mode machine et vous le trouverez dans `trap/machine_trap_entry.S` sous le label `mtrap_entry`,
* un registre dédié au mode machine: `mtvec` permêt d'indiquer l'adresse de ce traitant.
3. Création des piles machine pour chaque core à l'aide de la commande d'assemblage `.skip`. Le numéro de l'hart est donné par le registre `hartid` et permêt de calculer l'adresse des piles de chaqu'un des cores.
4. Les piles seront directement utilisées en plaçant l'adresse du haut de la pile dans le registre SP.
5. Création d'une pile dédiée pour la gestion des traps machine.
L'adresse du haut de cette pile est enregistré dans le registre `mscratch`.
Ce registre sera lu par le traitant de trap dans l'optique de basculler sur une pile indépendante.
6. Il faut endormir (parquer) les core Risc-v appelés hart avec un id supérieur à zéro pour ne continuer le boot qu'avec un seul hart.
    * Endormir les harts avec l'instruction wfi (wait for interrupt).
    * Boucler avec wfi si ils se réveillent (ou activation dans le cas du multi Cœres).
    * (Optionnel) Si on veut pouvoir réactiver ces harts dans un second temps, il faudra activer les interruptions IPI (Interprocessor interrupts) dans le csr `msie`.
7. Pour le hart 0, le voyage continue avec un jump vers notre programme de boot C |boot_riscv()`

### Configuration de la machine et de la console `boot/$(MACHINE)/setup.c` (fourni)

Dans cette initialisation spécifique à la machine, les configuration suivantes sont effectuées:

* UART in / out si disponible (périphérique externe au processeur);
* htif sinon (liaison série intégrée dans le processeur);
* activation des compteurs de performances machine et supervisor si disponible;
* configuration du Platform-Level Interrupt Controller  (PLIC).

Ici le travail à déjà été fait. Il sera important de le lire car quelques lignes vous seront utiles par la suite.

### Affichage des informations du processeur `info.c` (fourni)

Divers registres csr sont à dispositions pour obtenir des informations sur l'architecture.
Ce programme affiche :

* Type de processeur : 32 ou 64 bits;
* extensions prises en charge par le processeur (i, m, a, c, f, d, … (voir la page sur l'[ISA](ISA-RISC-V).

### `boot/common/boot.c` : boot_riscv

Dans cette partie nous allons configurer pas à pas le processeur, les interruptions et ces périphériques pour pouvoir passer en mode supervisor.
C'est ici que votre travail commence.

* On peut s'assurer que la mémoire virtuelle est désactivée pour le moment en écrivant 0 dans le csr `satp`;
* Désactivation globale des interruption machine à l'aide du csr `mstatus`.  
En mode machine, celles-ci doivent-être toujours désactivées.
Il est bien sur possible de les activer pour des essais, c'est ce que l'on fait dans les tests fournis;
* Activation de la protection de la mémoire physique (PMP) :
    * Configuration de la PMP à l'aide des registres **CSR** `pmpcfg` et `pmpaddr`,
    * Dans `pmpcfg0`, authoriser les accès mémoires en lecture, écriture et exécution,  le champ A doit-être configuré sur le mode **NAPOT**,
    * `pmpaddr0` doit inclure toute la mémoire;
* Passage en mode supervisor :
    * Configuration du registre `mepc` qui indique ce que l'on va exécuter en mode supervisor,
    * indication du mode dans lequel on doit se rendre (`mstatus`),
    * désactiver les interruptions dans le mode où l'on se rend (`mstatus` également),
    * lancement du kernel `kernel_start` à l'aide de l'instruction **mret**.


Note: Avant d'activer les interruptions, il faudra penser à changer de pile.
En effet, vous êtres toujours sur la pile machine qui sera également utilisée pour traiter les interruptions et les exceptions machines.

## Références

* [Bootload procedure lowRISC](https://www.lowrisc.org/docs/untether-v0.2/bootload/)
* [Stack overflow : RISC-V startup code for stack pointer initalization](https://stackoverflow.com/questions/50214840/risc-v-startup-code-for-stack-pointer-initalization)

