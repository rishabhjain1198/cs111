//NAME: RISHABH JAIN
//EMAIL: rishabh.jain1198@gmail.com
//ID: 604817863

#define _GNU_SOURCE
#include "SortedList.h"
#include<sched.h>
#include <string.h>
#include<stdio.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
    if (list == NULL || element == NULL)
        return;
    SortedList_t *prev = list;
    SortedList_t *cur = prev->next;

    while (cur != list) {
        if (strcmp(cur->key, element->key) < 0)
            break;
     //   fprintf(stderr, "LOOPING\n");
        prev = cur;
        cur = cur->next;
    }

    if (opt_yield & INSERT_YIELD)
        sched_yield();
    
    element->next = prev->next;
    element->prev = prev;
    prev->next = element;
    element->next->prev = element;
}

int SortedList_delete(SortedListElement_t *element)
{
    if(element == NULL)
        return -1;

    if(element->next->prev == element->prev->next)
    {
        if(opt_yield & DELETE_YIELD)
            sched_yield();

        element->prev->next = element->next;
        element->next->prev = element->prev;
        return 0;
    }
    return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
    if(list == NULL || key == NULL)
        return NULL;
    SortedListElement_t *curr = list->next;
    while(curr != list)
    {
        if(strcmp(curr->key, key) == 0)
            return curr;
        if(opt_yield & LOOKUP_YIELD)
            sched_yield();
        curr = curr->next;
    }
    return NULL;
}

int SortedList_length(SortedList_t *list)
{
    int counter = 0;
    if(list == NULL)
        return -1;
    SortedListElement_t *curr = list->next;
    while(curr != list && curr != NULL)
    {
        counter++;
        if(opt_yield & LOOKUP_YIELD)
            sched_yield();
        curr = curr->next;
    }
    return counter;
}
