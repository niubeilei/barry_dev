# File: makefile
#
# This is a makefile for use with g++
#

LIBNAME = webwall
CC = g++

FLAGS = -shared -fPIC -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_KERNEL_SIMULATE -DAOS_USERLAND

INCDIRS = -I../include -I../../

OBJDIR = ../../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../obj

#LIBS = -WI -Bstatic -lcommon -L../obj
LIBS = -lcommon -L../obj

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).so

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	$(CC) $(FLAGS) $(LIBS) -o $(TARGETLIB) $<
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 

