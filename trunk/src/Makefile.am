bin_PROGRAMS = transd

transd_SOURCES = transd.c slist.c configfile.c config_lex.l config_yacc.y
transd_LDADD = -L/usr/X11R6/lib -lX11 -lXmu

noinst_HEADERS = slist.h configfile.h

AM_YFLAGS = -d

INCLUDES = -I/usr/X11R6/include

