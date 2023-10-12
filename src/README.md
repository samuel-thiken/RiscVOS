# Source du projet PCSEA RISC-V

Point d'entré du projet sur le wiki <https://gitlab.ensimag.fr/pcserv/documentation-risc-v/wikis>

## Dossiers des sources

* user/ : espace user de l'OS, ils sont compilés seul et intégrés au kernel ensuite.
    * ./*****/ : un programme user est placé dans un dossier et doit être ajouté au Makefile user (APPS_NAME).
    * ./tests/ : les programmes de test des enseignants qu'il faudra porter en espace kernel au début du projet.
    * ./ulib/ : la librairie spécifique à l'espace user.
    * ./build/ : outils pour le Makefile user.
    * ./Makefile : permet de compiler l'espace user du projet.
* kernel : espace kernel du projet.
    * ./boot/
        * ./common/ : contient le point d'entrée du kernel et des fichiers liés au boot commun a toutes les machines.
        * ./*MACHINE*/ : dossier pour la machine MACHINE particulière.
            * default.lds : script de lien du kernel.
            * setup.c : configuration spécifique à la machine (UART, poweroff, …).
        * ./boot.mk : inclue par le Makefile kernel, il informe sur les objets a générer pour le boot.
    * ./build/ : dossier des outils utiles au Makefile kernel.
    * ./Makefile : permet de compiler le kernel du projet.
* lib : notre librairie standard (utilisée par le monde user et kernel)
    * ./include/ : fichiers d'en-tête de la lib.
    * Un dossier par `.h` qui contient un fichier `.c` par fonction contenue dans le header.
* build : sortie de la compilation
* gdbinit : les commandes présentent dans ce fichier seront exécutées au lancement de GDB (pratique pour un scénario de debug à rejouer).
* toolchain.config : configuration du nom des outils de la toolchain.
* riscv_flags.config : options de compilation et d'édition de lien (utilisés dans lib, user et kernel).
* Makefile : permet de compiler l'ensemble du projet et bien plus (voir section target).

## Makefile

L'ensemble des fichiers de sorties sont placés dans le dossier build.

### Targets

`make target`

#### Compilation

* clean : supprime l'ensemble des fichiers compilés;
* all : compile l'espace user puis l'espace kernel;
* user : compile seulement l'espace user;
* kernel : compile le kernel sans recompiler l'espace user mais prend quand même les programmes user précédemment générés.

#### Simulation

* go : lance le kernel avec Qemu dans le terminal. (Dépendance : target all);
* debug : lance le kernel avec Qemu dans le terminal et attend la connexion avec GDB. (Dépendance : target all);
* gdb : lance le debuger qui va se connecter à Qemu.
Une fois la connexion établie, les commandes du fichier `./gdbinit` seront évaluées et la main vous sera restituée afin d'entrer des commandes complémentaires.

#### Docker

* dk-image-dl ou pull : télécharge l'image raifer/riscv depuis la plateforme Dockerhub. Moins de 2Go, très rapide;
* dk-image-build : construire l'image Docker à partir du Dockerfile `../docker/Dockerfile`. ***Atxxtention très long, plusieurs heures***;
dk-image-rm : supprime l'image riscv de Docker;
* dk-bash : ouvrir un bash dans un container riscv;
* dk-all : compile le kernel dans un container, les fichiers de sortie seront toujours dans le dossier `./build/`;
* dk-go : lance le kernel avec Qemu dans un container (Dépendance : target all);
* dk-debug : lance un container avec Qemu dans une optique de debug. Qemu attend gdb pour continuer. Le port 1234 de la machine host est mapé avec Qemu (Dépendance : target all);
* dk-gdb : lance un second container avec gdb et se connecte au port 1234 du host.



Pour des informations complémentaires, voir :
* le Readme dans le dossier `../docker/`;
* la page dédiée sur le [wiki](https://gitlab.ensimag.fr/pcserv/documentation-risc-v/wikis/docker).
