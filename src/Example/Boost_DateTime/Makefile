# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = Example.exe

CC = g++

.SUFFIXES: .cpp .h

#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
FLAGS = -g 

INCDIRS = -I../../../src 

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lboost_date_time


DLIBS = \
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp 
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
