
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

LIBNAME = BitmapTester

TARGETEXE = bitmaptorturer.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

CUST_LIBDIR = ../../../CustomizedLibs
LIBDIR = ../../../obj
LIBDIR2 = ~/Jimo_Servers0
EXLIBDIR = ../../../Lib

OBJDIR = ../../../obj/$(LIBNAME)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lstdc++ -lscws -lboost_system -lboost_date_time \
	   -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lboost_thread \
	   -L../../../Runtime_lib -L../../../obj
DLIBS = \
		$(LIBDIR)/libBitmapMgr.a \
		$(LIBDIR)/libDataRecord.a \
		$(LIBDIR)/libDataField.a \
		$(LIBDIR)/libAppIdGens.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libIILEntryMap.a \
		$(LIBDIR)/libDataColComp.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libConfigMgr.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libSysMsg.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(EXLIBDIR)/libsnappy.a \
		$(LIBDIR)/libTransMgr.a 

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -rdynamic -Wl,--no-as-needed $(FLAGS) -o $(LIBDIR2)/$(TARGETEXE) $^ $(DLIBS) $(LIBS) $(DLIBS) $(DLIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c -static  $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(LIBDIR2)/$(TARGETEXE) 
