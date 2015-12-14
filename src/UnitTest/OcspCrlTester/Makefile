# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = OcspCrlTester

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

LIBS = -lnsl -lpthread

DLIBS = \
		$(LIBDIR)/libKernelSimu.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libASN1.a \
		$(LIBDIR)/libCliSimuLib.a \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libKernelUtil.a \
		-lcurses -lreadline \
#		$(LIBDIR)/libHttpProc.a \
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
