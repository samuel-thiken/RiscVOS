/*
 * Macros génériques pour manipuler des buffers circulaires
 * Inspiré de queue.h
 *
 * Ces macros manipulent des pointeurs sur des structures définies
 * à l'aide de STRUCT_BUFFER(type). Le champ count de cette structure
 * est mis à jour à chaque lecture/écriture. Il est prévu pour pouvoir
 * être modifié par l'utilisateur, par exemple pour indiquer des
 * processus bloqué en écriture (count > size) ou en lecture (count < 0).
 * Les autres champs ne doivent pas être modifiés directement.
 */

#ifndef ___BUF_H___
#define ___BUF_H___
#include <assert.h>
/*
 * Construit le type d'un buffer en fonction du type des données stockées
 */
#define STRUCT_BUFFER(data_type)                                        \
        struct {                                                        \
                int size;            /* Taille du buffer */             \
                int count;           /* Données dans le buffer */       \
                data_type *start;    /* Début du buffer */              \
                data_type *end;      /* Fin du buffer */                \
                data_type *writepos; /* Tête d'écriture */              \
                data_type *readpos;  /* Tête de lecture */              \
        }

/*
 * Initialise un buffer avec un tableau de données prédéfini (alloué
 * sur le tas ou défini statiquement).
 * buf          : un pointeur vers un buffer de type T
 * data_ptr     : un pointeur (type T*) vers le début de la zone allouée
 *                aux données du buffer
 * size         : taille de la zone allouée aux données
 */
#define INIT_BUFFER(buf, data_ptr, data_size)                           \
        do {                                                            \
                (buf)->size = (data_size);                              \
                (buf)->count = 0;                                       \
                (buf)->start = (data_ptr);                              \
                (buf)->end = (data_ptr) + (data_size) - 1;              \
                (buf)->writepos = (data_ptr);                           \
                (buf)->readpos = (data_ptr);                            \
        } while (0)


/*
 * Vérifie si le buffer est vide ou plein
 */
#define buf_empty(buf)          ((buf)->count <= 0)
#define buf_full(buf)           ((buf)->count >= (buf)->size)



/* Macros à usage interne. Ne pas utiliser directement */
#define ___buf_incr(buf, pos)                                           \
        ((buf)->pos = ((buf)->pos == (buf)->end)                        \
                                ? (buf)->start                          \
                                : (buf)->pos+1                          \
        )
#define ___buf_decr(buf, pos)                                           \
        ((buf)->pos = ((buf)->pos == (buf)->start)                      \
                                ? (buf)->end                            \
                                : (buf)->pos-1                          \
        )


/*
 * Écrit une donnée dans le buffer et incrémente son compteur
 * buf          : pointeur vers un buffer
 * data         : donnée à écrire
 */
#define buf_write(buf, data)                                            \
        do {                                                            \
                assert((buf)->count >= 0                                \
                        && (buf)->count < (buf)->size);                 \
                *((buf)->writepos) = (data);                            \
                ___buf_incr(buf, writepos);                             \
                ++((buf)->count);                                       \
        } while (0)

/*
 * Annule la dernière écriture, si le buffer est non vide
 * buf          : pointeur vers un buffer
 * ptr          : pointeur affecté par cette macro
 *                si buf est vide, ptr est mis à NULL
 *                sinon il pointe vers la dernière donnée écrite
 */
#define buf_unwrite(buf, ptr)                                           \
        if (buf_empty(buf)) {                                           \
                (ptr) = NULL;                                           \
        } else {                                                        \
                ___buf_decr(buf, writepos);                             \
                (ptr) = ((buf)->writepos);                              \
                --((buf)->count);                                       \
        }

/*
 * Lit une donnée dans le buffer et décrémente son compteur.
 * buf          : pointeur vers un buffer
 * data         : variable où placer la donnée à lire
 */
#define buf_read(buf, data)                                             \
        do {                                                            \
                assert((buf)->count > 0);                               \
                assert((buf)->count <= (buf)->size);                    \
                (data) = *((buf)->readpos);                             \
                ___buf_incr(buf, readpos);                              \
                --((buf)->count);                                       \
        } while (0)

/*
 * Quand le buffer est plein, lit la donnée au début du buffer
 * et écrit une donnée à la fin.
 * buf          : pointeur vers un buffer
 * read         : variable où placer la donnée sortie du buffer
 * write        : donnée à insérer dans le buffer
 */
#define buf_rotate(buf, read, write)                                    \
        do {                                                            \
                assert((buf)->readpos == (buf)->writepos);              \
                (read) = *((buf)->readpos);                             \
                *((buf)->writepos) = (write);                           \
                ___buf_incr(buf, readpos);                              \
                ___buf_incr(buf, writepos);                             \
        } while (0)



/*
 * Réinitialise le buffer
 */
#define buf_reset(buf)                                                  \
        do {                                                            \
                (buf)->count = 0;                                       \
                (buf)->writepos = (buf)->start;                         \
                (buf)->readpos = (buf)->start;                          \
        } while (0)

/*
 * Libère la zone de données contenue dans buffer
 */
#define buf_free_data(buf)                                              \
        ( mem_free((buf)->start, (buf)->size * sizeof(*(buf)->start)) )
#endif

