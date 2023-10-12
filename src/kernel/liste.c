#include "liste.h"
#include "stdlib.h"
#include "mem.h"
#include "assert.h"

cellule_t *init_cell(void *value)
{
    cellule_t *cell = mem_alloc(sizeof(cellule_t));
    assert(cell != NULL);
    cell->val = value;
    cell->next = NULL;

    return cell;
}

void free_cell(cellule_t *cell)
{
    mem_free(cell, sizeof(cellule_t));
    cell = NULL;
}

list_t *init_liste()
{
    list_t *liste = mem_alloc(sizeof(list_t));
    assert(liste != NULL);
    liste->size = 0;
    liste->head = NULL;
    liste->queue = NULL;

    return liste;
}

void free_liste(list_t *liste)
{
    cellule_t *cell = liste->head;
    cellule_t *prec = cell;

    while (prec != NULL)
    {
        cell = prec->next;
        prec->next = NULL;
        free_cell(prec);
        prec = cell;
    }
    mem_free(liste, sizeof(list_t));
}

void add_queue(list_t *liste, void *value)
{
    cellule_t *cell = init_cell(value);
    liste->size += 1;
    cell->next = NULL;

    cellule_t **head = &(liste->head);
    cellule_t **queue = &(liste->queue);

    if (*head == NULL)
    {
        *head = cell;
        *queue = cell;
        return;
    }
    else
    {
        (*queue)->next = cell;
        *queue = cell;
    }
}

void *extract_head(list_t *liste)
{
    cellule_t **head = &(liste->head);
    cellule_t **queue = &(liste->queue);
    liste->size -= 1;

    cellule_t *cell = *head;
    if (cell == NULL)
        return NULL;
    else if (cell->next == NULL)
    {
        *head = NULL;
        *queue = NULL;
    }
    else
        *head = cell->next;

    cell->next = NULL;
    void *value = cell->val;
    mem_free(cell, sizeof(cellule_t));
    return value;
}

/**
 * Tri un processus d'une liste portant le pid pid selon la fonction comparison_func (fonction booléenne)
 */
void sort_process_list(list_t *liste, int (*get_element)(), void *elem, int (*comparison_func)())
{
    cellule_t **head = &(liste->head);
    cellule_t **queue = &(liste->queue);

    cellule_t *cell = *head;
    cellule_t *prec = *head;
    // On récupère l'élément que l'on souhaite changer de place
    while (cell != NULL && !(*get_element)(cell->val, elem))
    {
        prec = cell;
        cell = cell->next;
    }

    if (cell == NULL)
        return;

    prec->next = cell->next;

    if (cell == (*queue))
        (*queue) = prec;
    if (cell == (*head))
        (*head) = cell->next;

    cellule_t *next = *head;
    cellule_t *before_next = *head;
    // On récupère l'endroit où on doit déplacer l'élément
    while (next != NULL && !(*comparison_func)(cell->val, next->val))
    {
        before_next = next;
        next = next->next;
    }

    // L'élément doit passer en queue de la liste
    if (next == NULL)
    {
        if (cell->next != NULL)
            prec->next = cell->next;
        (*queue)->next = cell;
        cell->next = NULL;
        *queue = cell;
        // Si la liste ne contient qu'un élément
        if (*head == NULL)
            *head = cell;
    }
    // L'élément doit passer en tête de la liste
    else if (next == (*head))
    {
        prec->next = cell->next;
        cell->next = *head;
        *head = cell;
    }
    // L'élément ne doit être placé ni en tête, ni en queue de liste
    else
    {
        prec->next = cell->next;
        cell->next = next;
        before_next->next = cell;
    }
}

void *extract_pid(list_t *liste, int (*get_element)(), void *elem)
{
    cellule_t **head = &(liste->head);
    cellule_t **queue = &(liste->queue);
    liste->size -= 1;

    cellule_t *cell = *head;
    if (cell == NULL)
        return NULL;
    cellule_t *prec = cell;
    if ((*get_element)(cell->val, elem))
        *head = cell->next;
    else
    {
        while (cell != NULL && !(*get_element)(cell->val, elem))
        {
            prec = cell;
            cell = cell->next;
        }
        if (cell == NULL)
            return NULL;
        prec->next = cell->next;
    }
    cell->next = NULL;
    if (*head == NULL)
        *queue = NULL;
    else if ((*head)->next == NULL)
        *queue = *head;

    void *value = cell->val;
    mem_free(cell, sizeof(cellule_t));
    return value;
}
