/* portions by Matthew Hawn */
/* TODO: honor keyboard focus events! */

/*
config files:
/etc/transd/transd.conf
HOME/.transd
*/

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <stdio.h>

#include "configfile.h"


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


int main()
{
	dp = XOpenDisplay ( NULL );
	int scr = DefaultScreen ( dp );
	Window root = RootWindow ( dp, scr );
	XEvent event;

	char* name;
	
	cfg_parse_config_file ( ".." );
	
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
