# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = rsync_torture.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -ldl -lboost_filesystem -lboost_system


DLIBS = $(LIBDIR)/libRsync.a 
#		/usr/local/mysql/lib/libmysqlclient.a 
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS) $(DLIBS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(DLIBS):
	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
	cd $$p;make all;cd -;\
	done
clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
distclean:
	cd ..;make distclean;make all>/dev/null;cd -
#	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
#	cd $$p;make clean;cd -;\
#	done                                                      
