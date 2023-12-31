//------------------------------------------------------------------------------
// Projet : TP CSE (malloc)
// Cours  : Conception des systèmes d'exploitation et programmation concurrente
// Cursus : Université Grenoble Alpes - UFRIM²AG - Master 1 - Informatique
//------------------------------------------------------------------------------

#include "mem.h"
#include "mem_space.h"
#include "mem_os.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>


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


/* ---------------------------------------------
 * Fonctions adr_fict
 * Renvoie l'adresse du bloc fictif
 */
mem_free_block_t * adr_fict(){
	struct tete * tete = (struct tete *) mem_space_get_addr();
	return tete->next;
}
// ---------------------------------------------


/* ---------------------------------------------
 * Fonctions out_of_memory
 * Dis si l'adresse ne depasse pas la memoire
 */
int8_t out_of_memory(void * add){
// add = 2
// 1 <= 2 && 2 < 3 correct statement
// 1 > 2 || 2 >= 3 !correct
	return  mem_space_get_addr() > add || add >= mem_space_get_last_addr() ;
}


/* ---------------------------------------------
 * Fonctions de calcul
 * Retournent la taille memoire occuppee 
 * taille de structure + memoire reservee
 */
size_t calc_fb(mem_free_block_t* add){
	return add->size;
}

size_t calc_bb(struct bb * add){
	return sizeof(struct bb) + add->size;
}
// ---------------------------------------------


/* ---------------------------------------------
 * Fonctions de calcul d'adresse
 * Retournent l'adresse memoire juste apres la structure
 * adresse + taille de structure + memoire reservee
 */
struct bb *calc_add_bb(struct bb * add){
	return (struct bb *) ((char *)add + sizeof(struct bb) + add->size);
}

mem_free_block_t *calc_add_fb(mem_free_block_t * add){
	return (mem_free_block_t *) ((char *)add + add->size);
}
// ---------------------------------------------



/* ---------------------------------------------
 * Fonction *trouve_parent()
 * Prend en parametre un *fb dont on cherche le parent
 * Retourne le parent de fb 
 * Si fb est orphelin renvoie NULL
 */
mem_free_block_t *_trouve_parent(mem_free_block_t * b, mem_free_block_t * b_cherche){
	if(b_cherche == NULL || b == NULL || out_of_memory(b_cherche)) 
		return NULL;

	if(b->next == b_cherche)
		return b;

	return _trouve_parent(b->next, b_cherche);
}

mem_free_block_t *trouve_parent(mem_free_block_t * fb){
	struct tete * tete = (struct tete *) mem_space_get_addr();
	return _trouve_parent(tete->next, fb);
}
// ---------------------------------------------

void *_show_parent(mem_free_block_t * b, mem_free_block_t * b_cherche){
	if(b_cherche == NULL || b == NULL || out_of_memory(b_cherche))
		return NULL;

	printf(" -> %ld", (long int) b);
	if(b->next == b_cherche){
		printf("-> %ld |\n", (long) b);
		return b;
	}
	return _show_parent(b->next, b_cherche);
}

void *show_parent(mem_free_block_t * fb){
	struct tete * tete = (struct tete *) mem_space_get_addr();
	printf("tete : %ld", (long int) tete);
	return _show_parent(tete->next, fb);
}

/* ---------------------------------------------
 * Fonction *fb_before_adresse
 * Prend en parametre un *fb et une adresse `adr`
 * Renvoie le dernier fb avant adr s'il existe
 * NULL sinon
 */
mem_free_block_t *fb_before_add(mem_free_block_t *fb, void * add){
    if(out_of_memory(add)) return NULL;

	if((void *)fb->next > add || fb->next == NULL)
        return fb;

    return fb_before_add(fb->next, add);
}
// ---------------------------------------------


/**---------------------------------------------
 * Fonction *pick_bb
 * Prend 3 parametres : 
 * Start  : bb a l'adresse duquel on se trouve
 * add    : adresse a laquelle un bb est cense etre
 * next_fb: fb a la suite le(s) block(s) de bb
 *
 * Exemple : 
 * ----------------------------------------------
 * |0  bb1  |4   bb2   |16    bb3    |30 next_fb|
 * ----------------------------------------------
 * pick_bb(bb1, 16, next_fb) -> bb3;
 * pick_bb(bb1, 4 , next_fb) -> bb1;
 * 
 * pick_bb(bb1, 15, next_fb) -> NULL;
 * pick_bb(bb1, 17, next_fb) -> NULL;
 * 
 * pick_bb(bb1, 30, next_fb) -> NULL;
**/
struct bb *pick_bb(struct bb * start, void * add, void * next_fb){
    // NULL si start est sur / depasse le prochain fb
    if(add == NULL || (void *)start >= next_fb || (void * )start > mem_space_get_last_addr())
        return NULL;
	
