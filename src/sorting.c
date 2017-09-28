#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <list.h>

void _l_quicksort(list_t *arr, int left, int right, _l_comparator comp)
{
    int m = (left+right)/2;
    int l = left, r = right;
    while (l <= r) {
        while (l_bcheck(arr, l) && (comp(arr, l, m) < 0)) l+=1;
        while (l_bcheck(arr, r) && (comp(arr, r, m) > 0)) r-=1;
        if (l <= r) {
            l_swap(arr, l, r);
            l+=1; r-=1;
        }
    }
    if (r > left) _l_quicksort(arr, left, r, comp);
    if (l < right) _l_quicksort(arr, l, right, comp);
}