# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = UdpTrafficGen.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -DOMN_PLATFORM_UNIX $(Optimize) -Wall $(KERNEL_VERSION) -DAOS_KERNEL_SIMULATE -DAOS_USERLAND

INCDIRS = -I../../../src -I/usr/include/mysql

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread


DLIBS = \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libNMS.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libNetwork.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libLogger.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libOmnUtil.a \
		-lz
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

rexe: 
	rm TcpTrafficGen.exe
	make

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
