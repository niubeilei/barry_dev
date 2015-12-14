# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = DataProcDyn
CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I/usr/local/scws/include -L/usr/local/scws/lib
OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).so

#
# Rules for normal comile and link 
#
all:: lib

lib:
	$(CC) -fpic -shared -rdynamic $(FLAGS) $(INCDIRS) Sample.cpp -o libDLLCondSample.so
	mv libDLLCondSample.so $(OUTPUTDIR)


#$(OBJECTS): $(OBJDIR)/%.o: %.cpp
#	$(CREATEDIR)
#	$(CC) -fpic -shared $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OUTPUTDIR)/libDLLCond*.so 
