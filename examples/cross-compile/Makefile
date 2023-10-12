# a minimal example to test X-compile in host or in Docker container

DK_IMAGE_NAME = barbem/risc-v_cep
DK_DOCKERFILE_DIR=../../docker
DK_MNT_DIR=/mnt/host
DK_RUN_CMD=docker run --rm -v ${PWD}:${DK_MNT_DIR} -w ${DK_MNT_DIR} ${DK_IMAGE_NAME}

CC=riscv64-unknown-elf-gcc

all:main
	file main

dk-pull:${DK_DOCKERFILE_DIR}/Dockerfile
	docker pull ${DK_IMAGE_NAME}

dk-all:
	${DK_RUN_CMD} make all

main:main.c
	${CC} main.c -o main

clean:
	rm -f main
