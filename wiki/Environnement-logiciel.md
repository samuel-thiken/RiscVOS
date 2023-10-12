# Environnement logiciel pour architecture RISC-V

Afin de pouvoir travailler sur l'architecture RISC-V un environnement de compilation croisée et de simulation doit être mise en place.
Cette page a pour rôle de présenter les différents outils et leur procédure d'installation.

Quatre solutions sont proposées pour la mise en place de votre environnement de travail, de la plus facile à la plus complexe à mettre en œuvre:

1. Utiliser les machine de l'ENSIMAG (ssh possible);
2. Image Docker barbem/risc-v_cep;
3. Téléchargement d'une TArBall avec les outils RISC-V précompilés;
4. Compilation et Installation depuis les sources.

# Présentation des outils
## GNU toolchain

* Permet de compiler, débugger et étudier les binaire RISC-V depuis une autre architecture.
* Obligatoire dans le projet, il permet de compiler le kernel et les tests de validation.

## Simulation avec QEMU for CEP

* QEMU permet de virtualiser de nombreuses architectures et de lancer un système d'exploitation depuis multiboot, grub ou from scratch en simulant le comportement d'une machine et de ses périphériques.
* L'intégration de la cible RISC-V dans QEMU est récente, et de plus nous avons fait une version ad-hoc du système pour fournir un timer supervisor, un framebuffer et la capacité de quitter QEMU proprement lors d'un `exit` de votre OS.
Le QEMU de base disponible sur votre distrib ou depuis les sources officielles ne suffira donc pas pour le projet.
* Obligatoire dans le projet pour simuler l'architecture RISC-V dans le but de développer le boot et le kernel.

## Simulation avec Spike

Spike est le simulateur de référence de l'architecture RISC-V développé par Berkeley. Il est beaucoup plus lent que QEMU, aussi il n'apparaît ici que pour référence.

