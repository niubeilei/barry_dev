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
#// This file is automatically created by the TorturerGen facility. 
#//
#// Modification History:
#// 3/19/2007: Created by TorturerGen facility
#//
#////////////////////////////////////////////////////////////////////////////

TARGETEXE = TransTorturer.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include -I/usr/local/scws/include -L/usr/local/scws/lib 

LIBDIR = ../../../obj

THIRD_PARTY_LIBS = ../../../ThirdPartyLibs

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lscws -L/usr/local/scws/lib -lpthread -lstdc++ 


DLIBS = \
		$(LIBDIR)/libIdGen.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libIILClient.a \
		$(LIBDIR)/libSearchEngine.a \
		$(LIBDIR)/libIILMgr.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libWordClient.a \
		$(LIBDIR)/libWordMgr.a \
		$(LIBDIR)/libSEUtilServer.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libSiteMgr.a \
		$(LIBDIR)/libActions.a \
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
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libIILAgent.a \
		$(LIBDIR)/libTransPest.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libIILServer.a \
		$(LIBDIR)/libIILAgentTester.a \
		$(LIBDIR)/libTransUil.a
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
