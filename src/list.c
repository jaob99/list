#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <list.h>

#define l_block struct l_block

void _l_quicksort(list_t *arr, int left, int right, _l_comparator comp);

static int _mem_isnull(unsigned char* data, size_t le)
{
	unsigned char b = 0;
	register int i=0;
	for(;b==0&&i<le;i++)
		b |= data[i];
	return b==0;
}

static l_block* _l_create_block(size_t type,int blocksize)
{
	l_block* block= (l_block*) malloc(sizeof(l_block*) + ( blocksize*type));
	memset(block, 0, sizeof(l_block*) + ( blocksize*type));
	return block;
}

static l_block* _l_next_block(l_block* block)
{
	return block->next;
}

static l_block* _l_block_skip(l_block* block, int skip)
{
	l_block* c = block;
	register int i=0;
	for(;i<skip;i++) if( (c = c->next) == NULL) break;
	return c;
}

static void* _l_at(l_block* block, size_t size, int index)
{
	return (void*)(&block->data[index*size]);
}

list_t *l_create(size_t type,int bs)
{
	list_t *list = (list_t*)malloc(sizeof(list_t));
	list->length=0;
	list->blocksize = bs;
	list->type = type;
	list->block0 = _l_create_block(type,bs);
	
	return list;
}

void l_destroy(list_t *t)
{
	l_block* next=NULL;
	l_block* current = t->block0;
	do
	{
		next = _l_next_block(current);
		free(current);
		current = next;
	} while(next!=NULL);
	free(t);
}

void l_copy(const list_t* src,int src_offset,list_t* dest, int dest_offset,int len)
{
	register int i=0;
	
	for(;i<len;i++)
	{
		void* s_ptr = l_get(src, src_offset+i);
		if(i<dest->length) {
			void* d_ptr = l_get(dest, dest_offset+i);
			memcpy(d_ptr, s_ptr, src->type);
		} else l_add(dest, s_ptr);
	}
}

void l_copy_ar(const list_t* src, int src_offset, void* dest, int len)
{
	register int i=0;
	unsigned char* by = (unsigned char*) dest;
	
	for(;i<len;i++)
	{
		void* s_ptr = l_get(src, src_offset+i);
		memcpy( by, s_ptr, src->type);
		by+=src->type;
	}
}

list_t* l_create_from(void* from, int items, size_t type, int bs)
{
	list_t *l = l_create(type, bs);
	register int i=0;
	
	for(;i<items;i++)
		l_add(l, (void*)(((unsigned char*)from)+(i*type)));
	return l;
}

/*
static l_block* _l_last_block(l_block* b0)
{
	while(b0->next!=NULL)
	{
		b0 = b0->next;
	}
	return b0;
}*/ /** currently unused **/

void l_add(list_t *t,const void* o)
{
	int real_i = t->length%t->blocksize;
	int block_skip = t->length/t->blocksize;
	l_block *wbl;
	
	if(t->length>0 && ((t->length%t->blocksize)==0))
	{
		l_block *prev = _l_block_skip(t->block0, block_skip-1);
		wbl = prev->next = _l_create_block(t->type,t->blocksize);
	}
	else
		wbl = _l_block_skip(t->block0, block_skip);
	memcpy(_l_at(wbl, t->type, real_i), o, t->type);
	t->length+=1;
}

void l_nullify(list_t *t, int index)
{
	memset(l_get(t, index), 0, t->length);
}

void* l_get(const list_t *t, int index)
{
	int real_i = index%t->blocksize;
	int block_skip = index/t->blocksize;
	l_block *wbl= _l_block_skip(t->block0, block_skip);
	
	return _l_at(wbl, t->type, real_i);
}

static void* _l_ptoarray(const list_t* t,int *size, _l_predicate p)
{
	void* output0 = calloc(t->length, t->type);
	void* output;
	register int i=0,j=0;
		
	for(;i<t->length;i++)
	{
		if(p(t, i)) {
			memcpy( (void*)(((unsigned char*)output0)+(j*t->type)), l_get(t, i), t->type);
			j+=1;
		}
	}
	
	output = calloc(j, t->type);
	memcpy(output, output0, t->type*j);
	free(output0);
	
	if(size!=NULL) *size  = j;
	
	return output;
}

