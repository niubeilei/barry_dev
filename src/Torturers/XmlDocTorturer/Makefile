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

TARGETEXE = xml_doc_test.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

CUST_LIBDIR = ../../../CustomizedLibs

LIBDIR = ../../../obj
LIBDAR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)
INSDIR = ../../../install

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -ljconv -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws \
	   -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lutil -lthrift -lboost_thread

DLIBS = \
		$(LIBDIR)/libThriftSvrAPI.a \
		$(LIBDIR)/libJimoApi.a \
		$(LIBDIR)/libSvrProxyMgr.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libSvrProxyUtil.a \
		$(LIBDIR)/libTransBasic.a \
		$(LIBDIR)/libBitmapMgr.a \
		$(LIBDIR)/libAppIdGens.a \
		$(LIBDIR)/libReliableFile.a \
		$(LIBDIR)/libStorageMgr.a \
		$(LIBDIR)/libStorageMgrUtil.a \
		$(LIBDIR)/libTransClient.a \
		$(LIBDIR)/libTransServer.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libSysTrans.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libSystemMonitor.a \
		$(LIBDIR)/libSengTorUtil.a \
		$(LIBDIR)/libCompressUtil.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libDatabase.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libNetworkMgr.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libReplicMgr.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libUtilHash.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libConfigMgr.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libConds.a \
		/usr/lib/libmysqlclient.a \

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
