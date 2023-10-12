#
# Docker RISC-V Makefile
# 
# Mathieu Barbe <mathieu.barbe@grenoble-inp.org> - 2018 - 2020
# See LICENSE for license details.
#

IMAGE_NAME = barbem/risc-v_cep
DOCKERFILE_DIR = .
UID = $(shell id -u)
GID = $(shell id -g)
DK_SHARE_DIR = $(PWD)
DK_WORKING_DIR = /src
RUN_CMD = docker run
RUN_OPTS = \
-ti \
-u $(UID):$(GID) \
--rm \
-v $(DK_SHARE_DIR):$(DK_WORKING_DIR) \
-w $(DK_WORKING_DIR)

build: .image-built

.image-built: $(DOCKERFILE_DIR)/Dockerfile Makefile
	docker image build -t $(IMAGE_NAME) $(DOCKERFILE_DIR)
	touch .image-built .image-available

.PHONY: pull
pull:
	docker pull $(IMAGE_NAME):latest
	touch .image-available

dl: .image-available
.image-available:
	docker pull $(IMAGE_NAME):latest
	touch .image-available

bash:.image-available
	$(RUN_CMD) $(RUN_OPTS) $(IMAGE_NAME) /bin/bash

# Exécuter une commande avec un container
# DK_OPTS et CMD défnie par l'appelant
exec: .image-available
	$(RUN_CMD) $(RUN_OPTS) $(DK_OPTS) $(IMAGE_NAME) $(CMD)

clean:
	docker image rm $(IMAGE_NAME); \
	rm -f .image-built .image-available
