#include "slist.h"

void print_int ( void* data )
{
	printf ( "%d\n", *((int*)data) );
	
	return;
}

int main()
{
	int a = 1, b = 2, c = 3;

	slist* mylist = NULL;

	slist_print ( mylist, print_int );
	
	slist_add ( &mylist, &a );
	slist_print ( mylist, print_int );
	slist_add ( &mylist, &b );
	slist_print ( mylist, print_int );
	slist_add ( &mylist, &c );
	slist_print ( mylist, print_int );
	
	return(0);
}
