//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------
struct tete * tete;
/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/

/** 
 * Put b2 next to b1 in the memory
*/
void place_next_to(struct fb * b1, struct fb * b2){
	b2 = b1 + sizeof(struct fb) + b1->size;
}

/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/
void mem_init() {
	// struct * tete init;
	struct fb * bfict;
	struct fb * mem1;
	
	tete = mem_space_get_addr();
	bfict = (struct fb *) (tete + sizeof(tete));
	tete->next = bfict;
	tete->fit = mem_first_fit;

	place_next_to(bfict, mem1);
	// mem1 = bfict + sizeof(bfict);
	bfict->size = 0;
	bfict->occupied = 0;
	bfict->next = mem1;

	mem1->size = mem_space_get_size() - sizeof(struct fb) * 2 - sizeof(struct tete);
	mem1->occupied = 0;
	mem1->next = NULL;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
struct fb *trouve_queue(struct fb * b){
	if (b->next == NULL)
		return b;
	return trouve_queue(b->next);
} 

void *mem_alloc(size_t size) {
	struct fb * queue;
	queue->next = null;
	queue->size = size;

	struct fb * body = trouve_queue();
	body->next = queue;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
    return 0;
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
}

//----------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
// _mem_show fonction bis permettant la recursivite sur les blocks
//----------------------------------------------------------------
void _mem_show(void (*print)(void *, size_t, int free), struct fb * block) {
	if (block == NULL)
		return;

	print(block, block->size, block->occupied);
	return _mem_show(print, block->next);
}

void mem_show(void (*print)(void *, size_t, int free)) {
    struct fb * block;
	block = mem_space_get_addr() + sizeof(tete) + sizeof(struct fb);
	_mem_show(print, block);
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
	//TODO: implement
		if (*mff == mem_worst_fit) {
		tete->fit = mem_worst_fit;
	}
	else if (*mff ==  mem_best_fit) {
		tete->fit =  mem_best_fit;
	}
	else  {
		tete->fit =  mem_first_fit;
	}
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	while (first_free_block!=NULL){
		if(first_free_block->size >= wanted_size){
			return first_free_block;
		}
		first_free_block = first_free_block->next;
	}
	return NULL;
}
//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	size_t diffTaille = first_free_block->size - wanted_size;
	mem_free_block_t* adresse = first_free_block;
	while(first_free_block != NULL){
		if(diffTaille < first_free_block->size - wanted_size){
			diffTaille = first_free_block->size - wanted_size;
			adresse = first_free_block;
		}
	}
	return adresse;
}

//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
    //TODO: implement
	assert(! "NOT IMPLEMENTED !");
	return NULL;
}
