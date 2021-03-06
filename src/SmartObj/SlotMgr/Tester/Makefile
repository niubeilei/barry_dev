
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

TARGETEXE = slotmgr_tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../../obj
LIBDAR = ../../../../obj

OBJDIR = ../../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../../bin

LIBS = -ljconv -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
	   -lboost_date_time  -lssl -lboost_regex 


DLIBS = \
		$(LIBDIR)/libSobjSlotMgr.a \
		$(LIBDIR)/libSmartObj.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libAppVar.a \
		$(LIBDIR)/libhtmlserver.a \
		$(LIBDIR)/libHtmlModules.a \
		$(LIBDIR)/libSearchEngineAdmin.a \
		$(LIBDIR)/libShortMsgClt.a \
		$(LIBDIR)/libShortMsgSvr.a \
		$(LIBDIR)/libShortMsgUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libSeLogUtil.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libSQLClient.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libSchedule.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libCounterTime.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSeReqProc.a \
		$(LIBDIR)/libSmartReply.a \
		$(LIBDIR)/libSeSiteMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libQueryClient.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libCounterServerClient.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libMicroblog.a \
		$(LIBDIR)/libInstantMsg.a \
		$(LIBDIR)/libMsgClient.a \
		$(LIBDAR)/libIILClient.a \
		$(LIBDAR)/libSearchEngine.a \
		$(LIBDAR)/libDocClient.a \
		$(LIBDAR)/libSeLogClient.a \
		$(LIBDAR)/libDocServer.a \
		$(LIBDAR)/libDocUtil.a \
		$(LIBDAR)/libIILMgr.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDAR)/libIILTransServer.a \
		$(LIBDAR)/libIDTransMap.a \
		$(LIBDAR)/libDocTrans.a \
		$(LIBDAR)/libIILIDIdGen.a \
		$(LIBDAR)/libSEModules.a \
		$(LIBDAR)/libWordClient.a \
		$(LIBDAR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSiteMgr.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libDataGenSeqno.a \
		$(LIBDIR)/libDataGenUtil.a \
		$(LIBDIR)/libHealthChecker.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libTaggedData.a \
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
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libDocProc.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libIILTrans.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocLock.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libUtilHash.a \
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
		$(LIBDIR)/libRandomUtil.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libOmnUtil.a 

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS) $(DLIBS) $(DLIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
