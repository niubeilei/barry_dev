# File: makefile
#
# This is a makefile for use with gcc
#

TARGETEXE = test.exe

CC = g++

.SUFFIXES: .cpp .h

CFLAGS =  -g -Wall -DOMN_PLATFORM_UNIX 

LFLAGS = #-Wl,-v

OBJDIR = ../../obj/$(TARGETEXE)
LIBDIR = ../../obj

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

INCDIRS = -I.

LIBS = -lpthread


DLIBS = \
		$(LIBDIR)/libNativeAPI.a 
		
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all: $(TARGETEXE)

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(LIBS)
	

$(OBJECTS): test.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(CFLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE)
