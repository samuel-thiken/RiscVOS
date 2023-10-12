# Processus et programme user

Les mécanismes des processus et des programmes user sont globalement identiques avec le projet x86.
Le but et le travail demandé sont identiques, cependant, quelques différences architecturales sont à prendre en comptes.

## Création d'un processus

## Les piles

## Changement de contexte

Un exemple de changement de contexte en mode machine ou supervisor est donné dans `examples/basic-context`. Vous pouvez l'utiliser tel quel pour la création des processus en mode supervisor sans mémoire virtuelle. En revanche, il faudra le modifier pour gérer la mémoire virtuelle et le mode user.

## Démarrage d'un processus

## Appel système