    if((void *)start == add)
        return start;
	
    return pick_bb(calc_add_bb(start), add, next_fb);
}
// ---------------------------------------------


/* ---------------------------------------------
 * Fonction *find_bb
 * Prend en parametre une adresse `adr`
 * Si adr est le debut d'un block renvoie ce bloc
 * NULL sinon
 */
struct bb *find_bb(void * add){
	if (out_of_memory(add) || add == NULL){
		printf("Out of memory or NULL! \n");
		return NULL;
	}

    struct tete * tete = (struct tete *) mem_space_get_addr();
    mem_free_block_t * fb = fb_before_add(tete->next, add);
	
	if (fb->next == NULL)
	    return pick_bb((struct bb *) calc_add_fb(fb), add, (void *) mem_space_get_last_addr());
	
    return pick_bb((struct bb *) calc_add_fb(fb), add, (void *) fb->next);
}
// ---------------------------------------------

/* ---------------------------------------------
 * Fonction fusion
 * Prend en parametre une adresse de bloc libre
 * Si les blocs adjacents sont libres, la fonction créer un seul bloc libre
 */
void fusion(mem_free_block_t *fb){
	mem_free_block_t * parent = trouve_parent(fb);
	mem_free_block_t * child = fb->next;

	if(calc_add_fb(parent) == fb && parent != adr_fict()){
		parent->size += fb->size;
		parent->next = fb->next;
		fb = parent;
	}

	if(child == NULL)
		return ;


	if(calc_add_fb(fb) == child){
		fb->size += child->size;
		fb->next = child->next;
	}
}

//-------------------------------------------------------------
// mem_init
//-------------------------------------------------------------

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

	bfict->size = sizeof(mem_free_block_t);
	mem1 = calc_add_fb(bfict);
	bfict->next = mem1;

	mem1->size = mem_space_get_size() - sizeof(mem_free_block_t) - sizeof(struct tete);
	mem1->next = NULL;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
	if (size < 0){
		printf("-> [ERROR] Taille negative demandee (%ld)\n;", size);
		return NULL;
	}

	if (size < sizeof(mem_free_block_t*)){
		printf("-> Taille demandee trop petite (%ld), taille attribuee : %ld\n;", size, sizeof(mem_free_block_t*));
		size = sizeof(mem_free_block_t*);
	}
	struct tete * tete = (struct tete *) mem_space_get_addr();

	mem_free_block_t * libre = (tete->fit)(tete->next, size + sizeof(struct bb));
	// Si il n'y as pas d'emplacement libre de taille `size` 
	if (libre == NULL)
		return NULL;

	mem_free_block_t * p_libre = trouve_parent(libre);
	// Si l'emplacement libre est bfict
	if (p_libre == NULL)
		return NULL;

	mem_free_block_t * next = libre->next;
 	int size_init = libre->size;
	struct bb * busy = (struct bb *) libre;
	busy->size = size;

	// Si il n'y a pas de place pour une zone libre
	// on attribue toute la zone a busy
	if(size_init - sizeof(mem_free_block_t) <= calc_bb(busy)){
		busy->size = size_init - sizeof(struct bb);
		p_libre->next = next;
	}
	else{
		libre = (mem_free_block_t * ) calc_add_bb(busy);
		libre->size = size_init - calc_bb(busy);
		libre->next = next;
		
		p_libre->next = libre;
	}

	return busy + 1;
}

//-------------------------------------------------------------
// mem_get_size
//-------------------------------------------------------------
size_t mem_get_size(void * zone) {
	struct bb* adrStruct = find_bb(zone);
	//Si zone n'est pas un bb
	if(adrStruct == NULL)
		return 0;
	
	return adrStruct->size;
}

