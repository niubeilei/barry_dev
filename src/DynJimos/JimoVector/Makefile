# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = JimoVector

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

ROOTDIR = ../../..
INCDIRS = -I$(ROOTDIR)/src -I/usr/local/scws/include

OBJDIR = $(ROOTDIR)/obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = $(ROOTDIR)/obj

LIBS = -lnsl

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).so

#
# Rules for normal comile and link 
#
all:: lib

# $(CC) -shared -Wl,-soname,libctest.so.1 -o $(TARGETLIB) $(OBJECTS) 
lib: $(OBJECTS)
	$(CC) -shared -o $(TARGETLIB) $(OBJECTS) 
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c -fpic -shared -rdynamic $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