* Spike est un simulateur fonctionnel de la norme RISC-V;
* C'est une solution complémentaire pour tester votre système d'exploitation.
Il pourra vous aidez à trouver ou dénicher des erreurs dans votre implémentation.
Il vérifie en outre que les accès mémoires soient alignés, ce que ne fait pas QEMU (après, cela dépend de l''Execution Environment' du système RISC-V modélisé) ;
* Spike permet également de lancer des programme elf RISC-V directement sur votre ordinateur. (Pour cela PK est également nécessaire, voir ci après);
* Optionnel mais cela serait intéressant de lancer votre OS avec cette outils à des étapes clés de votre projet;
* Spike est maintenant livré avec la RISC-V GNU toolschain.

## Proxy Kernel (PK)

* Permet d'exécuter des binaire RISC-V directtement depuis votre host x86;
* Optionel dans ce projet;
* Projet Github [riscv/riscv-pk](https://github.com/riscv/riscv-pk)
* Le proxy-kernel est en phase de d'arrêt, et n'est plus maintenu

# Mise en place des outils du projet
## Méthode 1: Utilisation des postes de l'ENSIMAG

Les outils nécessaires au projet sont préinstallés sur les machines de l'ENSIMAG.
Il est possible de réaliser l'ensemble du projet à travers un terminal en `ssh` sur nos machines.

***La version de QEMU des postes Ensimag a été mise à jour!***

```
/matieres/4MMPCSEA/xinul
```

* Ce répertoire contient l'environnement de développement croisé, les programmes sont présents dans le sous dossier `bin`.
* Les outils de la GNU toolschain commencent tous par le préfixe `riscv64-unknown-elf-`.
* Vous avez également accès à une instance spécifique de QEMU, `qemu-system-risv64`. 
* Le Makefile de xinul a la capacité de détecter si vous êtes sur un poste de l'Ensimag.
Si le répertoire `/matieres` est trouvé, les outils RISC-V du sous dossier `bin` seront utilisés pour compiler et simuler xinul.
Vous n'avez donc pas à modifier votre variable d'environnement `$PATH` pour utiliser le Makefile.
* En revanche si vous voulez utiliser vous même les outils il peut-être intéressant d'ajouter ce répertoire à votre path (voir dans la section suivante pour modifier votre variable `$PATH`).

## Méthode 2: Image Docker

Si vous voulez éviter toute procédure compliquée et avoir un environnement qui marche à tout les coup, vous pouvez utiliser notre image Docker.
Visiter la [page Docker](docker) du wiki pour en apprendre d'avantage sur cette solution et son utilité dans le projet,

## Regroupement des outils et configuration de la Variables d'environnement PATH pour les méthodes 2 et 3

Il est conseillé pour des raison de clarté de rassembler l'ensemble des outils compilés dans un dossier spécifique. A titre d'exemple, nous utiliserons le dossier `/xinul` dans la suite de cette page. 

Première étape, créer le dossier et configurer les droits:

```
$ sudo mkdir -p /xinul
$ sudo chmod 777 /xinul
```

Afin que votre shell trouve automatiquement les binaire présents dans ce dossier il va falloir ajouter son chemin à la variable d'environnement `PATH`.

```
export PATH=/xinul/bin:$PATH
```

Pour éviter de ressaisir cette commande à chaque fois, vous pouvez l'ajouter à la fin du fichier de configuration de votre shell (`~/.bashrc` pour bash).

## Méthode 3: Utilisation de la tarball

Voici une tarball contenant les outils de la GNU toolschain pour RISC-V ainsi que QEMU for CEP.
Ces outils ont été extrait depuis l'image Docker `barbe/risc-v_cep` basée sur Debian:

* [xinul-toolschain-debian-x86-2022.tar](https://www.dropbox.com/s/bq0ke42nd4a17kg/xinul-toolschain-debian-x86-2022.tar.gz?dl=1);
* Cette archive contient un dossier racine `xinul` avec les outils;
* Le dossier `xinul` contient lui même un dossier `bin` avec les binaires, votre variable `PATH` doit pointer sur ce dossier.
* Voir les procédures de tests présenter dans la troisième méthode pour vérifier votre installation.

```
wget https://www.dropbox.com/s/bq0ke42nd4a17kg/xinul-toolschain-debian-x86-2022.tar.gz?dl=1
tar xf xinul-toolschain-debian-x86-2022.tar.gz
PATH=$(realpath ./xinul/bin):$PATH
```

# Méthode 4: Mise en place manuel de l'environnement de travail

L'installation d'un tel environnement n'est pas triviale et nécessite la configuration et la compilation de plusieurs projets.

## GNU toolschain
### installation depuis les paquets

Il existe des versions pacagée de `gcc` pour RISC-V sur différents système d'exploitation.
Dans votre distribution, La version de `gcc` packagée est peut-être un peu ancienne et ne pourra peut-être pas reconnaître les nouveaux registres CSR de la spécification.
De plus, certain outils de la GNU toolchain comme `gdb` ne sont pas forcement présents.

Si vous avez Debian vous pouvez consulter la page [RISC-V du wiki](https://wiki.debian.org/RISC-V) afin de voir l'avancement du portage des outils.

### Installation depuis les sources
#### Dépendances

Sur Debian :
```
$ sudo apt-get update
$ sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```

Sur Fedora/CentOS/RHEL OS :
```
$ sudo yum install -y git make autoconf automake python3 libmpc-devel mpfr-devel gmp-devel gawk  bison flex texinfo patchutils gcc gcc-c++ zlib-devel expat-devel
```

#### Compilation et installation

```
$ git clone https://github.com/riscv/riscv-gnu-toolchain
$ cd riscv-gnu-toolchain/
$ ./configure --prefix=/xinul
$ make
```


### Vérification de l'installation

L'exemple cross-compile du projet permet de vérifier l'installation de la compilation croisée.

```
$ cd ./examples/cross-compile
$ make
riscv64-unknown-elf-gcc main.c -o main
file main
main: ELF 64-bit LSB executable, UCB RISC-V, version 1 (SYSV), statically linked, not stripped
```

Le projet doit compiler et l'outil `file` doit stipuler que le programme est un exécutable pour l'architecture RISC-V.

### Problèmes rencontrés et solutions

* Attention de ne pas partir avec le dépôt `riscv-tools` mais bien le dépôt `riscv-gnu-toolchain`.

* Si votre version de `gcc` est trop récente par rapport au dépôt `riscv-binutils`, vous aurez peut-être des problèmes de compatibilité.  
Nous en avons notamment rencontré un avec le fichier `gdb/python/python.c` auquel il a fallu ajouter manuellement le correctif suivant : [https://github.com/bminor/binutils-gdb/commit/aeab512851bf6ed623d1c6c4305b6ce05e51a10c](https://github.com/bminor/binutils-gdb/commit/aeab512851bf6ed623d1c6c4305b6ce05e51a10c)

* Il est également à noter que si votre version de guile est trop récente (>=2.2),il peut être nécessaire de configurer l'installation avec le flag `--with-guile=no` de manière à éviter tout conflit.

* En cas de problème à l'installation de `gdb`, vous pouvez essayer d'ajouter le flag `--target=riscv64`.

## QEMU for CEP
### Installation depuis les paquets

Impossible, car notre machine est une machine ad-hoc adaptée aux besoins du projet.

### Installation depuis les sources

* Projet sur le gitlab de l'Ensimag [qemu-cep](https://gitlab.ensimag.fr/pcserv/qemu-cep).

***Attention, il faut passer sur la branche `xinul`.***

#### Dépendances

Sur Debian
```
$ sudo apt-get update
$ sudo apt-get install -y git python libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev libsdl1.2-dev
```

Sur Fedora/CentOS/RHEL OS:
```
$ sudo yum install -y git glib2-devel libfdt-devel pixman-devel zlib-devel libaio-devel libcap-devel libiscsi-devel bzip2
```

#### Compilation et installation

```
$ git clone -b xinul git@gitlab.ensimag.fr:pcserv/qemu-cep.git
$ cd qemu-cep
$ ./configure --prefix=/xinul --target-list=riscv64-softmmu
$ make
$ make install
```



### Vérification de l'installation

Remarque : Pour vérifier l'installation de QEMU il faut avoir préalablement installé les outils de compilation croisés présents sur cette page.

Depuis le top du dépôt :
```
$ cd examples/reduce_probe
$ make
$ qemu-system-riscv64 -nographic -machine spike_v1.10 -kernel build/bin/rv64imac/spike/hello
```

Hello devrait apparaître.
