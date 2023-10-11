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
 
    void *a = mem_alloc(10);
    struct bb *b = mem_realloc(a, 8) - sizeof(size_t);

    assert(b != NULL);
    assert(b->size == 8);

    mem_init();
    void *c = mem_alloc(10);
    struct bb *d = mem_realloc(c, 18) - sizeof(struct bb);
    assert(d != NULL);
    assert(d->size == 18);

    mem_init();
    void *e = mem_alloc(10);
    struct bb *bb = (struct bb *)((char *)e - sizeof(size_t));
    mem_realloc(e, 0);
    assert(bb->size != 0);

 
    printf("Test de mem_realloc réussi !\n");
}