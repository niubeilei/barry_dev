# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = Torturer.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread


DLIBS = \
		$(LIBDIR)/libConditions.a \
		$(LIBDIR)/libTorturer.a \
		$(LIBDIR)/libRVG.a \
		$(LIBDIR)/libAction.a \
		$(LIBDIR)/libSemanticData.a \
		$(LIBDIR)/libSemantics.a \
		$(LIBDIR)/libProgAid.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libNMS.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libNetwork.a \
		$(LIBDIR)/libLogger.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libEvent.a \
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
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libaos_util.a \
		$(LIBDIR)/libXmlLib.a \
		-lz
#		/usr/local/mysql/lib/libmysqlclient.a 
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
