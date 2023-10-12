/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 *
 * See license for license details.
 */

#include "stdio.h"
#include "assert.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "traps/trap.h"
#include "drivers/splash.h"
#include "tests/tests.h"
#include "process.h"
#include "syscall.h"

int kernel_start()
{
    splash_screen();
    splash_vga_screen();
    init_frames();
    create_kernel_memory();
    puts("Hello world!");

    uint8_t *ptr = (void *) 0x100000;
    *ptr = 42;
    printf("%u\n", *ptr);

    // Mise en place des processus
    syscall_init();
    set_up_process();

    //syscall_init(); à faire si mem virtuelle activée
    exit(0);
}
