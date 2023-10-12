# Projet PCSEA RISC-V

Bienvenue sur le projet de conception de système d'exploitation avancé sur architecture RISC-V 64.

## Xinul rules on RISC-V

Xinul  est le nom du projet original qui a lancé l'option RISC-V en PCSEA.
C'est de ce projet qu'a pu être extrait la base de travail sur laquelle votre projet pourra se construire.

### Récupérer la base du projet PCSERV

Vous avez normalement reçu un mail vous indiquant que vous pouviez clone votre dépôt git contenant de quoi démarrer votre projet
 
## Mise en place de l'environnement RISC-V

Afin de pouvoir compiler et lancer votre système d'exploitation il va falloir mettre en place un environnement de compilation et de simulation croisé RISC-V.
Dans ce but, plusieurs possibilités s'offre à vous: [Mettre en place un environnement de travail RISC-V](Environnement-logiciel).

Dans tous les cas, le projet peut-être compilé, exécuté et debuggé à distance en utilisant ssh.
Et si vous compilez vous même la *toolchain* vous aurez même le temps de lire la spécification RISC-V en grand détail. Mais faites cela en dehors des heures de projet, qui seront plus utilement employées à cuisiner les enseignants. 

Voir aussi les pages :

* Comment bien se servir de [GDB](GDB).
* Quelques infos sur [QEMU](QEMU).

## Travail demandé

Une fois l'environnement de travail RISC-V fonctionnel et le dépôt du groupe opérationnel le projet peut enfin commencer.

### Etape 0 : Prise en main de la spécification

La spécification RISC-V sera votre bible durant tout le projet, elle a un très gros avantage par rapport à l'ISA (Instruction Set Architecture) de x86, elle est beaucoup plus courte, alors n'ayez pas peur et allez lire la page [ISA RISC-V](ISA-RISC-V) qui vous aidera à entrer dans le sujet.

La spécification est divisée en deux volumes que vous pouvez trouver dans le dossier docs de votre projet :

- [Volume I User-Level ISA - V2.2](https://gitlab.ensimag.fr/pcserv/xinul/-/blob/master/docs/The%20RISC-V%20Instruction%20Set%20Manual%20-%20Volume%20I%20User-Level%20ISA%20-%20V2.2.pdf)
- [Volume II Privileged Architecture - V1.12-draft](https://gitlab.ensimag.fr/pcserv/xinul/-/blob/master/docs/The%20RISC-V%20Instruction%20Set%20Manual%20-%20Volume%20II%20Privileged%20Architecture%20-%20V1.12-draft.pdf)
 
Il est conseillé que chaque membre du groupe connaisse et comprenne bien une partie de la spécification RISC-V. Cela permettra, pour chaque partie du projet d'avoir un référent ISA qui pourra guider le travail et expliquer aux autres membres du groupe les points clés de la spécification.
Voici, de notre point de vue, les parties de la spécification RV64 à bien maîtriser :

* Registre généraux du processeur, qui est responsable de sauvegarder quoi et quand;
* Mécanismes [d'interruption et d'exception](Trap) '''machine''' et '''supervisor''' et comment les configurer;
* Mécanismes de passages entre les modes [machine](Modes) et [supervisor](Modes) (aller/retour), que ce passe t-il lors d'un `mret`et lors d'une [](trap);
* Plan mémoire de la machine [QEMU cep](Adressage-et-mémoire-virtuelle);
* Configuration du timer CLINT;
* [mémoire virtuelle](Adressage-et-mémoire-virtuelle) SV39;



### Etape 1 : Mode machine

Le but de ce projet est que vous puissiez avoir un point de vue global sur l'ensemble des couches d'un système d'exploitation du démarrage du processeur jusqu'aux programmes utilisateurs.
En x86, cet objectif ne serait pas réalisable dans le temps imparti, mais vous pouvez relever le défi avec l'architecture RISC-V.
Du coup autant partir depuis le démarrage du processeur et comprendre tout depuis le début.
La page [boot](boot) de ce wiki est là pour tout vous expliquer!

* Il est demandé de compléter le [boot](boot) du système en configurant le processeur pour passer en [mode supervisor](Modes).
* Mise en place d'un timer machine et préparer le timer supervisor : [gestion du temps](Trap);
* Découpage de l'espace mémoire en frame de 4k et implémenter un gestionnaire de frame


### Etape 2 : Mode supervisor

* Déléguer la [gestion du temps](Trap) au supervisor;
* Porter  les tests du dossier `src/user/tests` en espace du noyau;
* Mise en place d'un gestionaire de [traps](Trap) pour le mode supervisor;
* Mise en place de la [mémoire virtuelle](Adressage-et-mémoire-virtuelle) 1:1;
* Gestion des processus :
  * Phase 2 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap);
  * Phase 3 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap)
* Mise en place de l'[UART](UART-et-clavier) pour utiliser le clavier.
  * Phase 6 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap)
* files de messages et gestion de l'endormissement 
  * Phase 4 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap)

### Etape 3 : Mode user

* Mise en place du mode user
  * Phase 5 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap)
* implémenter un interprète de commandes
  * Phase 7 de la [roadmap d'ENSI-wiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap)

## Liens vers Ensiwiki

* [Projet de Conception de Système d'exploitation - Approfondissement — Ensiwiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_de_Conception_de_Syst%C3%A8me_d%27exploitation_-_Approfondissement);
* [projet système](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me);
* [road map](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_roadmap);
* [Mémoire virtuelle](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_m%C3%A9moire_virtuelle);
* [spécification](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_sp%C3%A9cification).

## Documentations

* [User-Level ISA Specification](https://gitlab.ensimag.fr/pcserv/documentation-risc-v/raw/master/riscv-spec-v2.2.pdf?inline=false);
* [Privileged ISA Specification](https://gitlab.ensimag.fr/pcserv/documentation-risc-v/raw/master/riscv-privileged.pdf?inline=false).
* Pour comprendre les noms des 32 registres et les conventions d'appels voir [RISC-V ELF psABI specification](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc)
* Pour la liste des pseudo-instructions facilitant (un peu) l'écriture de l'assembleur voir [Standard RISC-V Pseudo-instructions](https://github.com/riscv-non-isa/riscv-asm-manual/blob/master/riscv-asm.md#a-listing-of-standard-risc-v-pseudoinstructions)

### Dépôt de documentation
Vous trouverez dans ce wiki des liens externes pour compléter nos propos.
De plus, un dépôt `git` est associé à ce wiki.
Il regroupe les différentes documentations indispensables à la réalisation d'un projet sous RISC-V.

Attention ce dépôt contient des sous modules, utilisez la commande suivante pour le cloner :

`git clone --recurse-submodules git@gitlab.ensimag.fr:pcserv/documentation-risc-v.git`