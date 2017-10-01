#include <stdio.h>
#include <stdlib.h>

#include <list.h>

void p_show(list_t *list);
void p_show_sorted(const list_t *list);

int main()
{
	list_t *list;
	int size=10;
	int i;
	l_iterator iter;
	
	/* Create list for type "int" of default blocksize */
	list = l_create_d(sizeof(int));
	
	/* Add numbers */
	for(i=0;i<size;i++) {
		int random = rand();
		l_add(list, &random);
	}
	
	p_show_sorted(list);
	
	
	/* Create iterator */
	iter = li_create(list);
	
	/* Iterate over list */
	while( li_iterate(&iter) != NULL)
	{
		/* Current iterator item is in (void*) <iterator>.element */
		printf("%d ", *(int*)iter.element );
	}
	printf("\r\n");
	
	p_show(list);
	
	printf("\r\n");
	
	/* Free all memory allocated */
	l_destroy(list);
	
	return 0;
}
/* Create function "lp_is_even" with params (list_t*)list, (int)index for predicate */
L_P_FUNCTION(is_even) 
{
	return *(int*)l_get(list,index)%2==0;
}

L_P_FUNCTION(is_ten)
{
	return l_get_v(list,index,int)%10==0;
}

/* Create function "la_print" with params (list_t*)list, (int)index for action */
L_A_FUNCTION(print)
{
	printf("%d ",*(int*)l_get(list,index));
}

void p_show_sorted(const list_t *list)
{
	/* Create copy of list to sort */
	list_t* sorted = l_clone(list);
	
	/* Sort using default int ascending comparator & quicksort algorithm */
	l_sort(sorted, QUICKSORT, &lc_int_a);
	
	/* Print values */
	l_do(sorted, &la_print);
	
	printf("\r\n");
	
	/* Destroy copy of list */
	l_destroy(sorted);
}

void p_show(list_t *list)
{
	int* newdata;
	int newsize;
	register int i=0;
	
	/* Copy all elements that satisfy lp_is_even into new array */
	newdata = (int*)l_toarray_n(list, &newsize, &lp_is_even);
	
	for(;i<newsize;i++) printf("%d ", newdata[i]);
	
	/* Free new array */
	free(newdata);
	
	printf("\r\n");
	
	/* Call la_print on all elements that satisfy lp_is_ten */
	l_do_if(list, &la_print, &lp_is_ten);
}