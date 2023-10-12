FROM debian

LABEL maintainer="Mathieu Barbe <mathieu@barbe.io>"
LABEL Description="This image is used to cross compile and run Qemu for RISC-V project"

# Set RISC-V env
ENV RISCV=/xinul
ENV PATH=$PATH:$RISCV/bin

#
# Dependencies
#
RUN apt-get update && \
    apt-get install -y --no-install-recommends --no-install-suggests \
        git ca-certificates autoconf automake autotools-dev curl \
        libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison \
        flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev && \
    apt-get autoclean && \
    mkdir -p $RISCV /src

#
# RISCV GNU tool chain
#
RUN cd /src && \
    git clone https://github.com/riscv/riscv-gnu-toolchain && \
    cd riscv-gnu-toolchain && \
    ./configure --prefix=$RISCV && \
    make -j8 && \
    rm -r /src/riscv-gnu-toolchain

#
# Spike
#
RUN apt-get update && \
    apt-get install -y device-tree-compiler && \
    apt-get autoclean && \
    cd /src && \
    git clone https://github.com/riscv/riscv-isa-sim && \
    cd riscv-isa-sim && \
    ./configure --prefix=$RISCV --with-fesvr=$RISCV && \
    make -j8 && \
    make install && \
    rm -r /src/riscv-isa-sim

#
# Qemu RISC-V CEP machine
#
RUN cd /src && \
    apt-get update && \
    apt-get install -y git python libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev libsdl1.2-dev ninja-build && \
    apt-get autoclean && \
    git clone --depth 1 https://gitlab.com/barbem/qemu_for_cep.git --branch=xinul && \
    cd qemu_for_cep && \
    ./configure --prefix=$RISCV --target-list=riscv64-softmmu | tee $RISCV/qemu-configure-log.txt && \
    make -j8 && \
    make install && \
    rm -r /src/qemu_for_cep

# Expose port to connect GDB and Qemu
EXPOSE 1234