//-------------------------------------------------------------
// mem_free
//-------------------------------------------------------------
/**
 * Free an allocaetd bloc.
**/
void mem_free(void *zone) {
	zone -= sizeof(struct bb);
	struct bb * bb = find_bb(zone);
	//Si ce n'est pas un bb renvoi NULL
	if (bb == NULL)
		return;
	
	struct tete* tete = (struct tete*) mem_space_get_addr();
	mem_free_block_t * fb_parent = fb_before_add(tete->next, zone);
 
	size_t bb_size = calc_bb(bb);
	mem_free_block_t* fb = (mem_free_block_t*) (bb);
	fb->size = bb_size;

	fb->next = fb_parent->next;
	fb_parent->next = fb;

	fusion(fb); 
}

//----------------------------------------------------------------
// Itérateur(parcours) sur le contenu de l'allocateur
// mem_show
// _mem_show fonction bis permettant la recursivite sur les blocks
//----------------------------------------------------------------

void _mem_show_bb(void (*print)(void *, size_t, int free), struct bb * bb, void * limite){
	if((void *)bb >= limite || (void *)bb >= mem_space_get_last_addr() || bb == NULL)
        return;

    print((void *) bb, bb->size, 0);
	
    return _mem_show_bb(print, calc_add_bb(bb), limite);
}

void _mem_show_fb(void (*print)(void *, size_t, int free), mem_free_block_t * fb) {
	if (fb == NULL || (void *)fb >= mem_space_get_last_addr())
		return;

	print((void *) fb, fb->size, 1);
    
    void * add_next = calc_add_fb(fb);
    if (add_next < (void *)fb->next)
        _mem_show_bb(print, (struct bb *) add_next, (void *) fb->next);

    if (fb->next == NULL)
        _mem_show_bb(print, (struct bb *) add_next, (void *) mem_space_get_last_addr());

    return _mem_show_fb(print, fb->next);
}

void mem_show(void (*print)(void *, size_t, int free)) {
	struct tete * tete = (struct tete *) mem_space_get_addr();
	// le premier bloc est forcement un fb, la tete est suivie du bloc fictif
	_mem_show_fb(print, tete->next);
}

//-------------------------------------------------------------
// mem_fit
//-------------------------------------------------------------
void mem_set_fit_handler(mem_fit_function_t *mff) {
	struct tete * tete = (struct tete *) mem_space_get_addr();
	tete->fit = mff;
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
	if(first_free_block == adr_fict())
		first_free_block = first_free_block->next;

	while (first_free_block != NULL){
		if(first_free_block->size >= wanted_size)
			return first_free_block;
		
		first_free_block = first_free_block->next;
	}
	return NULL;
}
//-------------------------------------------------------------
mem_free_block_t *mem_best_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
	mem_free_block_t * adresse = NULL;
	size_t diffTaille = mem_space_get_size();

	while(first_free_block != NULL){
		if(first_free_block->size >= wanted_size){
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
		if(first_free_block->size >= wanted_size){
			if(diffTaille < first_free_block->size - wanted_size){
				diffTaille = first_free_block->size - wanted_size;
				adresse = first_free_block;
			}
		}
		first_free_block = first_free_block->next;
	}
	return adresse;
}

void *mem_realloc(void *p, size_t size){
	if(p == NULL){
		return mem_alloc(size);
	}

	if(size == 0){
		mem_free(p);
		return NULL;
	}
	//Si la taille demandée est plus petite que struct bb, 
	//on lui donne la taille struct bb
	if(size < sizeof(struct bb))
		size = sizeof(struct bb);

	struct bb * bb = find_bb(p-sizeof(struct bb));
	//Si p n'est pas l'adresse d'une zone allouée on retourne null
	if(bb == NULL){
		return NULL;
	}
	
	struct tete* tete = (struct tete*) mem_space_get_addr();
	mem_free_block_t * fb = tete->next;

	mem_free_block_t * fb_parent = fb_before_add(fb,bb);
	//Si la taille demandée est la même que celle que bb occupe déjà
	//On retourne l'adresse du bloc 
	if(size == bb->size){
		return bb + 1;
	}
	else if(size < bb->size){
		bb->size=size;
		fb_parent->next -= (size-bb->size);
		(fb_parent->next)->size+=(bb->size-size);
		return bb + 1;
	}
	else{
		void *a = mem_alloc(size);
		memcpy(a, p, bb->size);
		mem_free(p);
		return a;
	}
}
