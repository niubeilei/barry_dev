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
#// 08/21/2011	Created by Chen Ding
#////////////////////////////////////////////////////////////////////////////

TARGETEXE = gics_tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6  -lMagick++ -lboost_date_time -lboost_regex -lssl -ljconv


DLIBS = \
		$(LIBDIR)/libhtmlserver.a \
		$(LIBDIR)/libgics.a \
		$(LIBDIR)/libHtmlLayout.a \
		$(LIBDIR)/libHtmlModules.a \
		$(LIBDIR)/libDictionary.a \
		$(LIBDIR)/libHtmlUtil.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libSearchEngineAdmin.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libCounterServerClient.a \
		$(LIBDIR)/libSiteMgr.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libCounterServerClient.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libTaggedData.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libWordParser.a \
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
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libIILIDIdGen.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libWordTransClient.a \
		$(LIBDIR)/libWordClient.a \
		$(LIBDIR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSeLogSvr.a \
		$(LIBDIR)/libDocMgr.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocidIdGen.a \
		$(LIBDIR)/libDocProc.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libSecUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTransUil.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libShortMsgClt.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libEventCond.a \
		$(LIBDIR)/libQueryClient.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libRundata.a \
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
