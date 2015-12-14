# File: makefile
#
# This is a makefile for use with g++
#
DECODING_TOOL = decoding_tool.exe
RAWFID_TOOL = rawfid_tool.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall  -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DSANITY_CHECK
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

LIBDIR = ../../../obj

CUSTOM_LIBDIR = ../../../Lib

SHARED_LIBDIR = ../../../obj

OBJDIR = ../../../obj

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

LIBS = -L../../../Runtime_lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -ldl -L$(SHARED_LIBDIR) -lBlobSE -lRawSE -lJimoRaft -lStatisticsJimos -L$(CUSTOM_LIBDIR) -lboost_filesystem -lboost_system


DLIBS = \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libBitmapMgr.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libTransMgr.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libUtilData.a\
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libaos_thread.a \
		$(CUSTOM_LIBDIR)/libsnappy.a
#		/usr/local/mysql/lib/libmysqlclient.a
#
# objects and targets
#
DECODING_TOOL_FILES = DecodingTool.cpp
DECODING_TOOL_OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(DECODING_TOOL_FILES))

RAWFID_TOOL_FILES = RawfidTool.cpp
RAWFID_TOOL_OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(RAWFID_TOOL_FILES))

#
# Rules for normal comile and link
#
all: $(DECODING_TOOL) $(RAWFID_TOOL)

$(DECODING_TOOL): $(DECODING_TOOL_OBJECTS) $(DLIBS)
	$(CC) -o $@ $^ -Xlinker --start-group $(DLIBS) $(LIBS) -Xlinker --end-group

$(RAWFID_TOOL): $(RAWFID_TOOL_OBJECTS) $(DLIBS)
	$(CC) -o $@ $^ -Xlinker --start-group $(DLIBS) $(LIBS) -Xlinker --end-group

$(DECODING_TOOL_OBJECTS): $(DECODING_TOOL_FILES)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(RAWFID_TOOL_OBJECTS): $(RAWFID_TOOL_FILES)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(DLIBS):
	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
	cd $$p&&make all;cd -;\
	done
clean:
	rm -rf $(OBJECTS) $(DECODING_TOOL)
distclean:
	make clean;\
	cd ..;make distclean;cd -
	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
	cd $$p&&make clean;cd -;\
	done
