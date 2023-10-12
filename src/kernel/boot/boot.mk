#
# Projet PCSEA RISC-V
#
# Mathieu Barbe <mathieu@kolabnow.com> - 2019
#
# See license for license details.
#

BOOT_OBJS = \
	boot/common/boot.o \
	boot/common/crtm.o \
	boot/$(MACHINE)/setup.o \
