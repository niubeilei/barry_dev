# File: makefile
#
# This is a makefile for use with g++
#

TARGETEXE = keymantester

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../../include -I../../../ 

OBJDIR = ../../obj/$(TARGETEXE)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

LIBS = -lcommon -lpthread -lssl -lkeyman -L../../obj -L../../../../ThirdPartyLibs

#
# objects and targets
#

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE)

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp 
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 

