# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = SQLite

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I/usr/include
#INCDIRS = -I../ -I/usr/local/mysql-standard-4.0.20-pc-linux-i686/include

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBS = -lnsl 

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
