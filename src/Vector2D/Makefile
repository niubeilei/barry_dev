# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = Vector2D

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I../../Include

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBS = -lnsl 

#
# objects and targets
#
EXCLUDE_OBJS = $(OBJDIR)/VectorFileFactory.o $(OBJDIR)/ValueFile.o
EXCLUDE_CPPS = VectorFileFactory.cpp
COMPILED_CPPS = $(filter-out $(EXCLUDE_CPPS),$(wildcard *.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(COMPILED_CPPS))



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
