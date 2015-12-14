
TARGETEXE = Tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include -I/home/chen.ding/CppUnit/cppunit-1.12.1/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lstdc++


DLIBS = \
		$(LIBDIR)/libaos_net.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libaos_util.a \
		-ldl \
		/home/chen.ding/CppUnit/cppunit-1.12.1/src/cppunit/.libs/libcppunit.so -lm  -Wl,--rpath -Wl,/usr/local/lib \
		-lz
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
