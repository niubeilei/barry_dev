# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = raftserver.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall  -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

LIBDIR = ../../../lib
CUST_LIBDIR = ../../../CustomizedLibs

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

#LIBS = -L/usr/local/scws/lib -L../../../shared_libs -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lboost_regex -lMagick++ -lcrypto -ldl -lboost_regex -lboost_date_time -L../../../shared_libs -lJimoRaft -lClusterMgr
LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -ldl -lboost_regex -lboost_date_time -lboost_system -lboost_filesystem -L../../../shared_libs -lJimoRaft -lBlobSE -lRawSE


DLIBS = \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libClusterMgr.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libAosPython.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libNetFile.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSEServer.a \
		$(LIBDIR)/libCompressUtil.a \
		$(LIBDIR)/libDfmSnapshot.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libAppIdGens.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libXmlLib.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libIDO.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libUtilData.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libIILScanner.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libporting.a \
		$(CUST_LIBDIR)/libICTCLAS50.a
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
	$(CC) -rdynamic -Wl,--no-as-needed -o $@ $^ $(DLIBS) $(LIBS) $(DLIBS) $(DLIBS)
#$(CC) -rdynamic -o $@ $^ -Wl,--whole-archive $(DLIBS) -Wl,--no-whole-archive $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
