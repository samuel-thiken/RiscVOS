/*
 * Projet PCSEA RISC-V
 *
 * Benoît Wallon <benoit.wallon@grenoble-inp.org> - 2019
 * Mathieu Barbe <mathieu@kolabnow.com> - 2019
 *
 * See license for license details.
 */

#include "stdio.h"
#include "syscall.h"

#include "stddef.h"
#include "assert.h"

#include "process.h"
#include "cons_write.h"
#include "timer.h"
#include "message_file.h"
#include "keyboard.h"

#include "userspace_apps.h"
 
void* syscall_table[NB_SYSCALLS];

int start_process_user(const char* name, unsigned long ssize, int prio, void* arg) {
	const struct uapps *user_app = find_app(name);
    if (user_app == NULL) {
        printf("Couldn't find app %s\n", name);
        return -1;
    } 
	return start(user_app->start, ssize, prio, name, arg);
}

void syscall_init() {
    // Initialisation de la table des syscall
    // On initialise toutes les entrées à NULL
    for (int i = 0; i < NB_SYSCALLS; i++) {
        syscall_table[i] = NULL;
    }
    syscall_table[SYSC_chprio] = chprio;
    syscall_table[SYSC_clock_settings] = clock_settings;
    //syscall_table[SYSC_cons_echo] = cons_echo;
    //syscall_table[SYSC_cons_read] = cons_read;
    syscall_table[SYSC_cons_write] = cons_write;
    syscall_table[SYSC_current_clock] = current_clock;
    syscall_table[SYSC_exit] = exit_process;
    syscall_table[SYSC_getpid] = getpid;
    syscall_table[SYSC_getprio] = getprio;
    syscall_table[SYSC_kill] = kill;
    syscall_table[SYSC_pcount] = pcount;
    syscall_table[SYSC_pcreate] = pcreate;
    syscall_table[SYSC_pdelete] = pdelete;
    syscall_table[SYSC_preceive] = preceive;
    syscall_table[SYSC_preset] = preset;
    syscall_table[SYSC_psend] = psend;
    syscall_table[SYSC_start] = start_process_user;
    syscall_table[SYSC_wait_clock] = wait_clock;
    syscall_table[SYSC_waitpid] = waitpid;
    //syscall_table[SYSC_sleep] = sleep;
    //syscall_table[SYSC_getname] = getname;
    //syscall_table[SYSC_getstatus] = getstatus;
    //syscall_table[SYSC_psize] = psize;
    //syscall_table[SYSC_waitpid_nohang] = waitpid_nohang;
    //syscall_table[SYSC_sleepms] = sleepms;
    //syscall_table[SYSC_cons_chbuffer] = cons_chbuffer;
    //syscall_table[SYSC_cons_wait] = cons_wait;
    //syscall_table[SYSC_reboot] = reboot;
    //syscall_table[SYSC_shm_create] = shm_create;
    //syscall_table[SYSC_shm_acquire] = shm_acquire;
    //syscall_table[SYSC_shm_release] = shm_release;
}
