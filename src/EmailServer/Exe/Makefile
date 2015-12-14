
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

TARGETEXE = email.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../obj
LIBDAR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
	   -lboost_date_time  -lssl


DLIBS = \
		$(LIBDIR)/libEmailServer.a \
		$(LIBDIR)/libSearchEngineAdmin.a \
		$(LIBDIR)/libConds.a \
		$(LIBDIR)/libValueSel.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libDocServer.a \
		$(LIBDIR)/libDocProc.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libSeLogClient.a \
		$(LIBDIR)/libWordTransClient.a \
		$(LIBDIR)/libVersionServer.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libSEClient.a \
		$(LIBDIR)/libSmartDoc.a \
		$(LIBDIR)/libDbQuery.a \
		$(LIBDIR)/libQuery.a \
		$(LIBDIR)/libIILSelector.a \
		$(LIBDIR)/libQueryCond.a \
		$(LIBDIR)/libQueryClient.a \
		$(LIBDIR)/libCounterServerClient.a \
		$(LIBDIR)/libCounterUtil.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libDocSelector.a \
		$(LIBDIR)/libEventMgr.a \
		$(LIBDIR)/libSeReqProc.a \
		$(LIBDIR)/libSeSiteMgr.a \
		$(LIBDIR)/libEventCond.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libTransUil.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libMicroblog.a \
		$(LIBDAR)/libIILClient.a \
		$(LIBDAR)/libSearchEngine.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDAR)/libIILMgr.a \
		$(LIBDAR)/libSEModules.a \
		$(LIBDAR)/libWordClient.a \
		$(LIBDAR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSiteMgr.a \
		$(LIBDIR)/libActions.a \
		$(LIBDIR)/libEmailClt.a \
		$(LIBDIR)/libShortMsgClt.a \
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
		$(LIBDIR)/libKernelUtil.a \
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
		$(LIBDIR)/libSeSysLog.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransUil.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libSecUtil.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libUtilTime.a \
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
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
