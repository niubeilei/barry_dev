# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = http_proc

TARGETEXE = http_proc.exe

KERNELHOME = /usr/src/linux

AOSCORE_DIR = $(KERNELHOME)/net/AosCore

CC = g++

.SUFFIXES: .c .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall $(KERNEL_VERSION) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE

INCDIRS = -I../../

LIBDIR = ../../../lib

OBJDIR = ../../../obj/$(LIBNAME)

CREATEDIR = (if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../lib

LIBS = -lnsl -lpthread 

#
# objects and targets
#
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: lib

lib: $(OBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

exe: $(TARGETEXE)

DLIBS = \
        $(LIBDIR)/libKernelSimu.a \
        $(LIBDIR)/libUtil1.a \
        $(LIBDIR)/libAppMgr.a \
        $(LIBDIR)/libDataStore.a \
        $(LIBDIR)/libSingleton.a \
        $(LIBDIR)/libThread.a \
        $(LIBDIR)/libUtilComm.a \
        $(LIBDIR)/libTracer.a \
        $(LIBDIR)/libXmlParser.a \
        $(LIBDIR)/libObj.a \
        $(LIBDIR)/libConfig.a \
        $(LIBDIR)/libDebug.a \
        $(LIBDIR)/libAlarm.a \
        $(LIBDIR)/libPorting.a \
        $(LIBDIR)/libOmnUtil.a \
        $(LIBDIR)/libUtil1.a \
	$(LIBDIR)/libhttp_proc.a \
        -lz

$(TARGETEXE): $(OBJDIR)/main.o
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

$(OBJDIR)/main.o: main.cpp
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
#	$(CC) version
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 

