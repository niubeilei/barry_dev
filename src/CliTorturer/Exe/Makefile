# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = CliTorturer.exe

CC = g++

.SUFFIXES: .cpp .h

#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../../../src -I../rhcInclude

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lcrypto -L../rhcLib -lsqlite3 

DLIBS = \
		$(LIBDIR)/libCliTorturer.a \
		$(LIBDIR)/libLogicExpr.a \
		$(LIBDIR)/libCliUtil.a \
		$(LIBDIR)/libTorturerGen.a \
		$(LIBDIR)/libProgAid.a \
		$(LIBDIR)/libRVG.a \
		$(LIBDIR)/libTorturerConds.a \
		$(LIBDIR)/libTorturerUtil.a \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libKernelUtil.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libUtil1.a \
		-lcurses -lreadline \
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
