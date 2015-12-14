# File: makefile
#
# This is a makefile for use with g++
#

TARGETEXE = Tester 

CC = gcc

.SUFFIXES: .c .h

FLAGS = -Wunused -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND 
#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I../rhcInclude

LIBDIR = ../../../../obj
OBJDIR = ../../../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin
#OUTPUTDIR = ../../../bin

LIBS = 

DLIBS =	$(LIBDIR)/libnewRhcApi.a 
#
#
# objects and targets
#
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.c 
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
