
#//
#// Copyright (C) 2005
#// Packet Engineering, Inc. All rights reserved.
#//
#// Redistribution and use in source and binary forms, with or without
#// modification is not permitted unless authorized in writing by a duly
#// appointed officer of Packet Engineering, Inc. or its derivatives
#//
#// Description:
#//
#// Modification History:
#// 12/4/2007: Created by Allen Xu
#//
#////////////////////////////////////////////////////////////////////////////

LIBNAME = Admin

TARGETEXE = admin.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

CUST_LIBDIR = ../../../CustomizedLibs
LIBDIR = ../../../obj
EXLIBDIR = ../../../Lib

OBJDIR = ../../../obj/$(LIBNAME)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lstdc++ -lscws \
	   -lboost_system -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lrt -lboost_thread \
	   -L../../../obj  -L../../../Runtime_lib -lStatisticsJimos

ARCH:=$(shell getconf LONG_BIT)
BIT32:=32
ECGLIB=ContecLib64.a

ifeq ($(ARCH),$(BIT32))
	ECGLIB=ContecLib32.a
endif



DLIBS = \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libHBaselib.a \
		$(LIBDIR)/libDataEngine.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libJimoAgentMonitor.a \
		$(LIBDIR)/libStoreQuery.a \
		$(LIBDIR)/libSystemMonitor.a \
		$(LIBDIR)/libMsgProc.a \
		$(LIBDIR)/libSEServer.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libDfmWrapper.a \
		$(LIBDIR)/libDfmSnapshot.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libAppVar.a \
		$(LIBDIR)/libHtmlLayout.a \
		$(LIBDIR)/libHtmlModules.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libHtmlUtil.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libShortMsgClt.a \
		$(LIBDIR)/libShortMsgSvr.a \
		$(LIBDIR)/libShortMsgUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libLogTrans.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSQLClient.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libMediaData.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSeReqProc.a \
		$(LIBDIR)/libSmartReply.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libLogUtil.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libBitmapMgr.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libAppIdGens.a \
		$(LIBDIR)/libCounterClt.a \
		$(LIBDIR)/libCounterServer.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libCompressUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocLock.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libBatchQuery.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libUtilData.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStatUtil.a \
		$(LIBDIR)/libVector2DUtil.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libIILTransServer.a \
		$(LIBDIR)/libIDTransMap.a \
		$(LIBDIR)/libDocTrans.a \
		$(LIBDIR)/libStreamFS.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libIILIDIdGen.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libWordClient.a \
		$(LIBDIR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libIILEntryMap.a \
		$(LIBDIR)/libGroupby.a \
		$(LIBDIR)/libIILScanner.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libIILUtil.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libTaskScheduler.a \
		$(LIBDIR)/libTaskUtil.a \
		$(LIBDIR)/libDataset.a \
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libDataScanner.a \
		$(LIBDIR)/libDataCacher.a \
		$(LIBDIR)/libDataSort.a \
		$(LIBDIR)/libSorter.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libDataBlob.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libMetaData.a \
		$(LIBDIR)/libNetFile.a \
		$(LIBDIR)/libNetworkMgrUtil.a \
		$(LIBDIR)/libRemoteBackupClt.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libTimeSelector.a \
		$(LIBDIR)/libDataGenSeqno.a \
		$(LIBDIR)/libDataGenUtil.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libHealthChecker.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libStatemachine.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libDataFormat.a \
		$(LIBDIR)/libDataProc.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libStatTrans.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libDataAssembler.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libDataCollector.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libAdminTrans.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libSysTrans.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libConfigMgr.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libMySQL.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libJQLParser.a \
		$(LIBDIR)/libJQLStatement.a \
		$(LIBDIR)/libJSON.a \
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libSQLDatabase.a \
		$(LIBDIR)/libAosConfStat.a \
		$(EXLIBDIR)/libsnappy.a\
		$(EXLIBDIR)/libmysqlclient.a\
		$(EXLIBDIR)/libcurl.a


#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -rdynamic $(FLAGS) -o $(LIBDIR)/$(TARGETEXE) $^ $(DLIBS) $(LIBS) $(DLIBS) -lncurses -lrt

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(LIBDIR)/$(TARGETEXE) 
