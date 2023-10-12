#
# Projet PCSEA RISC-V
#
# Mathieu Barbe <mathieu@kolabnow.com> - 2019
#
# See license for license details.
#
.globl strap_entry
.text
    .align 4
strap_entry:
    # On bascule sur la stack machine (la même pour tout le monde)
    csrrw sp, sscratch, sp
    # On alloue la struct trap_frame sur la pile
    addi sp, sp, -37*8
    # Sauvegarde minimaliste du contexte (registre à la charge du caller)
    sd ra, 1*8(sp)
    sd t0, 5*8(sp)
    sd t1, 6*8(sp)
    sd t2, 7*8(sp)
    sd a0, 10*8(sp)
    sd a1, 11*8(sp)
    sd a2, 12*8(sp)
    sd a3, 13*8(sp)
    sd a4, 14*8(sp)
    sd a5, 15*8(sp)
    sd a6, 16*8(sp)
    sd a7, 17*8(sp)
    sd t3, 28*8(sp)
    sd t4, 29*8(sp)
    sd t5, 30*8(sp)
    sd t6, 31*8(sp)

    csrr t0, scause // on le met dans t0 pour le comparer
    li t1, 8
    beq t0, t1, syscall_handler # si c'est un syscall, on va dans le handler de retour de syscall

    csrr    a0, scause
    csrr    a1, sepc
    mv a2, sp
1:  jal strap_handler

restore:
    # Restauration des registres
    ld ra, 1*8(sp)
    ld t0, 5*8(sp)
    ld t1, 6*8(sp)
    ld t2, 7*8(sp)
    ld a0, 10*8(sp)
    ld a1, 11*8(sp)
    ld a2, 12*8(sp)
    ld a3, 13*8(sp)
    ld a4, 14*8(sp)
    ld a5, 15*8(sp)
    ld a6, 16*8(sp)
    ld a7, 17*8(sp)
    ld t3, 28*8(sp)
    ld t4, 29*8(sp)
    ld t5, 30*8(sp)
    ld t6, 31*8(sp)
    addi sp, sp, 37*8
    csrrw sp, sscratch, sp
    sret

syscall_handler:
    mv t2, a7
    slli t2, t2, 3
    la t0, syscall_table
    add t0, t0, t2
    ld t0, 0(t0)
    jalr ra, t0
    csrr t0, sepc
    addi t0, t0, 4
    csrw sepc, t0
    j restore