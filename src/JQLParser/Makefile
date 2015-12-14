# File: makefile
#
# This is a makefile for use with g++
#
LIBNAME = JQLParser

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I../../Include

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

GRAMMARSPATH = ./Grammars

LIBS = -lnsl 

#
# objects and targets
#

OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))
CCOBJECTS = $(patsubst %.cc,$(OBJDIR)/%.o,$(wildcard *.cc))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: bison

bison:
	make clean
	cat $(GRAMMARSPATH)/Head.yy  $(GRAMMARSPATH)/Mid.yy  $(GRAMMARSPATH)/Stmt*.yy $(GRAMMARSPATH)/Bottom.yy   > Parser.yy
	flex -o Parser.lex.cpp Parser.l
	bison -vd Parser.yy
#mv Parser.tab.cc Parser.tab.cpp
	make lib

lib: $(OBJECTS) $(CCOBJECTS)
	ar rvu $(TARGETLIB) $^
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(CCOBJECTS): $(OBJDIR)/%.o: %.cc
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@


clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
	rm -rf Parser.yy
	rm -rf Parser.lex.cpp Parser.tab.cc *.hh *.output
