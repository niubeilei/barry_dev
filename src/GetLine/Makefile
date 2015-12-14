LIBNAME = getline

AR=				/usr/bin/ar
CC=				/usr/bin/cc
INSTALL=		/usr/bin/install
LN=				/bin/ln

CFLAGS=			-DPOSIX -O   -g
LDFLAGS=

OBJDIR = ../../obj/$(LIBNAME)
OUTPUTDIR = ../../obj

#
# objects and targets
#
OBJECTS = *.o
TARGETLIB = lib$(LIBNAME).a

all: $(TARGETLIB) testgl

# try this after installing to check the installed static library
testgl: $(TARGETLIB) testgl.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o testgl testgl.o $(OUTPUTDIR)/$(TARGETLIB) -lncurses

testgl.o: testgl.c getline.h

$(TARGETLIB): getline.o
	$(AR) cr libgetline.a getline.o
	mv $(TARGETLIB) $(OUTPUTDIR)

getline.o: getline.c getline.h

clean:
	rm -rf testgl $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 

install:
	

