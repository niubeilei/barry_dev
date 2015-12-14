# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = ExternTool

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

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all: lib myping gendata fperf

myping: $(OBJDIR)/ping.o $(OBJDIR)/CPing.o 
	$(CC) $(LDFLAGS) $(CFLAGS) -o myping $^

gendata: $(OBJDIR)/dataGenRandom.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o gendata $^

fperf: $(OBJDIR)/fperf.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o fperf $^

lib: $(OBJDIR)/CPing.o
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
	rm -rf *.o
	rm -rf myping
	rm -rf fperf
	rm -rf gendata 
	rm -rf $(all)
