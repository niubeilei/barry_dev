
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

TARGETEXE = localserver.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

CUST_LIBDIR = ../../../CustomizedLibs
LIBDIR = ../../../obj
LIBDAR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -ljconv -lsybdb -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
	   -lboost_date_time  -lssl -lboost_regex -lgsoap++ -lcrypto -ldl -lz -lutil -lthrift

ARCH:=$(shell getconf LONG_BIT)
BIT32:=32
ECGLIB=ContecLib64.a

ifeq ($(ARCH),$(BIT32))
	ECGLIB=ContecLib32.a
endif



DLIBS = \
		$(LIBDIR)/libFmtMgr.a \
		$(LIBDIR)/libStoreQuery.a \
		$(LIBDIR)/libDataEngine.a \
		$(LIBDIR)/libINotifyMgr.a \
		$(LIBDIR)/libSystemMonitor.a \
		$(LIBDIR)/libMsgProc.a \
		$(LIBDIR)/libSEServer.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libDfmUtil.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libAppVar.a \
		$(LIBDIR)/libHtmlLayout.a \
		$(LIBDIR)/libHtmlModules.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libHtmlUtil.a \
		$(LIBDIR)/libSearchEngineAdmin.a \
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
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSeReqProc.a \
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
		$(LIBDIR)/libCounterServer.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libMicroblog.a \
		$(LIBDIR)/libInstantMsg.a \
		$(LIBDIR)/libMsgClient.a \
		$(LIBDAR)/libIILClient.a \
		$(LIBDAR)/libSearchEngine.a \
		$(LIBDAR)/libDocClient.a \
		$(LIBDAR)/libCompressUtil.a \
		$(LIBDAR)/libSeLogClient.a \
		$(LIBDAR)/libDocServer.a \
		$(LIBDAR)/libMsgServer.a \
		$(LIBDAR)/libDocLock.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDAR)/libDocUtil.a \
		$(LIBDAR)/libIILMgr.a \
		$(LIBDAR)/libSnapshot.a \
		$(LIBDIR)/libBatchQuery.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDAR)/libIILTransServer.a \
		$(LIBDAR)/libIDTransMap.a \
		$(LIBDAR)/libDocTrans.a \
		$(LIBDAR)/libJimo.a \
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
		$(LIBDIR)/libSyBaseMgr.a \
		$(LIBDIR)/libDataJoin.a \
		$(LIBDIR)/libIILEntryMap.a \
		$(LIBDIR)/libGroupby.a \
		$(LIBDIR)/libIILScanner.a \
		$(LIBDIR)/libImportData.a \
		$(LIBDIR)/libIILAssembler.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libIILUtil.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libTaskScheduler.a \
		$(LIBDIR)/libTaskUtil.a \
		$(LIBDIR)/libJobTrans.a \
		$(LIBDIR)/libDataScanner.a \
		$(LIBDIR)/libVirtualFile.a \
		$(LIBDIR)/libDataCacher.a \
		$(LIBDIR)/libDataSort.a \
		$(LIBDIR)/libSorter.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libDataBlob.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libDataset.a \
		$(LIBDIR)/libVirtualFile.a \
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
		$(LIBDIR)/libTester.a \
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
		$(LIBDIR)/libAosPython.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSQLDatabase.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libStorageEngine.a \
		$(LIBDIR)/libDataFormat.a \
		$(LIBDIR)/libDataProc.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libThriftServer.a \
		$(LIBDIR)/libThriftSvrAPI.a \
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
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libJQLParser.a \
		$(LIBDIR)/libJQLStatement.a \
		$(LIBDIR)/libMySqlAdapter.a \
		$(LIBDIR)/libJQLExpr.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libValue.a \
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
		/usr/lib/libmysqlclient.a \
		$(CUST_LIBDIR)/libcurl.a \
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
	$(CC) -rdynamic -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
