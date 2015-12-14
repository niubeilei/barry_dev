# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = ReadLine

CC = gcc

.SUFFIXES: .cpp .h .c

#FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND
FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_USERLAND  -D__USE_FILE_OFFSET64

INCDIRS = -I../ -I/usr/local/scws/include -L/usr/local/scws/lib

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

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
