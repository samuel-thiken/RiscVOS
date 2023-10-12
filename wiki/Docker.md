# Docker

L'image Docker attaché au projet Xinul offre un environnement stable de compilation et de simulation pour notre projet RISC-V.
Cette image pourra également servir à l'automatisation des tests de non régression à travers le pipeline de Gitlab.

Cette image permet avant tout une compilation croisée et un lancement rapide de l'OS sans altérer le système host.

## Comment créer et utiliser l'image Docker du projet Xinul

Voir le fichier [xinul/docker/Readme.md](https://gitlab.ensimag.fr/pcserv/xinul/blob/master/docker/README.md) du projet Xinul.

L'image Docker risc-v_cep est hébergée sur le hub du projet Docker à l'adresse <https://hub.docker.com/repository/docker/barbem/risc-v_cep>

Cette image est basée sur Debian et contient :

* ridcv-gnu-toolchain
* riscv-spike
* riscv-fesvr
* riscv-qemu

## Astuce: Changer le répertoire de sauvegarde des images docker :

Si votre montage /var n'a plus d'espace libre pour contenir vos images Docker, vous pouvez changer le répertoire par défaut.

Dans le fichier /etc/systemd/system/docker.service.d/docker-storage.conf

```bash
[Service]
ExecStart=
ExecStart=/usr/bin/dockerd --data-root=/home/benoit/.docker -H fd://
```

Source (https://github.com/IronicBadger/til/blob/master/docker/change-docker-root.md)
