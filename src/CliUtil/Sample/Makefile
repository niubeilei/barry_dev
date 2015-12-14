# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = test

CC = g++

.SUFFIXES: .cpp .h

#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../.. -I../../rhcInclude 

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lcrypto -L../../rhcLib -lsqlite3 

DLIBS = \
		$(LIBDIR)/libCliUtil.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libSecuredShell.a \
		$(LIBDIR)/libKernelSimu.a \
		$(LIBDIR)/libUserlandWrapper.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libKernelUtil.a \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/liblicense.a \
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

