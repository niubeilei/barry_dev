
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

LIBNAME = Task

TARGETEXE = task.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include -I../../../SyncEngine

CUST_LIBDIR = ../../../CustomizedLibs
LIBDIR = ../../../obj
EXLIBDIR = ../../../Lib

OBJDIR = ../../../obj/$(LIBNAME)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lstdc++ -lscws -lboost_filesystem \
	   -lboost_system -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lrt \
	   -L../../../Runtime_lib -lDatasetJimos \
	   -lDataCubicJimos -lJimoDataProc -lSchemaJimos -lSchemaPickerJimos -lStreamEngine \
	   -lStatisticsJimos -L../../../obj\
	   -lQueryCacher -lQueryProc -lQueryEngine -lBitmapEngine -lQueryTermJimos -lDatasetConverters\
	   -lQueryInto	-lQueryFormat -lJimoProg -lJimoDataReceiver -lSyncEngineClient -lSyncEngine -lSynchers

ARCH:=$(shell getconf LONG_BIT)
BIT32:=32
ECGLIB=ContecLib64.a

ifeq ($(ARCH),$(BIT32))
	ECGLIB=ContecLib32.a
endif



DLIBS = \
		$(LIBDIR)/libMetaData.a \
		$(LIBDIR)/libJSON.a \
		$(LIBDIR)/libSequence.a \
		$(LIBDIR)/libGroupQuery.a \
		$(LIBDIR)/libVector2DQryRslt.a \
		$(LIBDIR)/libDataEngine.a \
		$(LIBDIR)/libStoreQuery.a \
		$(LIBDIR)/libSystemMonitor.a \
		$(LIBDIR)/libMsgProc.a \
		$(LIBDIR)/libSEServer.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libDfmWrapper.a \
		$(LIBDIR)/libDfmSnapshot.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libAppVar.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libLogTrans.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSQLClient.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libMediaData.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSmartReply.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libLogUtil.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libQueryClient.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libBitmapMgr.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libAppIdGens.a \
		$(LIBDIR)/libCounterClt.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libCompressUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libDocLock.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libDfmWrapper.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libUtilData.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libIILTransServer.a \
		$(LIBDIR)/libIDTransMap.a \
		$(LIBDIR)/libBitmapTrans.a \
		$(LIBDIR)/libDocTrans.a \
		$(LIBDIR)/libStreamFS.a \
		$(LIBDIR)/libIILIDIdGen.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libWordClient.a \
		$(LIBDIR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libRemoteConvertData.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libJimoAgentMonitor.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libAdminTrans.a \
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
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libDataScanner.a \
		$(LIBDIR)/libDataCacher.a \
		$(LIBDIR)/libDataSort.a \
		$(LIBDIR)/libSorter.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libDataBlob.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libDataset.a \
		$(LIBDIR)/libDataCube.a \
		$(LIBDIR)/libDataCubeReadDocUtil.a \
		$(LIBDIR)/libDataSplitter.a \
		$(LIBDIR)/libSchema.a \
		$(LIBDIR)/libNetFile.a \
		$(LIBDIR)/libNetworkMgrUtil.a \
		$(LIBDIR)/libRemoteBackupClt.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libTimeSelector.a \
		$(LIBDIR)/libDataGenSeqno.a \
		$(LIBDIR)/libDataGenUtil.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libHealthChecker.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libStatemachine.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libDataFormat.a \
		$(LIBDIR)/libDataProc.a \
		$(LIBDIR)/libStatTrans.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libStatEngine.a \
		$(LIBDIR)/libVector2D.a \
		$(LIBDIR)/libStatSvr.a \
		$(LIBDIR)/libVector2DUtil.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libDataAssembler.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libDataCollector.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libSysTrans.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libFuncs.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libJQLParser.a \
		$(LIBDIR)/libJQLStatement.a \
		$(LIBDIR)/libJimoLogic.a \
		$(LIBDIR)/libStatUtil.a \
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libMySQL.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libConfigMgr.a \
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
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libSQLDatabase.a \
		$(LIBDIR)/libAosConfStat.a \
		$(LIBDIR)/libAosConf.a \
		$(EXLIBDIR)/libsnappy.a \
		$(EXLIBDIR)/libmysqlclient.a \
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
	$(CC) -rdynamic -Wl,--no-as-needed $(FLAGS) -o $(LIBDIR)/$(TARGETEXE) $^ $(DLIBS) $(LIBS) $(DLIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(LIBDIR)/$(TARGETEXE) 
