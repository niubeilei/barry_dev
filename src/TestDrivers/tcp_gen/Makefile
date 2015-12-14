# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = traffic_gen.exe

CC = gcc

.SUFFIXES: .c .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(COptions)

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread


DLIBS = \
		$(LIBDIR)/libaos_net.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libomni_parser.a \
		$(LIBDIR)/libsemanobj.a \
		$(LIBDIR)/libsemantics.a \
		$(LIBDIR)/libsemanoprs.a \
		$(LIBDIR)/librvg.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libconditions.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libaos_util.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libporting.a \
		-lz
#		/usr/local/mysql/lib/libmysqlclient.a 
#
# objects and targets
#
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.c
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
