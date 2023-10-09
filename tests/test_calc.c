//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------
#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"
#include "../src/mem.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


int main() {
    mem_init();
    size_t a_size=20, b_size=25, c_size=26, d_size=40;
    void * a = mem_alloc(a_size);
    void * b = mem_alloc(b_size);
    void * c = mem_alloc(c_size);
    void * d = mem_alloc(d_size);
    mem_alloc(45);
    void * f = mem_alloc(10);

    mem_free(b);

    assert(calc_bb(a)==(size_t)a_size+8);
    assert(a_size == calc_fb(a));
    
    printf("calc_bb et calc_fb OK !\n");

    assert(d == calc_add_bb(c));
    assert(c == calc_add_fb(b));

    printf("calc_add_bb et calc_add_fb OK !\n");

    mem_free(d);
   // printf("Valeur de size_t : %zu\n",fb_before_add(a,d));
    mem_free(f);

    assert(f==fb_before_add(adr_fict(),f+1));
    printf("fb_before_add OK !\n");

    assert(a == find_bb(a));
    assert(NULL == find_bb(b));

    printf("find_bb OK !\n");
  
    assert(b == trouve_parent(d));
    printf("trouve_parents OK !\n");

    printf("Tous les tests ont réussis !\n");

    return 0;
}
