#ifndef _LIST_H
#define _LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define L_DEFAULT_BLOCKSIZE 16

/* Functional Macros */

#define l_create_d(type) l_create(type, L_DEFAULT_BLOCKSIZE)
#define l_create_from_d(from, length, type) l_create_from(from,length, type, L_DEFAULT_BLOCKSIZE)
#define l_get_v(list,index, type) *(type*)l_get(list,index)

/** Returns 1 if <index> is a valid index for <t> **/
#define l_bcheck(t, index) (index>=0&&index<t->length)

/* Enums */

typedef enum {
	QUICKSORT
} _l_sort_alg;

/* Structures */

struct l_block {
	struct l_block* next;
	unsigned char data[];
};

typedef struct {
	int length;
	size_t type;
	int blocksize;
	struct l_block* block0;
} list_t;

typedef struct {
	list_t *list;
	void* element;
	int index;
} l_iterator;

/* Function Pointers */

#define L_P_FUNCTION(name) int lp_ ## name(const list_t *list, int index)
#define L_A_FUNCTION(name) void la_ ## name(const list_t *list, int index)
#define L_C_FUNCTION(name) int lc_ ## name(const list_t *list, int i1, int i2)
typedef int (*_l_predicate)(const list_t *, int);					/** Takes (list), (index). Returns non-zero if true **/
typedef int (*_l_comparator)(const list_t*, int,int);				/** Takes (list), (i1), (i2). Returns 0 if equal, -1 if <i1> goes before <i2>, +1 if <i1> goes after <i2> **/
typedef void (*_l_action)(const list_t*, int);						/** Takes (list), (index) **/

/* Functions */
#ifdef __cplusplus
extern "C" {
#endif

list_t*	l_create(size_t type,int blocksize);						/** Allocates new list_t aligned to size <type> (e.g. "sizeof(int)"), and first block with <blocksize> (use l_create_d(size_t) to use default blocksize) **/
list_t* l_create_from(void* from, int items, size_t type, int bs);	/** Allocates new list_t and copies elements from <from> into it **/
void	l_destroy(list_t *t);										/** Frees all memory alocated to <t> and then frees <t> **/
void	l_add(list_t *t, const void* o);							/** Adds new element at end of array with copy of data at <o>, then updates ->length pointer **/
void*	l_get(const list_t *t, int index);							/** Returns pointer to element at <index> **/
void	l_nullify(list_t *t, int index);							/** Writes over element at <index> with 0s (bytes) **/
void 	l_copy(const list_t* src,int so,list_t* de, int o,int len);	/** Copies <len> elements from <src> (offset by <so>) to <de> (offset by <o>) **/
void 	l_copy_ar(const list_t* src, int so, void* dest, int len);	/** Copies <len> elements from <src> (offset by <so>) to <dest> **/
void*	l_toarray_n(const list_t *t, int* size, _l_predicate p);	/** Creates a new array and copies each element that satisfies <p>, or if <p> is NULL, all of them. Size of the new array is returned in <size> unless <size> is NULL **/
void	l_toarray(const list_t *t, void* dest, size_t len);			/** Copies <len> elements from <t> to <dest> **/
int 	l_isnull(const list_t *t, int index);						/** Returns 1 if all bytes of element at <index> are 0 **/
void 	l_nullify_if(list_t *t, _l_predicate f);					/** Nullifies elements that satisfy <f> **/
void 	l_do(list_t *t, _l_action act);								/** Calls <act> on all elements of <t> **/
void 	l_do_if(list_t *t, _l_action act, _l_predicate f);			/** Calls <act> on elements that satisfy <f> **/
void 	l_swap(list_t *t, int index1, int index2);					/** Swap elements at <index1> and <index2> **/
void 	l_sort(list_t *t, _l_sort_alg alg, _l_comparator comp);		/** Sorts <t> with algorithm <alg> using comparator <comp> **/
list_t* l_clone(const list_t *t);									/** Creates new list from <t> **/
list_t* l_clone_if(const list_t *t, _l_predicate f);				/** Creates new list from <t> with all elements that satisfy <f> **/
	
l_iterator	li_create(list_t *list);								/** Creates iterator from <list> and initialises at index 0 **/
int			li_next(l_iterator *it);								/** Updates current element and increments pointer in <it> and returns non-zero if index is in bounds **/
void* 		li_get(l_iterator *i);									/** Updates and returns current element of iterator, or NULL if not in bounds **/
void*		li_iterate(l_iterator *i);								/** Updates and returns current element of iterator and increments pointer, or returns NULL if not in bounds **/
void*		li_set(l_iterator *i,int index);						/** Sets current index to <index> then updates and returns current element, or NULL if index is not in bounds **/


/* Predefined Predicates */

L_P_FUNCTION(not_null);		/** Allows all non-null elements **/

/* Predefined Actions */

L_A_FUNCTION(assert_zero);	/** Call assert(0) **/

/* Predefined Comparators */

L_C_FUNCTION(int_a);		/** Ascending sort for (int) type **/

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H */