x# Plan d'adressage de la machine “cep”

| Périphérique         | Adresse    | Size       | Description                                                      |
|----------------------|------------|------------|---------------------------------------------|
| RAM                  | 0x00000000 | 0x01ffffff | Mémoire RAM - Noyau                         |
| CLINT                | 0x02000000 |    0x10000 | Core Level Interrupt                        |
| PLIC                 | 0x0c000000 |  0x4000000 | Platform Level Interrupt Controller         |
| UART                 | 0x10001300 |     0x1000 | Universal Asynchronous Receiver Transmiter  |
| Périphériques divers | 0x30000000 |            | Boutons poussoirs, switchs, leds            |
| VRAM                 | 0x80000000 |   0x384000 | Mémoire vidéo / Frame buffer                |

## Memory Map par périphérique

### CLINT

#### Version courte

| Registre  | Adresse    | Description                                                                                                  |
|-----------|------------|--------------------------------------------------------------------------------------------------------------|
| TIMER_CMP | 0x02004000 | Registre 64 bits contenant la valeur de référence pour le timer                                              |
| TIMER     | 0x0200bff8 | Registre 64 bits qui s'incrémente et déclenche une interruption lorsque sa valeur atteint celle de TIMER_CMP |


#### Version étendue

Chaque registre 64 bits du timer est accessible à travers 2 sous-registres 32 bits contenant les parties basse (LO) et hautes (HI) des registres 64 bits. 

| Registre     | Adresse    |
|--------------|------------|
| TIMER_CMP    | 0x02004000 |
| TIMER_CMP_HI | 0x02004004 |
| TIMER_CMP_LO | 0x02004000 |
| TIMER        | 0x0200bff8 |
| TIMER_HI     | 0x0200bffc |
| TIMER_LO     | 0x0200bff8 |


### PLIC

| Registre             | Adresse    | Description                                                                                                        |
|----------------------|------------|--------------------------------------------------------------------------------------------------------------------|
| PLIC_PENDING_1       | 0x0c001000 | Etat des lignes d'interruptions externes de 0 à 31                                                                 |
| PLIC_ENABLE_1        | 0x0c002000 | Masque d'autorisation des lignes d'interruptions externes de 0 à 31                                                |
| PLIC_THRESHOLD       | 0x0c200000 | Seuil de priorité général. Les interruptions avec un niveau de priorité inférieure au seuil général sont ignorées. |
| PLIC_IRQ_PRIORITY_0  | 0x0c000000 | Niveau de priorité de la ligne d'interruption externe 0                                                            |
| PLIC_IRQ_PRIORITY_1  | 0x0c000004 | Niveau de priorité de la ligne d'interruption externe 1                                                            |
| ...                  | ...        | ...                                                                                                                |
| PLIC_IRQ_PRIORITY_31 | 0x0c00007c | *Niveau* de priorité de la ligne d'interruption externe 31                                                           |
| PLIC_IRQ_CLAIM       | 0x0c200004 | Registre d'acquittement des interruptions externe.                                                                 |

### Périphériques divers

| Registre            | Adresse    | Description                                              |
|---------------------|------------|----------------------------------------------------------|
| PERIPH_LED          | 0x30000000 | Registre de contrôle et statut des LEDs                  |
| PERIPH_PUSH         | 0x30000004 | Registre de statut des boutons poussoirs et des switchs  |

# Mémoire
## Mémoire physique sur la machine CEP

La machine "cep" possède 32MiB de mémoire vive débutant à l'adresse physique 0x0.
Cette mémoire est initialisée au lancement de Qemu à partir des informations extraites du fichier `kernel.bin`.
Le fichier `./kernel/boot/cep/default.lds` organise la mémoire du système en regroupant les différentes sections des fichiers objets.
Ce script défini quelques variables qui permettront de se retrouver dans la mémoire lors de l’exécution du noyau. On retrouve ainsi:

* `_memory_start`
* `_text_start` < Code du noyau > `_text_end`
* `_rodata_start` < Variable constante comme les chaînes de caractères > `_rodata_end`
* `_data_start` < > `_data_end`
* `_bss_start` < Variable globale non initialisées > `_bss_end`
* `_heap_start` < Tas de 2MiB pour les allocations dynamiques > `_heap_end`
* `_free_memory_start` < Frame libre >
* `_memory_end`

L'espace libre situé entre les pointeurs `_free_memory_start` et `_memory_end` devra être utilisé pour fournir des blocs mémoire de 4kiB au noyau.
Pour cela, vous pourrez vous inspirer du fichier `./kernel/mem.c` qui utilise les pointeurs sur la heap pour fournir de la mémoire à l'allocateur dynamique `mem_alloc()`.

