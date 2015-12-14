# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = Funcs

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

ROOTDIR=../..
INCDIRS = -I../ -I../../Include

OBJDIR = $(ROOTDIR)/obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = $(ROOTDIR)/obj

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws
DLIBS = \
		$(LIBDIR)/libSEUtil.a

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
