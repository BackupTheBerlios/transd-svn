bin_PROGRAMS = transd

transd_SOURCES = transd.c slist.c configfile.c config_yacc.y config_lex.l
transd_LDADD = -L/usr/X11R6/lib -lX11 -lXmu

noinst_HEADERS = slist.h configfile.h config_yacc.h

AM_YFLAGS = -d

INCLUDES = -I/usr/X11R6/include

