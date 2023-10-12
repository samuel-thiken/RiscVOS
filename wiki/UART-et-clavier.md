## Les interruptions clavier

## Glossaire

* UART : Universal Asynchronous Receiver Transmitter, une liaison série à connaître. [UART sur Wikipedia](https://fr.wikipedia.org/wiki/UART);
* PLIC : Platform Level Interrupt Controller.

## Introduction

Sous le projet RISC-V on ne travaille pas avec un ordinateur complet, mais plutôt avec une petite carte de développement.
Il n'est pas possible de brancher un clavier PS2 directement sur la machine CEP que nous utilisons dans le projet.
En revanche, cette plateforme possède une liaison série UART bi-directionnelle.

En effet, les messages qui apparaissent à l'écran sont envoyés/reçu grâce à un composant UART présent sur la plateforme CEP.

Le processeur riscv simulé par Qemu, à l'autre bout, récupère ces informations et les affiche à l'écran.

De façon analogue, Qemu récupère les caractères entrés au clavier et les envoie sur la liaison série en direction de ce même composant.

C'est ce qui est fait par défaut si l'option `-nographic` est utilisée à l'invocation de Qemu, sinon il faut utiliser `-serial mon:stdio` pour que l'UART simulée soit connectée à la console qui a lancé Qemu.

## UART

Afin d'avoir un `printf` fonctionnel dès le début du projet, l'UART a déjà été configurée.
Cette action est réalisée dans le fichier `./kernel/boot/cep/setup.c` appelé au début du boot.
Il configure en outre la vitesse de l'UART et l'emplacement i.e. l'adresse) en mémoire du composant de liaison.
Après la configuration, il est possible d'écrire un caractère grâce à `console_dev->putchar(c)`, qui s'assure qu'on ne perd aucun caractère (par _polling_, ... mais la fifo ne devrait jamais être pleine), et de lire un caractère avec `console_dev->putchar(c)`, qui retourne une valeur négative si aucun caractère n'a été tapé au clavier, et le code ascii du caractère sinon.

```
auxval_t __auxv[] = {
		{UART0_CLOCK_FREQ,       32000000},
		{UART0_BAUD_RATE,        115200},
		{CEP_UART0_CTRL_ADDR,    0x10013000},
		{CEP_TEST_CTRL_ADDR,     0x10000000},
		{0,                      0}
};
```

Pour information, `auxval` est une implémentation d'un petit `device tree`, un dictionnaire (clef, valeur) utilisé pour les périphériques d'I/O dans le projet.

## Récupération d'un caractère

Comme nous l'avons vue, un caractère tapé au clavier sous Qemu sera envoyé vers la plateforme CEP à travers une liaison série.
Cette trame série sera décodée en un caractère ASCII par le composant UART de la plateforme CEP.
Il sera nécessaire de prévenir le processeur de l'arrivée de cette nouvelle information afin que celui-ci la traite.

Pour ce faire, le composant UART est connecté sur une des sources du PLIC. Ce dernier est en charge d'orchestrer les différentes demandes d'interruption et, le cas échéant, de prévenir le processeur qu'une interruption externe est en attente (bit 11 de `mip`).

Note : Le mécanisme du timer à travers le CLINT n'a rien à voir avec celui du PLIC.
En effet, le timer possède sa propre entrée d'interruption sur le processeur (bit 7 de `mip`). 

### Travail demandé

* Configuration du PLIC dans le but de générer une interruption externe vers le processeur lors de l'arrivé d'un caractère sur la liaison série;
* Création d'un traitant d'interruption spécifique au PLIC (interruption externe);
* Distinguer la cause de l'interruption externe;
* dans le cas d'une interruption externe du composant UART, récupérer le caractère transmit.

### Configuration du PLIC 

Par chance, pour cette année, la configuration du PLIC est déjà implémentée dans `kernel/drivers/cep/cep_plic.c`.
Vous trouverez la _data-sheet_ qui décrit son fonctionnement [ici](https://github.com/riscv/riscv-plic-spec/blob/master/riscv-plic.adoc).
Il est nécessaire de la lire avec attention pour en comprendre le fonctionnement.

Le PLIC enregistre un certain nombre d'émetteurs d'interruption (les sources) et de destinataires d'interruption (les cibles).
On peut noter qu'un processeur RISC-V dispose de deux cibles : une cible représentant le processeur en mode machine et une cible représentant le processeur en mode superviseur.
Dans notre cas nous utiliserons la cible superviseur.

Sur notre machine CEP, les adresses de configuration du PLIC sont les suivantes (source : [ici](https://static.dev.sifive.com/FE310-G000.pdf), page 30) :

| Nom | Adresse |
| ------ | ------ |
| Base PLIC | 0x0c000000 |
| Vecteur d'activation (offset) | 0x2000 | 
| Configuration des cibles (offset) | 0x200000 |
| Configuration des sources (offset) | 0x0 |

* Le signal d'interruption de l'UART est relié sur la source 1 du PLIC;
* la cible 0 du PLIC est reliée au processeur sur l'entrée des interruptions externes - mode machine;
* chaque cible (machine ou supervisor) dispose d'un vecteur d'activation des sources.
Il faudra activer les interruption UART dans ce vecteur;
* il faut ensuite paramétrer la priorités de notre source : dans notre cas, comme nous ne traiterons qu'une seule source, on peut utiliser n'importe quel nombre strictement supérieur à 0;
* finalement, il faut configurer les seuils de priorité pour chaque cibles : toutes les interruptions qui arrivent avec une priorité strictement supérieure à ce seuil seront envoyées à la cible.
Pour faire simple, il est possible d'utiliser 0.

### Côté OS

Du côté de l'OS, il s'agit d'une interruption externe.
Il faudra que celle-ci soit traitée en mode *supervisor*.

## Traitement du caractère

L'avantage de l'UART, à la différence du clavier comme en x86, c'est qu'il n'y a pas besoin de décoder la touche, on obtient directement le caractère frappé sous Qemu.
Vous pouvez donc vous lancer directement dans la gestion des *buffers* décrit dans la section `La lecture de caractères dans le projet` de la page [Projet système : Aspects techniques — Ensiwiki](https://ensiwiki.ensimag.fr/index.php?title=Projet_syst%C3%A8me_:_Aspects_techniques).