## Mémoire virtuelle sous RISC-V

### Glossaire

* Une adresse virtuelle est composée d'un VPN et d'un offset;
  * VPN : Virtual Page Number (Partie haute de l'adresse virtuelle);
  * offset : les 12 bits de poids faible de l'adresse, il représente le décalage dans la page VPN;
* PPN Physical Page Number, on peut parler aussi de numéro de page frame (cadre de page);
* ASID : Address Space Identifier 
* PTE Page Table Entry 
* SATP : Supervisor Address Translation and Protection register (_csr_);
* SUM : permit Supervisor User Memory access (champ du _csr_ `sstatus`).

### Mode de virtualisation

* _bare_;
* SV39;
* SV48.

Il existe également le mode SV32 avec ces deux niveaux de table de page, mais celui-ci n'est disponible que sous l'architecture 32 bits.
Le projet tournant en 64 bits, nous travaillerons avec le mode SV39 et ses trois niveaux de table.

***Il est impératif de lire la section SV32 de la norme RISC-V privilégiéer pour comprendre les mécanismes de virtualisation.
La section SV39 regroupe quand à elle, les caractéristiques qui la distingue du mode SV32.
Cette page ne fait que reprendre des points importants de la norme sans pour autant les expliquer, reportez-vous à à la spécification privilégiée pour plus de détail.***

#### SV39 : 

* Ce mode permet de travailler avec 512 GiB de mémoire virtuelle linéaire;
* adresse virtuelle :
  * composé de 39 bits utile  (VPN + offset),
  * étant codée sur 64 bits, celle-ci doit comporter les bits 63 à 39 tous égaux au bit 38 (correspond aux adresses virtuelles supérieurs à 256 GiB…),
  * Le VPN sur 27 bits est découpé en trois _sous-VPN_ de 9 bits, VPN0, VPN1 et VPN2.
Pour chacun des trois niveau de table des pages, une des trois parties du VPN ciblera une des entrée de la table appelée PTE,
* le parcours des PTE permet de convertir le VPN en un PPN de 44 bits.
*  l'offset sera ensuite ajouté au PPN pour construire l'adresse physique;
* 3 types de pages :
  * adressage d'une page de 4 KiB, feuille de dernier niveau,
  * adressage d'une megapage de 2 MiB, feuille dans la table  de second niveau,
  * adressage d'une gigapage de 1 GiB, feuille définie dans la table des pages de premier niveau;
  * ces trois types de page doivent forcement pointer vers une adresse physique alignée sur leur taille respective.

## Plan mémoire de l'OS lors de l’exécution d’un processus

Ce plan mémoire est donné à titre d'exemple, rien ne vous interdit d'en changer !
(Mais alors il faut changer les scripts d'édition de liens, ...)

* 0 -> 1 GiB : (0x00000000 → 0x3fffffff)
  * Code et data du kernel
  * heap kernel
  * mémoire vive libre
  * périphériques variés
* 1 GiB : Espace user : (0x40000000 → 0x7fffffff)
  * code et data du programme user
  * pile user
  * heap user
  * shared memory
* 2 GiB Espace mémoire vidéo (0x80000000 → 0x800384000)
  * Framebuffer 1028x720 en TrueColor (pixel sur 32 bits, ARGB)

# Travail à réaliser sur la mémoire
## Phase A : Distributeur de frame

Comme nous l'avons vue dans la section sur l'organisation de la mémoire physique, les pointeurs `_free_memory_start` et `_memory_end` nous donnent le début et la fin de la mémoire libre.
La mémoire virtuelle sous risc-v comme sous x86 est basée sur des pages mémoire de 4KiB.
C'est la plus petite unité utilisée pour les opérations de mappage mappage mémoire virtuelle / physique.
Ces pages de 4KiB doivent-être alignées sur 4KiB afin d'être adressées correctement par la MMU.

Il vous est donc demandé de découper l'espace libre de la mémoire en blocs de 4KiB et de fournir une API au noyau pour allouer et libérer des frames de 4KiB.

* `init_frames()`
* `void *get_frame()`
* `release_frame(void *)`

## Phase B : De la mémoire virtuelle mais rien ne change

Dans cette première phase, l'objectif va être d'activer la mémoire virtuelle et permettre au noyau de continuer à s'exécuter normalement.
Il n'y aura donc aucune différence de comportement vis-à-vis du noyau (les adresses restent identiques), c'est seulement au niveau du matériel qu'une translation 1:1 sera réalisée.
Dans le projet x86, la translation 1 pour 1 est déjà réalisée lors du boot du système, avec le projet RISC-V, c'est à vous de la mettre en œuvre.

1. Création du directory (table des pages de premier niveau) qui sera utilisé par l'ensemble des processus kernel.
Cette table devant être alignée sur 4 KiB, le distributeur de la phase A devrait vous aider (ou vous pouvez tricher un peu en utilisant `memalign(size_t alignment, size_t n); et `free(void*)` de `./lib/stdlib/malloc.c` en attendant le distributeur de frame du copain ;-P);
2. Création d'une feuille PTE gigapage kernel mappée en 1:1 qui contiendra la mémoire du noyau et les périphériques d'I/O;
3. Configurer la page table directory du kernel avec ce PTE;
4. Activer la mémoire virtuelle à partir du **csr** `satp` avec le mode SV39 et le PPN de votre directory;
5. Réaliser le test de validation.

### Validation de la phase B

Si votre code continue à s’exécuter après la configuration de `satp` c'est déjà bon signe.
Un exercice intéressant et un bon test est de mapper votre kernel sur une deuxième giga page.
Par exemple entre 4 et 5 GiB.
Ainsi il sera possible d'accéder à l'adresse physique 0x1000 à partir de l'adresse virtuelle 0x1000  et 0x100001000.

```
uint8_t variable = 42;
uint8_t *ptr1 = &variable;
uint8_t *ptr2 = ptr1 + 0x100000000;

printf("ptr1 @%p = %u\n", ptr1, *ptr1);
printf("ptr2 @%p = %u\n", ptr2, *ptr2);
if (*ptr1 == *ptr2)
    puts("Test mémoire virtuelle OK");
else
    puts("Test mémoire virtuelle FAIL");
```

## Phase C : Vers le monde user

Voir la mémoire virtuelle du projet x86.
Mais voici quelques détails relatifs au RISC-V.

### ASID

Ce champ ASID de `satp` permet d'attribuer un identifiant à un directory. Cette approche place cette étiquette dans le cache du mécanisme de traduction d'adresse (TLB), ce qui permet de conserver dans le TLB des traductions correspondant à différents processus (plus sur le TLB dans l'excellent cours d'architecture de 2A). Il n'est donc plus nécessaire d'invalider les entrées dans la TLB lors d'un changement de processus.

En effet, prenons le cas où deux processus possèdent le même ASID et qu'un changement de contexte survient entre eux.
Pour une adresse virtuelle du premier processus, le processeur avait convertit un VPN en un PPN.
Dans un soucis de performance, la translation mise en cache dans le TLB est étiquetée avec l'ASID présent dans `satp`.
Lors du changement de contexte, le plan mémoire (table des pages de premier niveau (directory)) doit être chargé dans `satp`.
Imaginons maintenant que le nouveau processus demande par hasard la même adresse virtuelle que l'ancien processus (scénario très probable), le processeur regarde dans sa TLB et trouve pour l'ASID du processus en cours une traduction valide du VPN, alors que ce sont deux processus différents.
Dans ce cas, le nouveau directory n'est pas parcouru et la traduction d'adresse est erronée.

Note : Qemu utilise le mécanisme des ASID pour accélérer la simulation (l'invalidation systématique lors d'un changement de `satp` qui était utilisée avant nécessite de parcourir la table des pages à la recherche de la traduction beaucoup plus souvent).
Étant donné que le champs ASID est sur 16 bits, et que notre OS ne fait pas (encore) tourner plus de 64K processes entre deux reboot, une bonne (et simple) solution consiste à utiliser le pid du processus comme ASID.
Cela peut même un peu aider lors du debuggage.

### SUM

Le champ SUM du registre `sstatus` indique si le superviseur peut accéder à une page de l'espace utilisateur.
Ce mécanisme de sécurité permet de limiter l'exploitation d'une faille qui pourrait potentiellement être présente au niveau du noyau.
En effet, une faille découverte au niveau des appels système peut conduire à la fuite d'informations protégées ou à l'exécution de code arbitraire dans un mode privilégié.

Le mécanisme SUM interdit au supervisor d'exécuter, de lire ou d'écrire dans de la mémoire user.
Dans ces conditions, il est donc impossible de faire exécuter au noyau un morceau de code forgé dans un programme user.
De même, il ne sera pas possible de récupérer de l'information dans une variable user.

Dans le cadre de ce projet, il vous est demandé de désactiver ce mécanisme de protection.
Si cela vous intéresse, la solution peut-être mise en œuvre dans le cadre de ce projet sous la forme d'une extension.
Dans ce cas, ce mécanisme devra être mis en place dans un second temps, une fois le kernel et le monde user fonctionnels. 
Notez que la nouvelle spec privilégiée (12-2 Draft) prévoit que la bit `SUM` ne soit plus pris en compte, très certainement pour éviter des trous de sécurité : _spectre_ et _meltdown_ sont passé par là !