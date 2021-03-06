/* setOpacity() by Matthew Hawn */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xmu/Xmu.h>

#include <unistd.h>    /* getopt, exit */
#include <stdlib.h>    /* getenv */
#include <stdio.h>
#include <signal.h>

#include "configfile.h"
#include "slist.h"

struct transd_options options;
Display* dp;

const unsigned int OPAQUE = 0xffffffff;

slist* window_list = NULL;


void walkWindowList ( void (*func) ( Window, void* ), void* data )
{
	slist* p = window_list;
	
	if ( p == NULL )
	{
		return;
	}
	
	while ( p != NULL )
	{
		assert(p->data != NULL);
		
		DEBUG(10, "window# 0x%x\n", *(Window*)(p->data));
		
		func ( *(Window*)(p->data), data );
		p = p->next;
	}
	
	return;
}


void print_window_information ( Window w, void* data )
{
	char* name; XFetchName ( dp, w, &name );
	
	printf("\tWindow 0x%x / '%s'\n", w, name );
	
	return;
}


void SIGINT_handler ( int signum )
{
	printf("Exiting on SIGINT...\n");
	exit(3);
}


void SIGHUP_handler ( int signum )
{
	printf("Window list contents:\n---------------------\n");
	
	walkWindowList ( print_window_information, NULL );
	
	printf("---------------------\n");
	
	return;
}


void setOpacity ( Window w, unsigned int opacity )
{
	char* name;
	XFetchName ( dp, w, &name );	
	
	if ( opacity == OPAQUE )
		XDeleteProperty ( dp, w,
			XInternAtom ( dp, "_NET_WM_WINDOW_OPACITY", False ) );
	else
		XChangeProperty ( dp, w,
			XInternAtom ( dp, "_NET_WM_WINDOW_OPACITY", False ), 
			XA_CARDINAL, 32, PropModeReplace,
			(unsigned char*) &opacity, 1L );
	
	XSync ( dp, False );
	
	return;
}


static unsigned int getOpacity ( Window w, unsigned int def )
{
    Atom actual;
    int format;
    unsigned long n, left;

    unsigned char *data;
	
    int result = XGetWindowProperty ( dp, w,
	XInternAtom ( dp, "_NET_WM_WINDOW_OPACITY", False ),
	0L, 1L, False, XA_CARDINAL, &actual, &format, &n, &left, &data);

	
	
    if ( result == Success && data != NULL )
    {
	unsigned int i;
	memcpy ( &i, data, sizeof (unsigned int) );
	XFree ( (void*) data );
	return i;
    }
    
    return(def);
}


void setOpacityRecursive ( Window w, unsigned int opacity )
{
	char* name;
	Window* children;
	Window dummy;
	unsigned int n;
	unsigned int i;

	XFetchName ( dp, w, &name );
	setOpacity ( w, opacity );

	XQueryTree ( dp, w, &dummy, &dummy, &children, &n );

	if ( n == 0 || children == NULL )
		return;

	for ( i = 0; i < n; ++i )
	{
		setOpacityRecursive ( children[i], opacity );
	}

	XFree ( children );

	return;
}


void selectInput ( Window w, void* data )
{
	XWMHints* wmhints = XGetWMHints(dp, w);
	XWindowAttributes wattr;
	
	XGetWindowAttributes(dp, w, &wattr);
	
	XSelectInput ( dp, w, VisibilityChangeMask );
	
	DEBUG(10, "w is 0x%x\n", w);
	
	if ( wmhints != NULL && wmhints->initial_state == 1 && wattr.map_state == 2)
	{
		Window* wp = (Window*)malloc(sizeof(Window));
		*wp = w;
		
		XSelectInput ( dp, w, EnterWindowMask | LeaveWindowMask );
	
		DEBUG(4, "inserting window # 0x%x\n", *wp);
		
		slist_add ( &window_list, wp );
	}
	else if ( wattr.map_state == 2 )
	{
		XSelectInput ( dp, w, EnterWindowMask | LeaveWindowMask );
	
		DEBUG(4, "select isViewable window # 0x%x\n", w);		
	}

	XFree ( wmhints );
	
	return;
}


void executeRule ( Window w, void* data )
{
	
	cfg_rule* rule = (cfg_rule*) data;
	char* name;
	
	if ( !cfg_check_property ( dp, w, rule->action.property, rule->action.value ) )
	{
		Window dummy, parent, *children;
		unsigned int n;
		
		XQueryTree ( dp, w, &dummy, &parent, &children, &n );
		
		setOpacity ( parent, rule->action.opacity );
	}
	
	return;
}


