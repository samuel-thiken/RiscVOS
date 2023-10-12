#include "riscv.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "drivers/plic.h"
#include "keyboard.h"
#include "drivers/sifive_u/sifive_plic.c"
#include "drivers/console.h"
#include "cons_write.h"
#include "mem.h"

static STRUCT_BUFFER(char) keyboard_buffer;
static list_t *blocked_on_keyboard_list;

static bool echo = true;
static int cursor = 0;
static bool finished_line = false;



void init_keyboard(void) {
	char *buffer = (char *)mem_alloc(KEYBOARD_BUFFER_SIZE * sizeof(char));
	INIT_BUFFER(&keyboard_buffer, buffer, KEYBOARD_BUFFER_SIZE);
	blocked_on_keyboard_list = init_liste();
	plic_dev->init();
	csr_set(sie, MIP_SEIP); // pareil mais pour le supervisor
}


void handle_keyboard_interrupt() {
	int code = kgetchar();
	if (code < 0 || code > 155) {
		return;
	}
	char * deleted_char;
	if (code == BACKSPACE) {
		handle_echo(code);
		buf_unwrite(&keyboard_buffer, deleted_char);
		if (*deleted_char == EOL) {
			// dans le cas où on a un backspace sur une ligne vide, on veut pas revenir à la ligne précédente
			buf_write(&keyboard_buffer, *deleted_char); 
		}
		return;
	} 

	if (buf_full(&keyboard_buffer)) {
	  bool some_prio_higher = false;
	  cellule_t *cell = blocked_on_keyboard_list->head;
	  while (cell != NULL) {
    	process_t *proc = (process_t *)cell->val;
    	proc->state = ACTIVABLE;
    	some_prio_higher = some_prio_higher || proc->prio >= get_process_elu()->prio;
  	  cell = cell->next;
	  }
  wake_up_process(blocked_on_keyboard_list); // tous les process ACTIVABLE de la liste sont mis dans activable_process_list
	if (some_prio_higher) ordonnance();
	return;
	}

	handle_echo(code);
	buf_write(&keyboard_buffer, code);
	if (code == EOL) {
		finished_line = true;
		process_t *proc = extract_head(blocked_on_keyboard_list);
		if (proc != NULL) {
			proc->state = ACTIVABLE;
	    add_queue(activable_process_list, proc);
    	sort_process_list(activable_process_list, (void *)same_pid, proc, (void *)add_with_priority);
    	if (proc->prio >= get_process_elu()->prio) ordonnance();
		}
	}
}

int cons_read(char *string, unsigned long length) {
	char c;
	int nb_char_read = 0;
	if (length <= 0) {
		return 0;
	}

	while (!finished_line && nb_char_read < length) {
    process_t *running_proc = get_process_elu();
    running_proc->state = BLOCKED_ON_KEYBOARD;
    running_proc->msg_file = blocked_on_keyboard_list;
    add_queue(blocked_on_keyboard_list, running_proc);
    sort_process_list(blocked_on_keyboard_list, (void *)same_pid, running_proc, (void *)add_with_priority);
    ordonnance(); // le current proc est plus actif donc on schedule
	}	


	for (int i = 0; i < length; i++) {
		nb_char_read++;
		if (buf_empty(&keyboard_buffer)){
			break;
		}
		buf_read(&keyboard_buffer, c);
		if (c == EOL) {
			finished_line = false;
			break;
		}
		string[i] = c;
	}
	string[nb_char_read] = '\0';
	return nb_char_read;
}


void cons_echo(int on) {
	if (on == 1) {
		echo = true;
	} else if (on == 0) {
		echo = false;
	}
}

void handle_echo(char c) {
	char str[3];
	if (echo) {
		if (c == EOL) {
			str[0] = '\n'; // 10
			cursor = 0;
		} else if (c == BACKSPACE && cursor > 0){
			str[0] = '\b';
			str[1] = ' ';
			str[2] = '\b';
			cursor--;
		} else if (c == BS && cursor > 0) {
			str[0] = '\b';
			cursor--;
		} else if (c == LF) {
			str[0] = '\n';
			cursor = 0;
		} else if (c == HT) {
			str[0] = '\t';
			cursor += 4;
		} else if (c >= 31 && c <= 126) {
			str[0] = c;
			cursor++;
		} else if (c < 32) {
			str[0] = '^';
			str[1] = c + 64;
			cursor++;
		} else {
			// si aucun des cas on fait rien
			return;
		}
		cons_write(str, 1);
	}
}