L'aide de QEMU s'obtient avec `qemu-system-risc64 --help`, mais il y a beaucoup de choses, et il n'est pas évident de savoir ce qui est réellement utile dans notre cas. Cette page tente de vous aider à y voir clair.

Dans notre cas, et pour tout le projet sauf extensions graphiques, on l'utilisera comme suit:
```
$ qemu-system-risc64 -machine cep -bios none -nographic -kernel build/kernel.bin
```
Pour permettre à gdb de se connecter à QEMU, on ajoutera les options `-s -S`. Si le clavier n'a pas la bonne configuration, on peut ajouter `-k fr`.

Pour quitter QEMU, il faut taper `<CTRL-A>x`.
On peut avoir accès au "moniteur" de QEMU, tapez `<CTRL-A>c`. C'est particulièrement utile pour voir l'état des registres du processeur lors de plantages du noyau, ce qui se fait grâce à `info registers`. Mais il y a plein d'autres utilisations, visibles grâce à `help`. On peut retourner au simulateur avec la même séquence de touches.

Lorsque l'on désire utiliser le frame buffer, la ligne de commande préconisée est :
```
$ qemu-system-risc64 -machine cep -bios none -sdl -serial mon:stdio -kernel build/kernel.bin
```
L'option `-serial mon:stdio`permet d'avoir l'uart fonctionnelle sur la console en même temps que la fenêtre graphique.

Tout cela est disponible dans le Makefile à la racine des sources, par les appels suivants :
```
$ make go          # exécution sur la console uniquement
$ make debug       # exécution sur la console avec attente de connexion gdb
$ make go_graphic  # exécution graphique et console
$ make gdb         # lance un gdb qui se connecte à QEMU
```