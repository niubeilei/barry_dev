# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = Cli.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../../src -I../rhcInclude

LIBDIR = ../../obj

OBJDIR = ../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../bin

LIBS =  -lpthread -lcrypto -L../../ThirdPartyLibs

DLIBS = \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libSecuredShell.a \
		$(LIBDIR)/libKeyman.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libKernelUtil.a \
		$(LIBDIR)/libCommandMan.a \
		-lcurses -lreadline \
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

$(OBJECTS): $(OBJDIR)/%.o: %.cpp version.h 
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
