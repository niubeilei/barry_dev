# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = ido_tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -L../../../obj -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lboost_regex -lMagick++ -lcrypto -ldl


DLIBS = \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libIDO.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libUtilData.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libporting.a 
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
	$(CC) -rdynamic -o $@ $^ -Wl,--whole-archive $(DLIBS) -Wl,--no-whole-archive $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
