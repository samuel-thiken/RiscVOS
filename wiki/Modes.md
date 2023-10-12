# Les modes de l'architecture RISC-V

L'architecture RISC-V dispose de 3 modes différents :
* Le mode machine (00) : tous les accès sont autorisés, il n'y a pas de mémoire virtuelle
* Le mode superviseur : mode privilégié avec pagination active
* Le mode utilisateur : mode non privilégié

Dans le projet on utilisera M+S+U. Le mode machine est obligatoire car c'est dans celui-ci que la machine démarre. Les modes utilisateur et superviseur sont utilisés pour réaliser l'OS avec mémoire virtuelle et toutes les protections associées.

# Mode machine

Le mode machine fait partie des trois modes principaux avec les modes supervisor et user.
C'est le mode de plus bas niveau et celui qui offre donc le plus de privilège.
Depuis le démarrage du processeur et jusqu'à l'instruction `mret` c'est le mode machine qui opère.

## Particularité du mode machine

* Mode bas niveau;
* configuration du hardware
* accès possible à tout les CSR;
* bare metal (mémoire virtuelle désactivé);
* les interruptions peuvent-être complémentent désactivées.

# Mode supervisor

Le mode supervisor fait partie des trois modes principaux avec les modes machine et user.
C'est le mode intermédiaire de la norme.
Il gère le mode user et repose sur le mode machine.
Pour communiquer  avec le mode machine, le superviseur peut utiliser le mécanisme d'appel système : supervisor binary interface (SBI).

C'est dans le mode supervisor que les processus et les fonctionnalités du système d'exploitation sont gérés.

Dans notre projet, la première fonction exécutée en mode supervisor est la fonction `kernel_start()`du fichier `./src/kernel/start.c`.

## Particularité du mode supervisor

* Mode intermédiaire;
* configuration du système d'exploitation;
* Répond aux appels systèmes du monde user;
* Utilise la SBI pour communiquer avec le mode machine;
* accès possible aux csr supervisor et user;
* Mémoire virtuelle ou baremetal (par défaut baremetal);
* Protection de la mémoire physique (PPM) obligatoire si prise en charge par le processeur;

# Mode user

Le mode user fait partie des trois modes principaux avec les modes machine et supervisor.
C'est le mode le moins privilégié de la norme, il est utilisé pour exécuter des programmes utilisateurs.
Ces programmes user n'ont pas accès aux ressources du mode supervisor : kernel, périphériques d'entrées sorties et configuration du système.
Dans le but d'utiliser les périphériques et de communiquer avec le kernel depuis le monde user, des appels systèmes devront être mis en place durant le projet (c.f. [Spécifications](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_sp%C3%A9cification)).

Pour communiquer  avec le mode supervisor, le monde user peut utiliser les appels systèmes que vous devrez mettre en place durant le projet.

## Particularité du mode user

* Mode comportant le moins de privilèges;
* Utilise les appels systèmes pour communiquer avec le mode supervisor;
* accès possible aux csr user;
* Mémoire virtuelle obligatoire;