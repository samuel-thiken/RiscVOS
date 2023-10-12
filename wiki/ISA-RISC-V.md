# ISA RISC-V

Les deux volumes de la spécification sont présents sur le dépôt attaché à ce wiki :

* [User-Level ISA Specification](https://gitlab.ensimag.fr/pcserv/documentation-risc-v/raw/master/riscv-spec-v2.2.pdf?inline=false);
* [Privileged ISA Specification](https://gitlab.ensimag.fr/pcserv/documentation-risc-v/raw/master/riscv-privileged.pdf?inline=false).
* [psABI: function calling conventions](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-cc.adoc)

Les sources de la spec sont également sur le projet Github [isa-manual](https://github.com/riscv/riscv-isa-manual) de RISC-V.
Ce projet est également présent dans le dépôt attaché à ce wiki sous forme de sous module.
Dans celui-ci les specs sont présentes sous forme de source LaTeX, pour les spécifications encore en discussion, et de release pdf pour celles qui ont été figées à une date précise.
Attention, nous utilisons la spec superviseur telle que présente dans nos documents, hors la 1.12 vient de sortir, donc ne prenez pas cette dernière (nous n'avons pas vérifié la compatibilité ascendante).

La psABI (Processor Specific Application Binary Interface) contient les conventions d'appel de fonction, et en particulier les registres dont on sait qu'ils seront sauvés par le compilateur avant un appel (utile pour le changement de contexte) et ceux dont on sait qu'ils seront sauvés et restauré par le compilateur dans une fonction (utile pour la gestion des interruptions).

## Extensions

Les spécifications de base 32 et 64 bits peuvent être « étendues » de façon variées, mais toujours conformément à la référence edictée par le consortium.
Une implémentation donnée prendra en charge une partie de ces extensions.

Ces extensions sont abrégées par une seul lettre et en voici une liste non exhaustive :

* I : manipulation des nombres entiers, Integer;
* M : multiplication et division de nombres entiers;
* F : manipulation des nombre flottants;
* D : manipulation des nombre à doubles précisions;
* A : opération atomique;
* C : utilisation d'un jeu d'instruction compressé;
* S : mode supervisor;
* …

Voir le document de spécification pour une liste exhaustive des extensions.


## Quelques pages du wiki abordant des points de la spec RISC-V

* [Mode machine](Modes);
* [Mode dupervisor](Modes);
* [Mode user](Modes);

* [Trap : interruption et exception](Trap).


## Trap