void walkWindowTree ( Window w, void (*func) ( Window, void* ), void* data )
{
	Window* children;
	Window dummy;
	unsigned int n;
	unsigned int i;

	func ( w, data );

	XQueryTree ( dp, w, &dummy, &dummy, &children, &n );

	if ( n == 0 || children == NULL )
		return;

	for ( i = 0; i < n; ++i )
	{
		walkWindowTree ( children[i], func, data );
	}

	XFree ( children );

	return;
}


void removeWindowFromList ( Window w )
{
	slist* p = window_list;
	
	if ( p == NULL )
	{
		return;
	}
	
	while ( p != NULL )
	{
		assert(p->data != NULL);
		
		DEBUG(10, "window# 0x%x\n", *(Window*)(p->data));
		
		if ( *(Window*)(p->data) == w )
		{
			if ( (p - 1) != NULL )
				(p - 1)->next = p->next;
		}
		
		p = p->next;
	}
	
	return;
}


void show_usage()
{
	printf
	(
		/*"Usage: transd [-dhLv] [-f <config-file>]\n"*/
		"Usage: transd [-dhLv]\n"
	);
	exit(2);
}


void show_version()
{
	printf
	(
		"transd Version 0.2 (C)2005 Patrick Leslie Polzer\n"
		"Run with the -L switch for license information.\n"
	);
	
	exit(0);
}


void show_license()
{
	printf
	(
		"This program is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation; either version 2 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with this program; if not, write to the Free Software\n"
		"Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n"
	);
	exit(0);
}


void parse_options ( int argc, char** argv )
{
	extern char *optarg;
	extern int optind, optopt;	
	
	char c;
	
	while ( (c = getopt ( argc, argv, ":Ld:hv" ) ) != -1 )
	{
		switch (c)
		{
			case 'd':
				printf("Debugging messages enabled.\n");
				options.debug = atoi(optarg);
			break;
			
			/*case 'f':
				options.config_file = strdup(optarg);
			break;*/
			
			case 'h':
				show_usage();
			break;
			
			case 'v':
				show_version();
			break;
			
			case 'L':
				show_license();
			break;
			
			case ':':
				fprintf ( stderr, "Option -%c requires an operand\n", optopt );
				show_usage();
			break;
			
			case '?':
				fprintf ( stderr, "Unrecognized option: -%c\n", optopt );
				show_usage();
			break;
		}
	}
	    
	return;
}


cfg_rule* cached_rule;
Window cached_window;

