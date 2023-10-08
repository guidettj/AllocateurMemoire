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

int a_size, b_size, c_size, d_size;

void set_values(int va, int vb, int vc, int vd){
    a_size = va;
    b_size = vb;
    c_size = vc;
    d_size = vd;
}

int main() {
    mem_init();

    set_values(20,25,26,40);
    void * a = mem_alloc(a_size);
    void * b = mem_alloc(b_size);
    void * c = mem_alloc(c_size);
    void * d = mem_alloc(d_size);

    mem_free(b);
    printf("%zu/n",calc_bb(a));
    printf("%zu/n",(size_t)a_size);
    assert(calc_bb(a)==(size_t)a_size);
    assert(a_size == calc_fb(a)-16);
    
    printf("calc_bb et calc_fb OK !\n");

    assert(d == calc_add_bb(c));
    assert(c == calc_add_fb(b));

    printf("calc_add_bb et calc_add_fb OK !\n");

    mem_free(d);
    assert(b==fb_before_add(a,d));
    printf("fb_before_add OK !\n");

    assert(a == find_bb(a));
    assert(NULL == find_bb(b));

    printf("find_bb OK !\n");

    printf("Tous les tests pour les calculs ont réussis !\n");

    return 0;
}
