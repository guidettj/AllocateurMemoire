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
//struct tete * tœete; Pas le droit
/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/

size_t calc_fb(mem_free_block_t* add){
	return sizeof(mem_free_block_t) + add->size;
}

size_t calc_bb(struct bb * add){
	return sizeof(struct bb) + add->size;
}

struct bb *calc_add_bb(struct bb * add){
	return (struct bb *) ((char *)add + sizeof(struct bb) + add->size);
}

mem_free_block_t *calc_add_fb(mem_free_block_t * add){
	return (mem_free_block_t *) ((char *)add + sizeof(mem_free_block_t) + add->size);
}

mem_free_block_t *_trouve_parent(mem_free_block_t * b, mem_free_block_t * b_cherche){
	if(b_cherche == NULL || b == NULL)
		return NULL;

	if(b->next == b_cherche)
		return b;

	return _trouve_parent(b->next, b_cherche);
}

mem_free_block_t *trouve_parent(mem_free_block_t * fb){
	struct tete * tete = (struct tete *) mem_space_get_addr();
	return _trouve_parent(tete->next, fb);
}
void fusion_(){
	struct tete* tete = (struct tête*)mem_space_get_addr();
	mem_free_block_t * fb1 = tete->next;
	while(fb1 != NULL){
		if(calc_add_fb(fb1) == fb1->next ){
			mem_free_block_t * fb2 = fb1->next;
			if (fb2->next != NULL)
				fb1->next = fb2->next;
			else
				fb1 = NULL;
		}		
	}
}

void fusion(mem_free_block_t *fb){
	mem_free_block_t *  parent =  trouve_parent(fb);
	if( trouve_parent(fb) == NULL)
		return NULL;
	if( calc_add_fb (parent) == fb){
		parent->next = fb->next;
		parent->size = parent->size + calc_fb(fb);
	}
	mem_free_block_t * child = calcul_add_fb(fb);
	if(fb->next == child){
		fb->next = child->next;
		fb->size = fb->size + calc_fb(child);
	}
}


/**
 * Initialize the memory allocator.
 * If already init it will re-init.
**/
void mem_init() {
	struct tete * tete;
	mem_free_block_t * bfict;
	mem_free_block_t * mem1;
	
	tete = (struct tete *) mem_space_get_addr();
	bfict = (mem_free_block_t *) (tete + 1);
	tete->next = bfict;
	tete->fit = mem_first_fit;

	bfict->size = 0;
	mem1 = calc_add_fb(bfict);
	bfict->next = mem1;

	mem1->size = mem_space_get_size() - sizeof(mem_free_block_t) * 2 - sizeof(struct tete);
	// printf("%p", mem_space_get_addr())
	printf("sizeof(mem_free_block_t) : %ld\n", sizeof(mem_free_block_t));
	printf("sizeof(struct tete) : %ld\n\n", sizeof(struct tete));

	printf("Memoire totale : %ld\n", mem_space_get_size());
	printf("Memoire debut  :%ld\n\n", mem_space_get_size() - sizeof(mem_free_block_t) * 2 - sizeof(struct tete));

	printf("Adresse mem_d : %p\n", mem_space_get_addr());

	printf("Adresse Tete  : %p\n", tete);
	printf("Adresse bfict : %p\n", bfict);
	printf("Adresse mem1  : %p\n\n", mem1);

	printf("Adresse parent mem1 : %p\n", trouve_parent(mem1));

	// printf("Adresse mem1  : %p\n\n", mem1);

	

	mem1->next = NULL;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/

// renvoie l'adresse du bloc libre juste avant le bloc occupe
// NULL si un tel fb n'est pas trouve
mem_free_block_t *fb_before_add(mem_free_block_t *fb, void * add){
    if(fb->next == NULL)
        return NULL;

    if((void *)(fb->next) > add)
        return fb;

    return fb_before_add(fb->next, add);
}

// nom de fonc doit etre tirer d'un tas de chose
// renvoie l'adresse 

struct bb *pick_bb(struct bb * start, void * add, void * next_fb){
    if(start == NULL || add == NULL || (void *)start >= next_fb)
        return NULL;

    if((void *)start == add)
        return start;
    
    return pick_bb(calc_add_bb(start), add, next_fb);
}

void *mem_alloc(size_t size) {
	struct tete * tete = (struct tete *) mem_space_get_addr();

	mem_free_block_t * libre = (tete->fit)(tete->next, size);
	// pas d'emplacement libre de taille `size` 
	if (libre == NULL)
		return NULL;
		
	mem_free_block_t * p_libre = trouve_parent(libre);
	if (p_libre == NULL)
		return NULL;

	struct bb * busy = (struct bb *) libre;
	busy->size = size;
	
	// Aura t on de la place pour mettre un mem_free_block_t 
	// si on attribue `size` memoire a busy ?
	int offset = libre->size - size;
	if(offset <= 0){
		// si <0 alors pas de place pour un struct fb
		// on va attribuer la taille a busy
		busy->size -= offset;
		p_libre->next = libre->next;
		
		libre->size = 0;
		libre = NULL;
	}
	else{
		libre->size -= size;
		libre = (mem_free_block_t * ) ((char *)busy + size + sizeof(struct bb));
		p_libre->next = libre;
	}

	return busy;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone)
{
	//chercher dans fb et si ca yest pas caster en bb et on pourra recup la taille
	struct bb* adrStruct = find_bb(zone);
	if(adrStruct == NULL){
		return NULL;
	}
	else{
	return adrStruct->size()
	}
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

// bb *pick_bb(bb * start, void * add, void * next_fb){
//     if(this == NULL || add == NULL || (void *)start >= next_fb)
//         return NULL;

//     if((void *)start == add)
//         return start;
    
//     return (start + sizeof(struct bb) + start->size, add, next_fb);
// }


// bb *find_bb(void * add){
//     struct tete * tete = (struct tete *) mem_space_get_addr();
//     mem_free_block_t fb = fb_before_add(tete->next, add);
//     return pick_bb((struct bb *) (fb + fb->size + sizeof(mem_free_block_t)), add, (void *) (fb->next);)
// }

void _mem_show(void (*print)(void *, size_t, int free), mem_free_block_t * block) {
	if (block == NULL)
		return;

	print(block, block->size, 1);
	return _mem_show(print, block->next);
}

void mem_show(void (*print)(void *, size_t, int free)) {
		// struct tete * tete = (struct tete *) mem_space_get_addr();
		// block = mem_space_get_addr() + sizeof(struct tete) + sizeof(mem_free_block_t);
		// _mem_show(print, block);
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
	//TODO: implement
	struct tete * tete = (struct tete *) mem_space_get_addr();
	tete->fit = mff;
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
	mem_free_block_t * adresse = NULL;
	size_t diffTaille = mem_space_get_size();

	while(first_free_block != NULL){
		if(first_free_block->size > wanted_size){
			if(diffTaille > first_free_block->size - wanted_size){
				diffTaille = first_free_block->size - wanted_size;
				adresse = first_free_block;
			}
		}
		first_free_block = first_free_block->next;
	}
	return adresse;
}


//-------------------------------------------------------------
mem_free_block_t *mem_worst_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
	mem_free_block_t * adresse = NULL;
	size_t diffTaille = 0;

	while(first_free_block != NULL){
		if(first_free_block->size > wanted_size){
			if(diffTaille < first_free_block->size - wanted_size){
				diffTaille = first_free_block->size - wanted_size;
				adresse = first_free_block;
			}
		}
		first_free_block = first_free_block->next;
	}
	return adresse;
}
