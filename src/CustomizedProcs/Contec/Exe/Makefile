# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = ecg.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)


LIBDIR = ../../../../obj

OBJDIR = ../../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../../bin

ARCH:=$(shell getconf LONG_BIT)
BIT32:=32
ECGLIB=ContecLib64.a

ifeq ($(ARCH),$(BIT32))
	ECGLIB=ContecLib32.a
endif

CUST_LIBDIR = ../../../../CustomizedLibs
DLIBS = \
		$(CUST_LIBDIR)/$(ECGLIB)\

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
