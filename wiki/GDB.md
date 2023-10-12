# GDB
## Installation de GDB

GDB fait parti de la GNU toolchain, il faut impérativement utiliser une version RISC-V pour le projet.
rendez-vous sur la page [](Environnement-logiciel) pour l'installer si ce n'est déjà fait.

## Connexion automatique entre GDB et Qemu

Lors du démarage de Qemu en mode debug, celui-ci ouvre le port 1234 en attente de GDB.
Il faut normalement, au lancement de GDB réaliser une connexion vers ce port.
Dans le projet et à partir de la commande `make gdb` ou `make dk-gdb` la connexion est automatique.


Remarque : vous pouvez ajouter des commande GDB à la fin du fichier `./src/gdbinit` qui seront automatiquement exécutées après la connexion à Qemu. Très utile pour réaliser un scénario de debug.

## Lancement de GDB

1. Se rendre dans le dossier `./src/`.
2. `make run`pour compiler le kernel et le lancer avec QEMU qui attendra GDB pour démarrer. Ou `make dk-run` avec Docker.
3. Dans un second terminal, `make gdb` ou `make dk-gdb` lance GDB, se connecte à Qemu et exécute le fichier `./src/gdbinit`.
4. Debug.

## Problèmes connus avec GDB

* Les 'watchpoints'  qui permettent de surveiller une adresse ou une zone mémoire particulière ne fonctionne pas encore sous RISC-V.
* Si vous avez des problème du type registre non disponible lors du positionnement d'un break point, il se peut que vous n'ayez pas la bonne version de QEMU, voir la page [](Environnement-logiciel).


## Documentation sur GDB


* [Déboguer son programme avec GDB - OpenClassrooms](https://openclassrooms.com/courses/deboguer-son-programme-avec-gdb)
* ***Très bon article*** : [Dix choses qui ne sont faisables qu'avec GDB](http://devarea.developpez.com/dix-choses-faisables-avec-GDB/)
* [Introduction à la rétroingénierie de binaires](https://zestedesavoir.com/articles/97/introduction-a-la-retroingenierie-de-binaires/)
* Pour voir le code source (ASM ou C) dans GDB voir [TUI mode](https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html)