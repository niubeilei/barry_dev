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
#// 7/18/2007: Created by TorturerGen facility
#//
#////////////////////////////////////////////////////////////////////////////

TARGETEXE = Torturer.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../../src -I/usr/local/mysql/include -I/cpe/aos/include

LIBDIR = ../../../../obj

LIBLOC = -L/cpe/aos/lib

OBJDIR = ../../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../../bin

LIBS = -lnsl -lpthread


DLIBS = \
		        -lqosapi -lrhcutil -lxml2 -laosxml \
        $(LIBDIR)/libCliTorturer.a \
		$(LIBDIR)/libTorturerGen.a \
		$(LIBDIR)/libTorturer.a \
		$(LIBDIR)/libTorturerConds.a \
		$(LIBDIR)/libTorturerUtil.a \
		$(LIBDIR)/libProgAid.a \
		$(LIBDIR)/libCliUtil.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libOmnUtil.a \
		-lz
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBLOC) $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(TARGETEXE) 
	rm -rf $(OBJDIR)/*
