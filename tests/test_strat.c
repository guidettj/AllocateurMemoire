//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------
#include "mem_space.h"
#include "mem.h"
#include "mem_os.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TAILLE 50

int a_size, b_size, c_size, d_size;

void set_values(int va, int vb, int vc, int vd){
    a_size = va;
    b_size = vb;
    c_size = vc;
    d_size = vd;
}

int main() {
    mem_init();
    void * p[TAILLE];

    for (int i = 0; i < TAILLE; i++) {
        p[i]= mem_alloc(10);
        assert(p[i] != NULL);
    }

    // On regarde si les blocks alloues sont bien colles l'un a l'autre
    for (int i = 1; i < TAILLE; i++) {
        assert(p[i] == (char *)p[i-1]+ sizeof(size_t) + 10);
    }

    printf("mem_first_fit OK !\n");

    // reinitialisation de memoire
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    set_values(20, mem_space_get_size()/2, 200, 10);

    void * a = mem_alloc(a_size);
    void * b = mem_alloc(b_size);
    void * c = mem_alloc(c_size);
    
    mem_free(b);
    // il y a un gros bloc dispo apres a et un plus petit bloc dispo apres c
    void * d = mem_alloc(d_size);
    // best_fit doit choisir le bloc plus petit
    assert((char *)d == (char *)c + c_size + sizeof(size_t));
    
    mem_init();
    mem_set_fit_handler(mem_best_fit);
    set_values(18, 19, 20, 17);
    a = mem_alloc(a_size);
    b = mem_alloc(b_size);
    c = mem_alloc(c_size);
    mem_alloc(10);
    mem_free(a);
    mem_free(c);
    d = mem_alloc(d_size);
    
    if(a_size > c_size){
        assert((char *)d == (char *)b + b_size + sizeof(size_t));
    }
    else{
        assert(d == a); //debut
    }
    //assert((char *)d == (char *)b + b_size + sizeof(size_t));
    printf("mem_best_fit OK !\n");

    mem_init();
    mem_set_fit_handler(mem_worst_fit);

    a = mem_alloc(a_size);
    b = mem_alloc(b_size);
    c = mem_alloc(c_size);

    mem_free(b);
    // il y a un gros bloc dispo apres a et un plus petit bloc dispo apres c
    d = mem_alloc(d_size);
    // worst_fit doit choisir le bloc le plus grand

    if(b_size+16> mem_space_get_size()- a_size - b_size - c_size +16*3){
        assert((char *)d==(char *)a + sizeof(size_t) + a_size);
    }
    else{
        assert((char *)d==(char *)c + sizeof(size_t) + c_size);
    }


    printf("mem_worst_fit OK !\n");
    printf("Tous les tests pour les stratégies ont réussis !\n");

    return 0;
}
