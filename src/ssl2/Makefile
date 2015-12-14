# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = SSL2

KERNELHOME = /usr/src/linux

AOSCORE_DIR = $(KERNELHOME)/net/AosCore

CC = g++

.SUFFIXES: .c .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall $(KERNEL_VERSION) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE -DCHEN_TEST

INCDIRS = -I../

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = (if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBS = -lnsl 

#
# objects and targets
#
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 


