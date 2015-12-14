# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = AppMonitor.exe
OBJDIRName = AppMonitor

CC = g++

.SUFFIXES: .cpp .h

#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../../src -I/usr/local/mysql/include

LIBDIR = ../../obj

OBJDIR = ../../obj/$(OBJDIRName)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../bin

LIBS = -lnsl -lpthread


OBJ = 	$(OBJDIR)/AppMonitor.o

DLIBS = \
		$(LIBDIR)/libKernelSimu.a \
		$(LIBDIR)/libUserlandWrapper.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libAppMgr.a \
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
		$(LIBDIR)/libKernelUtil.a \
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
