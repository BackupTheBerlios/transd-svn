#include "slist.h"


void slist_print ( slist* list, void (*print)( void * ) )
{	
	slist* p = list;
	
	if ( p == NULL )
	{
		return;
	}
	
	while ( p != NULL )
	{
		assert(p->data != NULL);
		
		print ( p->data );
		p = p->next;
	}
	
	return;
}


void slist_add ( slist** list, void* i )
{
	slist* p = *list;
	slist* newitem = malloc ( sizeof(slist) );

	newitem->data = i;
	newitem->next = NULL;
	
	if ( p == NULL )
	{
		*list = newitem;
		
		return;
	}
	
	while ( p->next != NULL ) p = p->next;
	p->next = newitem;

	return;
}


int slist_size ( slist* list )
{
	slist* p = list;
	int s = 0;
	
	if ( p == NULL )
	{
		return(0);
	}
	
	while ( p != NULL )
	{
		++s;
		p = p->next;
	}
	
	return(s);
}
