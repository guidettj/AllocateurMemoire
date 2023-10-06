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
	return sizeof(mem_free_block_t) + add->size;
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
	return (mem_free_block_t *) ((char *)add + sizeof(mem_free_block_t) + add->size);
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


/* ---------------------------------------------
 * Fonction *fb_before_adresse
 * Prend en parametre un *fb et une adresse `adr`
 * Renvoie le dernier fb avant adr s'il existe
 * NULL sinon
 */
mem_free_block_t *fb_before_add(mem_free_block_t *fb, void * add){
    if(fb->next == NULL || out_of_memory(add))
        return NULL;

    if((void *)fb->next > add)
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
	printf("add = %p\n", add);
    if(add == NULL || (void *)start >= next_fb)
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
    return pick_bb((struct bb *) calc_add_fb(fb), add, (void *) (fb->next));
}
// ---------------------------------------------


void fusion(mem_free_block_t *fb){
	mem_free_block_t * parent = trouve_parent(fb);
	mem_free_block_t * child = fb->next;

	if(calc_add_fb(parent) == fb && parent != adr_fict()){
		parent->size += calc_fb(fb);
		parent->next = fb->next;
	}

	if(child == NULL)
		return ;
	printf("@child = %p\n", child);
	printf("@next to fb = %p\n", calc_add_bb((struct bb *) fb));
	printf("fb->size = %ld\n", fb->size);
	

	if((mem_free_block_t *) calc_add_fb(fb) == child){
		fb->size += calc_fb(child);
		fb->next = child->next;
		child = NULL;
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

	bfict->size = 0;
	mem1 = calc_add_fb(bfict);
	bfict->next = mem1;

	mem1->size = mem_space_get_size() - sizeof(mem_free_block_t) * 2 - sizeof(struct tete);
	printf("sizeof(mem_free_block_t) : %ld\n", sizeof(mem_free_block_t));
	printf("sizeof(struct tete) : %ld\n\n", sizeof(struct tete));

	printf("Memoire totale : %ld\n", mem_space_get_size());
	printf("Memoire debut  :%ld\n\n", mem_space_get_size() - sizeof(mem_free_block_t) * 2 - sizeof(struct tete));

	printf("Adresse mem_d : %p\n", mem_space_get_addr());

	printf("Adresse Tete  : %p\n", tete);
	printf("Adresse bfict : %p\n", bfict);
	printf("Adresse mem1  : %p\n\n", mem1);

	printf("Adresse parent mem1 : %p\n", trouve_parent(mem1));

	printf("Adresse mem1  : %p\n\n", mem1);

	mem1->next = NULL;
}

//-------------------------------------------------------------
// mem_alloc
//-------------------------------------------------------------
/**
 * Allocate a bloc of the given size.
**/
void *mem_alloc(size_t size) {
	struct tete * tete = (struct tete *) mem_space_get_addr();

	mem_free_block_t * libre = (tete->fit)(tete->next, size + sizeof(struct bb));
	// pas d'emplacement libre de taille `size` 
	if (libre == NULL)
		return NULL;
		
	mem_free_block_t * p_libre = trouve_parent(libre);
	if (p_libre == NULL)
		return NULL;

 	int size_init = libre->size;

	struct bb * busy = (struct bb *) libre;
	busy->size = size;

	if(calc_fb(libre) <= calc_bb(busy)){
		// si <0 alors pas de place pour un struct fb
		// on va attribuer la taille a busy
		
		busy->size = calc_fb(libre) - (sizeof(mem_free_block_t) - sizeof(struct bb));

		p_libre->next = libre->next;
		libre = NULL;
	}
	else{
		libre = (mem_free_block_t * ) calc_add_bb(busy);
		libre->size = size_init - calc_bb(busy);
		p_libre->next = libre;
	}

	if(out_of_memory(p_libre->next + sizeof(mem_free_block_t))){
		printf("aaa\n");
		p_libre->next = NULL;
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
	
	struct bb * bb = find_bb(zone);
	printf("zone = %p\n", zone);
	if (bb == NULL)
		return;
	
	struct tete* tete = (struct tete*)mem_space_get_addr();
	mem_free_block_t * fb_parent = fb_before_add(tete->next, zone);
	
	printf("2\n");
    //chercher si l'adresse de la structure est bien un bb
	size_t bb_size = mem_get_size(zone);
	// printf("bb_size")
	mem_free_block_t* fb = (mem_free_block_t*) (bb);
	printf("3\n");
	
	fb->size = bb_size - (sizeof(mem_free_block_t) - sizeof(struct bb)); 
	printf("4\n");

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

	if(limite == mem_space_get_last_addr())
		return;
	
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
	//TODO: implement
	struct tete * tete = (struct tete *) mem_space_get_addr();
	tete->fit = mff;
}

//-------------------------------------------------------------
// Stratégies d'allocation
//-------------------------------------------------------------
mem_free_block_t *mem_first_fit(mem_free_block_t *first_free_block, size_t wanted_size) {
	while (first_free_block != NULL){
		if(calc_fb(first_free_block) >= wanted_size)
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
		if(calc_fb(first_free_block) > wanted_size){
			if(diffTaille > calc_fb(first_free_block) - wanted_size){
				diffTaille = calc_fb(first_free_block) - wanted_size;
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
		if(calc_fb(first_free_block) > wanted_size){
			if(diffTaille < calc_fb(first_free_block) - wanted_size){
				diffTaille = calc_fb(first_free_block) - wanted_size;
				adresse = first_free_block;
			}
		}
		first_free_block = first_free_block->next;
	}
	return adresse;
}
