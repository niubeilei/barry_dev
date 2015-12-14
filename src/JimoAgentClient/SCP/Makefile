
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
#//
#////////////////////////////////////////////////////////////////////////////

TARGETEXE = jscp

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/scws/include -L/usr/local/scws/lib

LIBDIR = ../../../obj
LIBDAR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lcrypto -ldl -lz -lutil -lrt

DLIBS = \
		$(LIBDIR)/libJimoAgentClient.a \
		$(LIBDIR)/libJimoAgentPackage.a \
		$(LIBDIR)/libJimoAgentUtil.a \
		$(LIBDIR)/libXmlInterface.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libReqDistr.a \
		$(LIBDIR)/libProggieUtil.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaos_alarms.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSEModules.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libSEUtil.a \

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
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(TARGETEXE) 
