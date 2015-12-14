#////////////////////////////////////////////////////////////////////////////
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

TARGETEXE = msgserver.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
	   -lboost_date_time  -lssl -lcrypto -lboost_regex

DLIBS = \
		$(LIBDIR)/libMsgServer.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libRemoteConvertData.a \
		$(LIBDIR)/libMediaData.a \
		$(LIBDIR)/libSchedule.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libDataGenUtil.a \
		$(LIBDIR)/libDataGenSeqno.a \
		$(LIBDIR)/libShortMsgClt.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libSearchEngineAdmin.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libDataCollector.a \
		$(LIBDIR)/libJob.a \
		$(LIBDIR)/libTaskMgr.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libTaskUtil.a \
		$(LIBDIR)/libNetworkMgrUtil.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libBatchQuery.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libIILTransServer.a \
		$(LIBDIR)/libDocTrans.a \
		$(LIBDIR)/libIDTransMap.a \
		$(LIBDIR)/libIILIDIdGen.a \
		$(LIBDIR)/libWordClient.a \
		$(LIBDIR)/libWordMgr.a \
		$(LIBDIR)/libQueryClient.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libDataGenUtil.a \
		$(LIBDIR)/libDataGenSeqno.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libCondsTorturer.a \
		$(LIBDIR)/libValueSelTorturer.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libTimeSelector.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libDocFileMgr.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocProc.a \
		$(LIBDIR)/libTimerMgr.a \
		$(LIBDIR)/libAosPython.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libSiteMgr.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libomni_parser.a \
		$(LIBDIR)/libaos.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libStatemachine.a \
		$(LIBDIR)/libSecUtil.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libBase64.a

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
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
