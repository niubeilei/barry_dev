# File: makefile
#
# This is a makefile for use with g++
#

TARGETEXE = qosapitester

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE

INCDIRS = -I../../include -I../../../ 

OBJDIR = ../../../../obj/$(TARGETEXE)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

LIBS = -lpthread -lqosapi -lcommon -L../../obj

#
# objects and targets
#

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:
	make exe

exe: $(TARGETEXE)

$(TARGETEXE): $(OBJECTS) 
	$(CC) -o $@ $^ $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE)
