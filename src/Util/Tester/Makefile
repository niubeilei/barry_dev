# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = utiltester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include -I../../../Include

EXLIBDIR = ../../../Lib
LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L ../../../Runtime_lib -lboost_system -lboost_date_time  -lssl -lboost_regex -lboost_thread -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lcrypto -ldl -lboost_filesystem 


DLIBS = \
		$(LIBDIR)/libSequence.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libClusterMgr.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libGroupQuery.a \
		$(LIBDIR)/libVector2DQryRslt.a \
		$(LIBDIR)/libJSON.a \
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
		$(LIBDIR)/libHtmlModules.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libHtmlUtil.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libShortMsgClt.a \
		$(LIBDIR)/libShortMsgUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libLogTrans.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSQLClient.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libMediaData.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSeReqProc.a \
		$(LIBDIR)/libJQLParser.a \
		$(LIBDIR)/libMsgService.a \
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
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libMicroblog.a \
		$(LIBDIR)/libInstantMsg.a \
		$(LIBDIR)/libMsgClient.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libCompressUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libDocLock.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libJQLParser.a \
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libUtilData.a \
		$(LIBDIR)/libStatEngine.a \
		$(LIBDIR)/libStatUtil.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libIILTransServer.a \
		$(LIBDIR)/libBitmapTrans.a \
		$(LIBDIR)/libIDTransMap.a \
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
		$(LIBDIR)/libAdminTrans.a \
		$(LIBDIR)/libFuncs.a \
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
		$(LIBDIR)/libTaskScheduler.a \
		$(LIBDIR)/libDataCollector.a \
		$(LIBDIR)/libTaskUtil.a \
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libDataScanner.a \
		$(LIBDIR)/libDataCacher.a \
		$(LIBDIR)/libDataSort.a \
		$(LIBDIR)/libSorter.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libDataBlob.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libMetaData.a \
		$(LIBDIR)/libDataset.a \
		$(LIBDIR)/libDataCube.a \
		$(LIBDIR)/libStatTrans.a \
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
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libStatemachine.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libDataFormat.a \
		$(LIBDIR)/libDataProc.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libVector2D.a \
		$(LIBDIR)/libVector2DUtil.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libDataAssembler.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libQueryFuncs.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libJimoCallServer.a \
		$(LIBDIR)/libJimoCall.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libXmlLib.a \
		$(LIBDIR)/libBSON.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libSysTrans.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libDocClientWrapper.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libMySQL.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libConfigMgr.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libCubeComm.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libJimoAPI.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libSQLDatabase.a \
		$(LIBDIR)/libAosConf.a \
		$(LIBDIR)/libJQLStatement.a \
		$(LIBDIR)/libJimoLogic.a \
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
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 
