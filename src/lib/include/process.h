#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "context.h"
#include "liste.h"
#include "mem.h"

#define MAX_SIZE_NAME_PROCESS 50
#define NBPROC 30
#define MAXPRIO 256

typedef enum process_state
{
    RUNNING,
    ACTIVABLE,
    SLEEPING,
    WAITING,
    ZOMBI,
    FINISHED,
    KILLED,
    BLOCKED_ON_MSG,
    BLOCKED_ON_KEYBOARD
} process_state;

typedef struct process_t
{
    int pid;
    char name[MAX_SIZE_NAME_PROCESS];
    process_state state;
    context_t context;
    unsigned long ssize;
    unsigned long *stack;
    unsigned long reveil;
    int prio;
    int msg; // contient la val du msg
    int error_msg;
    list_t* msg_file; // pointeur vers la liste de processus en attente de message
    int retval;
    int waitpid;
    list_t *fils;
    struct process_t *pere;
    memory_tables_t mem_tables;
} process_t;

// Table des processus
extern process_t *process_table[NBPROC];

// Mise en place des processus
extern void set_up_process();

// Affiche l'ensemble des processus
extern void ps();

// Processus en cous d'exécution
extern process_t *running_process;

extern list_t *activable_process_list;


// Initialisation d'un processus
extern int start(int (*pt_func)(void*), unsigned long ssize, int prio, const char *name, void *arg);
extern int start_in_kernel(int (*pt_func)(void *), unsigned long ssize, int prio, const char *name, void *arg);

extern void wake_up_process(list_t *list);

// Implante la politique d’ordonnancement en choisissant le prochain processus à activer
extern void ordonnance(void);

// Renvoie le pid du processus ELU
extern int getpid(void);

// Renvoie le nom du processus ELU
extern char *process_name(void);

// Renvoie le processus actif
extern process_t *get_process_elu();

extern void set_process_elu(process_t *proc);

// Renvoie le temps d'attente d'un processus
extern unsigned long nbr_secondes(void);

// endors un processus
extern void wait_clock(unsigned long clock);

// Attente d'un processus fils
extern int waitpid(int pid, int *retvalp);

// termine le processus actif
extern void exit_process(int retval);

// met fin à un processus
extern int kill(int pid);

// Récupère la priorité d'un processus
extern int getprio(int pid);

// Modifie la priorité d'un processus
extern int chprio(int pid, int newprio);


extern int same_pid(process_t *proc1, process_t *proc2);

extern int add_with_priority(process_t *proc1, process_t *proc2);

#endif
