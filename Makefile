CC	=gcc -march=native -std=gnu99
CFLAGS	=-pipe -Os
LDFLAGS	=
LDLIBS	=-lX11

all::	blacken-cmap

clean::
	${RM} *.o core.* lint tags

distclean clobber:: clean
	${RM} blacken-cmap
