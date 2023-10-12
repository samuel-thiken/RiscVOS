#ifndef LISTE_H
#define LISTE_H

typedef struct cellule_t
{
    void *val;
    struct cellule_t *next;
} cellule_t;

extern cellule_t *init_cell(void *value);

extern void free_cell(cellule_t *cell);

typedef struct list_t
{
    int size;
    cellule_t *head;
    cellule_t *queue;
} list_t;

extern list_t *init_liste();

extern void free_liste(list_t *liste);

extern void add_queue(list_t *liste, void *value);

extern void *extract_head(list_t *liste);

extern void sort_process_list(list_t *liste, int (*get_element)(), void *elem, int (*comparison_func)());

extern void *extract_pid(list_t *liste, int (*get_element)(), void *elem);

#endif
