
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

LIBNAME = NativeAPIEXE

TARGETEXE = zykie.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

CUST_LIBDIR = ../../../CustomizedLibs
LIBDIR = ../../../obj
EXLIBDIR = ../../../Lib

OBJDIR = ../../../obj/$(LIBNAME)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lnsl -lpthread -lstdc++ -lscws -lboost_thread \
	   -lboost_system -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lrt \

ARCH:=$(shell getconf LONG_BIT)
BIT32:=32
ECGLIB=ContecLib64.a

ifeq ($(ARCH),$(BIT32))
	ECGLIB=ContecLib32.a
endif



DLIBS = \
		$(LIBDIR)/libnative_api.a \
		$(EXLIBDIR)/libsnappy.a \
		$(EXLIBDIR)/libmysqlclient.a \
		$(EXLIBDIR)/libcurl.a

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS)
	$(CC) -rdynamic -Wl,--no-as-needed $(FLAGS) -o $(LIBDIR)/$(TARGETEXE) $^ $(DLIBS) $(LIBS) $(DLIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(LIBDIR)/$(TARGETEXE) 
