# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = AppProxy

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_USERLAND -DAOS_KERNEL_SIMULATE $(KERNEL_VERSION)

INCDIRS = -I../

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
 
