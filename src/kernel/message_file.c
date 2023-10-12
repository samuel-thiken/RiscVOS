#include "message_file.h"
#include "stdbool.h"
#include "stdio.h"
#include "buf.h"
#include "mem.h"

struct msgqueue {
  bool busy;
  msg_t message_list;
};

static struct msgqueue msgqueues[NBQUEUE];


int pcount(int fid, int *count) {
  // compte le nb de process en attente
  if (!valid_fid(fid)) {
    return -1;
  }
  int val = msgqueues[fid].message_list.process_waiting->size;
  // if val > 0 place -val dans count
  if (val > 0 && buf_empty(&msgqueues[fid].message_list.msgs)) {
    *count = -val;
  } else {
    *count = msgqueues[fid].message_list.msgs.count + val;
  }   
  return 0;
}

/**
 * @brief fini
 * 
 * @param count 
 * @return int le numéro de la file allouée
 */
int pcreate(int count) {
  int fid = getnewfid();
  if (count <= 0 || fid < 0) {
    return -1;
  }

  int *buffer = (int *)mem_alloc(count * sizeof(int));
  msgqueues[fid].busy = true; 
  INIT_BUFFER(&msgqueues[fid].message_list.msgs, buffer, count);
  msgqueues[fid].message_list.process_waiting = init_liste();
  return fid;
}

// presque fini
int pdelete(int fid) {
  if (!valid_fid(fid)) {
    return -1;
  }
  bool some_prio_higher = false;
  // fait passer dans l'état activable tous les processus, s'il en existe, qui se trouvaient bloqués sur la file
  cellule_t *cell = msgqueues[fid].message_list.process_waiting->head;
  while (cell != NULL)
 {
    process_t *proc = (process_t *)cell->val;
    proc->state = ACTIVABLE;
    proc->error_msg = -1;
    some_prio_higher = some_prio_higher || proc->prio >= get_process_elu()->prio;
    cell = cell->next;
  }
  wake_up_process(msgqueues[fid].message_list.process_waiting); // tous les process ACTIVABLE de la liste sont mis dans activable_process_list
  msgqueues[fid].busy = false; // on libère la file
  buf_free_data(&msgqueues[fid].message_list.msgs); // on free le buffer de message
  if (some_prio_higher) ordonnance();
  return 0;
}

int preceive(int fid, int *message) {
  if (!valid_fid(fid)) {
    return -1;
  }
  int msg;
  int error = 0;
  if (buf_empty(&msgqueues[fid].message_list.msgs)) {
    // pas de message, on passe en attente d'un psend
    process_t *running_proc = get_process_elu();
    running_proc->state = BLOCKED_ON_MSG;
    running_proc->msg_file = msgqueues[fid].message_list.process_waiting;
    add_queue(msgqueues[fid].message_list.process_waiting, running_proc);
    sort_process_list(msgqueues[fid].message_list.process_waiting, (void *)same_pid, running_proc, (void *)add_with_priority);
    ordonnance(); // le current proc est plus actif donc on schedule
    msg = get_process_elu()->msg; // après l'ordonnance, on récupère le message
    error = get_process_elu()->error_msg; // on récupère l'erreur
  } else if (buf_full(&msgqueues[fid].message_list.msgs) && msgqueues[fid].message_list.process_waiting->size > 0) {
    // file pleine 
    process_t *proc = extract_head(msgqueues[fid].message_list.process_waiting);
    buf_rotate(&msgqueues[fid].message_list.msgs, msg, proc->msg);
    proc->state = ACTIVABLE;
    add_queue(activable_process_list, proc);
    sort_process_list(activable_process_list, (void *)same_pid, proc, (void *)add_with_priority);
    if (proc->prio >= get_process_elu()->prio) ordonnance();
  } else {
    buf_read(&msgqueues[fid].message_list.msgs, msg);
  }
  
  if (error >= 0) {
    *message = msg;
  }
  return error;
}

int preset(int fid) {
  if (!valid_fid(fid)) {
    return -1;
  }
  bool some_prio_higher = false;
  // boucle qui met ACTIVABLE
  cellule_t *cell = msgqueues[fid].message_list.process_waiting->head;
  while (cell != NULL)
  {
    process_t *proc = (process_t *)cell->val;
    proc->state = ACTIVABLE;
    proc->error_msg = -1;
    proc->msg = 0; // reset de la valeur du message
    some_prio_higher = some_prio_higher || proc->prio > get_process_elu()->prio;
    cell = cell->next;
  }
  wake_up_process(msgqueues[fid].message_list.process_waiting); // on réveille tous les processus en attente de message
  buf_reset(&msgqueues[fid].message_list.msgs); // on vide la liste de message
  if (some_prio_higher) ordonnance();
  return 0;
}

// à tester
int psend(int fid, int message) {
  if (!valid_fid(fid)) {
    return -1;
  }
  int error = 0;
  if (buf_empty(&msgqueues[fid].message_list.msgs) && msgqueues[fid].message_list.process_waiting->size > 0) {
    // si buf vide && 1 ou plus processus en attente
    process_t *proc = extract_head(msgqueues[fid].message_list.process_waiting);
    proc->state = ACTIVABLE;
    proc->msg = message;
    proc->error_msg = 0;
    add_queue(activable_process_list, proc);
    sort_process_list(activable_process_list, (void *)same_pid, proc, (void *)add_with_priority);
    if (proc->prio >= get_process_elu()->prio) ordonnance();
  } else if (buf_full(&msgqueues[fid].message_list.msgs)) {
    process_t *running_proc = get_process_elu();
    running_proc->state = BLOCKED_ON_MSG;
    running_proc->msg_file = msgqueues[fid].message_list.process_waiting;
    running_proc->msg = message;
    running_proc->error_msg = 0;
    add_queue(msgqueues[fid].message_list.process_waiting, running_proc);
    sort_process_list(msgqueues[fid].message_list.process_waiting, (void *)same_pid, running_proc, (void *)add_with_priority);
    ordonnance(); // le current proc est plus actif donc on schedule
    error = get_process_elu()->error_msg; // on rerécupère le process elu et puis son msg
  } else if (msgqueues[fid].message_list.process_waiting->size == 0){
    // buf pas plein et pas de processus en attente
    buf_write(&msgqueues[fid].message_list.msgs, message);
  }
  return error;
}

/**
 * @brief parcours le tableau des files de messages et retourne le premier indice libre
 * 
 * @return int le fid
 */
int getnewfid() {
  int i;
  for (i = 0; i < NBQUEUE; i++) {
    if (!msgqueues[i].busy) {
      return i;
    }
  }
  return -1;

}

int valid_fid(int fid) {
  return fid >= 0 && fid < NBQUEUE && msgqueues[fid].busy;
}