int main ( int argc, char** argv )
{
	dp = XOpenDisplay ( NULL );
	int scr = DefaultScreen ( dp );
	Window root = RootWindow ( dp, scr );
	XEvent event;
	cfg_rule* rule;
	
	char* name;

	signal(SIGINT, SIGINT_handler);
	signal(SIGHUP, SIGHUP_handler);
	
	parse_options ( argc, argv );
	
	char* configfile = (char*) malloc ( strlen ( getenv("HOME") ) + strlen ("/.transd") + 1 );
	
	*configfile = '\0';
	DEBUG(1, "configfile: %s\n", configfile );
	DEBUG(1, "configfile: %s\n", configfile );
	
	if ( cfg_parse_config_file ( configfile ) == -1 )
	{
		DEBUG(1, "User-based config-file %s/transd.conf not found!\n", configfile);
		DEBUG(1, "Using global configuration file /etc/transd/transd.conf\n");
		
		if ( cfg_parse_config_file ( "/etc/transd" ) == -1 )
		{
			printf("Error: Could not find any config file! Exiting...\n");
			return(1);
		}
	}
	
	free ( configfile );

	XSetErrorHandler ( XmuSimpleErrorHandler );
	
	/* select window-enter/-leave events from all currently existing windows */
	walkWindowTree ( root, selectInput, NULL );
	
	DEBUG(1, "got %d initial windows in list\n", slist_size(window_list));
	
	/* select substructure events from root window (so we can keep track of new windows) */
	XSelectInput ( dp, root, SubstructureNotifyMask | EnterWindowMask );


	for (;;)
	{
		XNextEvent ( dp, &event );
	
		switch ( event.type )
		{
			case CreateNotify:
				DEBUG(5, "CreateNotify for 0x%x\n", event.xcreatewindow.window);
				selectInput ( event.xcreatewindow.window, NULL );
			break;
			
			case EnterNotify:
				DEBUG(5, "EnterNotify for 0x%x (detail %d)\n", event.xcrossing.window, event.xcrossing.detail);
				
				rule = cfg_get_rule ( dp, event.xcrossing.window, "Enter" );

				if ( rule != NULL )
				{
					DEBUG(5, "got rule for 0x%x\n", event.xcrossing.window);
					if ( !strcmp ( rule->action.property, "__TRANSD_SELF" ) )
					{
						Window dummy, parent, *children;
						unsigned int n;
						
						XQueryTree ( dp, event.xcrossing.window, &dummy, &parent, &children, &n );
						
						setOpacity ( parent, rule->action.opacity );
					}
					else
						walkWindowList ( executeRule, rule );
					
					/* check whether this Enter-rule has reverted the cached Leave-rule */
					if ( cached_rule != NULL )
					{
						Window dummy, parent, *children;
						unsigned int n;

						DEBUG(4, "SMART: A1. found cached rule for 0x%x\n", cached_window);
						
						XQueryTree ( dp, cached_window, &dummy, &parent, &children, &n );						
						DEBUG(4, "Opacity matching: 0x%x == 0x%x ?\n", getOpacity(parent, 0),
							rule->action.opacity );
						if ( getOpacity(parent, 0) == rule->action.opacity )
						{
							DEBUG(4,"SMART: A2. opac matching\n");
							if ( cfg_check_property ( dp, cached_window,
								rule->action.property, rule->action.value ) )
							{
								DEBUG(4,"SMART: A3. rule was not reverted, executing...\n");
								/* left window is not affected by this rule, execute cached rule */
								if ( !strcmp ( cached_rule->action.property, "__TRANSD_SELF" ) )
								{
									setOpacity ( parent, cached_rule->action.opacity );
								}
								else					
									walkWindowList ( executeRule, rule );
								
								cached_rule = NULL;
							}
						}
					}
						

				}
				else if ( event.xcrossing.detail != 4 )
				{
					
					DEBUG(5, "no rule for 0x%x\n", event.xcrossing.window);
					/* execute cached rule */
					if ( cached_rule != NULL )
					{
						DEBUG(5, "SMART: B1. found cached rule for 0x%x\n", cached_window);
						
						if ( !strcmp ( cached_rule->action.property, "__TRANSD_SELF" ) )
						{
							Window dummy, parent, *children;
							unsigned int n;
							
							XQueryTree ( dp, cached_window, &dummy, &parent, &children, &n );
							
							setOpacity ( parent, cached_rule->action.opacity );
						}
						else					
							walkWindowList ( executeRule, cached_rule );					
						
						cached_rule = NULL;
					}
				}
					
			break;
			
			case LeaveNotify:
				DEBUG(5, "LeaveNotify for 0x%x (detail %d)\n", event.xcrossing.window, event.xcrossing.detail);
				if ( event.xcrossing.detail != 2 )
				{
					rule = cfg_get_rule ( dp, event.xcrossing.window, "Leave" );
					
					
					if ( rule != NULL )
					{
						DEBUG(5, "got rule for 0x%x\n", event.xcrossing.window);
						
						/* cache rule and current opacity */
						DEBUG(5, "caching rule for 0x%x\n", event.xcrossing.window);
						cached_rule = rule;
						cached_window = event.xcrossing.window;
					}
					else
					{
						DEBUG(5, "no rule for 0x%x\n", event.xcrossing.window);
					}
				}
			break;
				
			case VisibilityNotify:
				DEBUG(5, "VisibilityNotify for 0x%x (state %d)\n", event.xvisibility.window, event.xvisibility.state);
				switch ( event.xvisibility.state )
				{
					case VisibilityFullyObscured:
						DEBUG(5, "Removing 0x%x from our list\n", event.xvisibility.window);
						removeWindowFromList ( event.xvisibility.window );
						DEBUG(1, "got %d windows in list now\n", slist_size(window_list));
					break;
					
					default:
						DEBUG(5, "Adding 0x%x to our list\n", event.xvisibility.window);
						selectInput( event.xvisibility.window, NULL );
						DEBUG(1, "got %d windows in list now\n", slist_size(window_list));
				}
			break;
				
			case DestroyNotify:
				DEBUG(5, "DestroyNotify for 0x%x (state %d)\n", event.xdestroywindow.window);
				DEBUG(5, "Removing 0x%x from our list\n", event.xdestroywindow.window);
				removeWindowFromList ( event.xdestroywindow.window );
			break;
		}
	}
	
	return(0);
}
