# File: Makefile
#
# This is a makefile for use with g++
#
TARGETEXE = sf-tester

CC = gcc

.SUFFIXES: .c .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_USERLAND -DAOS_KERNEL_SIMULATE -DCONFIG_AOS_TEST

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/Random_$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread 

#		$(LIBDIR)/libMySQL.a 
#		/usr/local/mysql/lib/libmysqlclient.a 

#		$(LIBDIR)/libEvent.a 
#
DLIBS = $(LIBDIR)/libSystemFacility.a 
#
# objects and targets
#
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c))

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
