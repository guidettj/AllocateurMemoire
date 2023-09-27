//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours : Conception des syst`emes d’exploitation et programmation concurrente
// Cursus : Universit´e Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#ifndef MEM_H
#define MEM_H
#include <stddef.h>

/* -----------------------------------------------*/
/* Interface utilisateur de votre allocateur */
/* -----------------------------------------------*/


typedef struct mem_free_block_s{
    size_t size;
    struct mem_free_block_s *next;
}mem_free_block_t;
typedef mem_free_block_t *(mem_fit_function_t)(mem_free_block_t *, size_t);

struct bb{
    size_t size;
};

struct tete{
    mem_free_block_t *next;
    mem_fit_function_t *fit;
};

void *mem_alloc(size_t);

void mem_free(void *);

size_t mem_get_size(void *);

/* It´erateur sur le contenu de l’allocateur */
void mem_show(void (*print)(void *, size_t, int free));

/* Enable logging for debugging */
void mem_set_logging(int enabled);

#endif //MEM_H
