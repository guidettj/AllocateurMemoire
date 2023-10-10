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
 
    void *a =  mem_alloc(10);
    void *b =  mem_realloc(a, 8);
    assert(b != NULL);

    mem_init();
    void *c =  mem_alloc(10);
    void *d =  mem_realloc(c, 18);
    assert(d != NULL);  

    mem_init();
    void *e =  mem_alloc(10);
    void *f =  mem_realloc(e, 0);
    assert(f == NULL);  


    printf("Test de mem_realloc réussi !\n");
}