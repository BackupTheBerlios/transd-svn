#ifndef SLIST_H
#define SLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct _slist
{
	void* data;
	struct _slist* next;
};
typedef struct _slist slist;


void slist_add ( slist** list, void* i );
void slist_print ( slist* list, void (*print)( void * ) );
int slist_size ( slist* list );


#endif
