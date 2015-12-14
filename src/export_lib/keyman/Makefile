# File: makefile
#
# This is a makefile for use with g++
#

LIBNAME = keyman
CC = g++

.SUFFIXES: .cpp .h

FLAGS = -shared -fPIC -DOMN_IPV4 -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_KERNEL_SIMULATE -DAOS_USERLAND

INCDIRS = -I../../ -I../../rhcInclude -I../include

OBJDIR = ../../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../obj

LIBS = 

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).so

#
# Rules for normal comile and link 
#
all:
ifeq ($(shell if [ -f links ]; then echo ""; else echo "n"; fi), n)
	@perl create_link -a || exit
	@touch links
endif
	@make lib

lib: $(OBJECTS)
	$(CC) $(FLAGS) $(LIBS) -o $(TARGETLIB) $(OBJECTS)
ifeq ($(shell if [ -d $(OUTPUTDIR) ]; then echo ""; else echo "n"; fi), n)
	@rm -rf ../obj
	@mkdir ../obj
endif
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	@rm -rf links $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
	@perl create_link -d