void* l_toarray_n(const list_t *t, int* size, _l_predicate p)
{
	if(p==NULL)
	{
		void* output = calloc(t->length, t->type);
		register int i=0;
		
		for(;i<t->length;i++)
			memcpy( (void*)(((unsigned char*)output)+(i*t->type)), l_get(t, i), t->type);
		
		if(size!=NULL) *size= (t->length);
		return output;
	}
	else return _l_ptoarray(t,size,p);
}

void l_toarray(const list_t *t, void* dest, size_t len)
{
	int real_i = len%t->blocksize;
	int block_skip = len/t->blocksize;
	l_block* b = t->block0;
	register int i=0;
	
	for(;i<block_skip;i++)
	{
		memcpy( (void*)(((unsigned char*)dest)+((i*t->blocksize)*t->type)), b->data, t->type*t->blocksize);

		b = b->next;
		
	}
	
	memcpy( (void*)(((unsigned char*)dest)+((block_skip*t->blocksize)*t->type)), b->data, t->type* real_i);
	
}

int l_isnull(const list_t *t, int index)
{
	void* ptr = l_get(t, index);
	return _mem_isnull((unsigned char*)ptr, t->type);
}

void l_nullify_if(list_t *t, _l_predicate f)
{
	register int i=0;
	for(;i<t->length;i++)
		if(f(t, i)) l_nullify(t, i);
}

void l_do_if(list_t *t, _l_action act, _l_predicate f)
{
	register int i=0;
	for(;i<t->length;i++)
		if(f(t, i)) act(t, i);
}

void l_do(list_t *t, _l_action act)
{
	register int i=0;
	for(;i<t->length;i++)
		act(t, i);
}


void l_swap(list_t *t, int index1, int index2)
{
	void* temp = malloc(t->type);
	memcpy(temp, l_get(t, index1), t->type);
	memcpy(l_get(t, index1), l_get(t,index2), t->type);
	memcpy(l_get(t,index2), temp, t->type);
	free(temp);
}

void l_sort(list_t *t, _l_sort_alg algorithm, _l_comparator comp)
{
	switch(algorithm)
	{
		case QUICKSORT:
			_l_quicksort(t, 0, t->length-1, comp);
			break;
	}
}

list_t* l_clone(const list_t *t)
{
	list_t *n = l_create(t->type, t->blocksize);
	l_copy(t, 0, n, 0, t->length);
	return n;
}

list_t* l_clone_if(const list_t *t , _l_predicate f)
{
	list_t *n = l_create(t->type, t->blocksize);
	register int i=0;
	for(;i<t->length;i++)
		if(f(t, i)) l_add(n, l_get(t,i));
	return n;
}

/* Iterator stuff */

int li_next(l_iterator *it)
{
	it->index+=1;
	it->element = NULL;
	return it->index<it->list->length;
}

l_iterator li_create(list_t *list)
{
	l_iterator i;
	i.index = 0;
	i.list = list;
	i.element = NULL;
	return i;
}

void* li_set(l_iterator *i,int index)
{
	i->index = index;
	return li_get(i);
}

void* li_get(l_iterator *i)
{
	if(i->index<i->list->length) {
		return i->element = l_get(i->list, i->index);
	}
	else return i->element = NULL;
}

void* li_iterate(l_iterator *i)
{
	if(i->index<i->list->length) {
		void* p = l_get(i->list, i->index);
		li_next(i);
		return i->element = p;
	}
	else return i->element =  NULL;
}

/* Predefined predicates */

L_P_FUNCTION(not_null)
{
	return !l_isnull(list,index);
}

/* Predefined Actions */

L_A_FUNCTION(assert_zero)
{
	assert(0);
}

/* Predefined Comparators */

L_C_FUNCTION(int_a)
{
	int v1 = l_get_v(list, i1, int);
	int v2 = l_get_v(list, i2, int);
	int comp_res = v1>v2?1:(((v1==v2)?0:-1));
	return comp_res;
}

#undef l_block