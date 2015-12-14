# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = Tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -Wall -DAOS_USERLAND -DAOS_KERNEL_SIMULATE -DCONFIG_AOS_TEST

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/AosUtilTester_$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread

#		$(LIBDIR)/libMySQL.a 
#		/usr/local/mysql/lib/libmysqlclient.a 

#		$(LIBDIR)/libEvent.a 
#
DLIBS = \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libUserlandWrapper.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libKernelSimu.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libNMS.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libNetwork.a \
		$(LIBDIR)/libRetransMgr.a \
		$(LIBDIR)/libLogger.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libStatistics.a \
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
		$(LIBDIR)/libKernelUtil.a \
		$(LIBDIR)/libTransMgr.a \
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
