# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = JimoAPI

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall  -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

ROOT = ../..
INCDIRS = -I../ -I../../Include

OBJDIR = $(ROOT)/obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = $(ROOT)/obj

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws
DLIBS = \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libJimoCall.a

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
