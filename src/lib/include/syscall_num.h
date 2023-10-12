#ifndef ___SYSCALL_NUM_H___
#define ___SYSCALL_NUM_H___

#define SYSC_chprio             0
#define SYSC_clock_settings     1
#define SYSC_cons_echo          2
#define SYSC_cons_read          3
#define SYSC_cons_write         4
#define SYSC_current_clock      5
#define SYSC_exit               6
#define SYSC_getpid             7
#define SYSC_getprio            8
#define SYSC_kill               9
#define SYSC_pcount             10
#define SYSC_pcreate            11
#define SYSC_pdelete            12
#define SYSC_preceive           13
#define SYSC_preset             14
#define SYSC_psend              15
#define SYSC_start              16
#define SYSC_wait_clock         17
#define SYSC_waitpid            18

#define SYSC_sleep              19
#define SYSC_getname            20
#define SYSC_getstatus          21
#define SYSC_psize              22
#define SYSC_waitpid_nohang     23
#define SYSC_sleepms            24
#define SYSC_cons_chbuffer      25
#define SYSC_cons_wait          26
#define SYSC_reboot             27

#define SYSC_shm_create         28
#define SYSC_shm_acquire        29
#define SYSC_shm_release        30

#define NB_SYSCALLS             31

#endif
