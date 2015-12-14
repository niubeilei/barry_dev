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

TARGETEXE = iiltester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../obj
LIBDAR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -ljconv -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
	   -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lthrift -lboost_thread -lBitmapEngine -L../../../obj

DLIBS = \
		$(LIBDIR)/libThriftSvrAPI.a \
		$(LIBDIR)/libAggrFuncs.a \
		$(LIBDIR)/libStatQryRslt.a \
		$(LIBDIR)/libVector2DQryRslt.a \
		$(LIBDIR)/libDataEngine.a \
		$(LIBDIR)/libHBaselib.a \
		$(LIBDIR)/libJimoApi.a \
		$(LIBDIR)/libStoreQuery.a \
		$(LIBDIR)/libSystemMonitor.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDAR)/libDfmSnapshot.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libAppVar.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libLogTrans.a \
		$(LIBDIR)/libStatTrans.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSQLClient.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libMediaData.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
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
		$(LIBDIR)/libCounterServer.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDAR)/libIILClient.a \
		$(LIBDAR)/libSearchEngine.a \
		$(LIBDAR)/libDocClient.a \
		$(LIBDAR)/libCompressUtil.a \
		$(LIBDAR)/libSeLogClient.a \
		$(LIBDAR)/libDocServer.a \
		$(LIBDAR)/libDocLock.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDAR)/libJimo.a \
		$(LIBDAR)/libDocUtil.a \
		$(LIBDAR)/libIILMgr.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStatSvr.a \
		$(LIBDIR)/libStatUtil.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDAR)/libIILTransServer.a \
		$(LIBDAR)/libIDTransMap.a \
		$(LIBDAR)/libBitmapTrans.a \
		$(LIBDAR)/libDocTrans.a \
		$(LIBDAR)/libIILIDIdGen.a \
		$(LIBDAR)/libSEModules.a \
		$(LIBDAR)/libWordClient.a \
		$(LIBDAR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libRemoteConvertData.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libIILEntryMap.a \
		$(LIBDIR)/libGroupby.a \
		$(LIBDIR)/libIILScanner.a \
		$(LIBDIR)/libImportData.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libIILUtil.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libTaskUtil.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libDataset.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libDataAssembler.a \
		$(LIBDIR)/libIILAssembler.a \
		$(LIBDIR)/libDataSort.a \
		$(LIBDIR)/libSorter.a \
		$(LIBDIR)/libNetFile.a \
		$(LIBDIR)/libNetworkMgrUtil.a \
		$(LIBDIR)/libRemoteBackupClt.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libHealthChecker.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libAccesses.a \
		$(LIBDIR)/libStatemachine.a \
		$(LIBDIR)/libAosPython.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libVector2D.a \
		$(LIBDIR)/libVector2DUtil.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libDocProc.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libSysTrans.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
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
		$(LIBDIR)/libJimoProg.a \
		$(LIBDIR)/libJimoLogic.a \
		$(LIBDIR)/libAccessServer.a \
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libSQLDatabase.a \
		$(LIBDIR)/libAosConfStat.a \
		$(LIBDIR)/libAosConf.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libTaskScheduler.a \
		/usr/lib/libmysqlclient.a
#		./lzma.a \
#		./Unpack8000w.a
#		$(CUST_LIBDIR)/libICTCLAS50.a\

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -rdynamic -Wl,--no-as-needed -o $@ $^ $(DLIBS) $(LIBS) $(DLIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c -static  $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
