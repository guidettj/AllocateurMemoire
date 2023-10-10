//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------
#include "../headers/mem_space.h"
#include "../headers/mem.h"
#include "../headers/mem_os.h"
#include "../src/mem.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    mem_init();
 
    struct bb *a = mem_alloc(10);
    struct bb *b = mem_realloc(a, 8) - sizeof(struct bb);
    // long int * bb = (long int *) b;
    assert(b != NULL);
    assert(b->size == 8);

    mem_init();
    struct bb *c = mem_alloc(10);
    struct bb *d = mem_realloc(c, 18) - sizeof(struct bb);
    assert(d != NULL);
    assert(d->size == 18);

    mem_init();
    struct bb *e = mem_alloc(10);
    struct bb *f = mem_realloc(e, 0) - sizeof(struct bb);
    printf("%ld\n",b->size);
    assert(f->size == 8);  

    mem_init();
    a = mem_alloc(10);

    printf("Test de mem_realloc réussi !\n");
}