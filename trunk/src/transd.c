/* portions by Matthew Hawn */
/* TODO: honor keyboard focus events! */

/*
config files:
/etc/transd/transd.conf
HOME/.transd
*/

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <unistd.h>    /* getopt, exit */
#include <stdlib.h>    /* getenv */
#include <stdio.h>

#include "configfile.h"


struct transd_options options;
Display* dp;

const unsigned int OPAQUE = 0xffffffff;


void setOpacity ( Window w, unsigned int opacity )
{
	if (opacity == OPAQUE)
		XDeleteProperty ( dp, w,
			XInternAtom ( dp, "_NET_WM_WINDOW_OPACITY", False ) );
	else
		XChangeProperty ( dp, w,
			XInternAtom ( dp, "_NET_WM_WINDOW_OPACITY", False ), 
			XA_CARDINAL, 32, PropModeReplace,
			(unsigned char*)&opacity, 1L );
	
	XSync(dp, False);
	
	return;
}


void selectInput ( Window w )
{
	XSelectInput ( dp, w, EnterWindowMask | LeaveWindowMask );
	
	return;
}


void setOpacityRecursive ( Window w, unsigned int opacity )
{
	char* name;
	Window* children;
	Window dummy;
	unsigned int n;
	unsigned int i;

	XFetchName ( dp, w, &name );
	/*if ( name != NULL )*/
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


void walkWindowTree ( Window w, void (*func) ( Window ) )
{
	Window* children;
	Window dummy;
	unsigned int n;
	unsigned int i;

	func(w);

	XQueryTree ( dp, w, &dummy, &dummy, &children, &n );

	if ( n == 0 || children == NULL )
		return;

	for ( i = 0; i < n; ++i )
	{
		walkWindowTree ( children[i], func );
	}

	XFree ( children );

	return;
}


void show_usage()
{
	printf
	(
		"Usage: transd [-dhv] [-f <config-file>]"
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
	
	while ( (c = getopt ( argc, argv, ":Ldhf:v" ) ) != -1 )
	{
		switch (c)
		{
			case 'd':
				printf("Debugging messages enabled.\n");
				options.debug = 1;
			break;
			
			case 'f':
				options.config_file = strdup(optarg);
			break;
			
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


int main ( int argc, char** argv )
{
	dp = XOpenDisplay ( NULL );
	int scr = DefaultScreen ( dp );
	Window root = RootWindow ( dp, scr );
	XEvent event;

	char* name;

	
	parse_options ( argc, argv );
	
	cfg_parse_config_file ( "../templates" );
	/*if ( cfg_parse_config_file ( getenv(HOME)"" ) == -1 )
	{
		cfg_parse_config_file() || return(1);
	}*/
	
	/* select window-enter/-leave events from all currently existing windows */
	walkWindowTree ( root, selectInput );
	
	/* select substructure events from root window (so we can keep track of new windows) */
	XSelectInput ( dp, root, SubstructureNotifyMask );	


	for (;;)
	{
		XNextEvent ( dp, &event );
	
		switch ( event.type )
		{
			case CreateNotify:
				XFetchName ( dp, event.xcreatewindow.window, &name );
				XSelectInput ( dp, event.xcreatewindow.window,
					EnterWindowMask | LeaveWindowMask );
			break;
			
			case EnterNotify:
				XFetchName ( dp, event.xcrossing.window, &name );
				setOpacity ( event.xcrossing.window, OPAQUE );
			break;
			
			case LeaveNotify:
				XFetchName ( dp, event.xcrossing.window, &name );
				if ( event.xcrossing.detail != 2 )
					setOpacity ( event.xcrossing.window, OPAQUE * 0.75 );
			
			break;
			
			/*default:
				printf ( "Spurious event\n" );
			*/
		}
	}
	
	return(0);
}
