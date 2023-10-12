#include "syscall_num.h"

.text
.globl chprio
.align 2
chprio:
    addi a7, zero, SYSC_chprio
    ecall
    ret


.globl clock_settings
.align 2
clock_settings:
    addi a7, zero, SYSC_clock_settings
    ecall
    ret


.globl cons_write
.align 2
cons_write:
    addi a7, zero, SYSC_cons_write
    ecall
    ret


.globl current_clock
.align 2
current_clock:
    addi a7, zero, SYSC_current_clock
    ecall
    ret


.globl exit
.align 2
exit:
    addi a7, zero, SYSC_exit
    ecall
    ret


.globl getpid
.align 2
getpid:
    addi a7, zero, SYSC_getpid
    ecall
    ret


.globl getprio
.align 2
getprio:
    addi a7, zero, SYSC_getprio
    ecall
    ret


.globl kill
.align 2
kill:
    addi a7, zero, SYSC_kill
    ecall
    ret


.globl pcount
.align 2
pcount:
    addi a7, zero, SYSC_pcount
    ecall
    ret


.globl pcreate
.align 2
pcreate:
    addi a7, zero, SYSC_pcreate
    ecall
    ret


.globl pdelete
.align 2
pdelete:
    addi a7, zero, SYSC_pdelete
    ecall
    ret


.globl preceive
.align 2
preceive:
    addi a7, zero, SYSC_preceive
    ecall
    ret


.globl preset
.align 2
preset:
    addi a7, zero, SYSC_preset
    ecall
    ret


.globl psend
.align 2
psend:
    addi a7, zero, SYSC_psend
    ecall
    ret


.globl start
.align 2
start:
    addi a7, zero, SYSC_start
    ecall
    ret


.globl wait_clock
.align 2
wait_clock:
    addi a7, zero, SYSC_wait_clock
    ecall
    ret


.globl waitpid
.align 2
waitpid:
    addi a7, zero, SYSC_waitpid
    ecall
    ret
