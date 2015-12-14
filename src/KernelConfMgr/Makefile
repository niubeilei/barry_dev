# File: makefile
#
# This is a makefile for use with g++
#

TARGETEXE = KernelConfMgr
#LIBNAME = SystemCli

CC = g++

.SUFFIXES: .cpp .h

#FLAGS = -Werror -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I../rhcInclude -I../export_lib/include

LIBDIR = ../../obj
OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir -p $(OBJDIR); fi)

OUTPUTDIR = ../../../bin
#OUTPUTDIR = ../../../bin

LIBS = -lpthread -lcrypto -L../../ThirdPartyLibs -lsqlite3 

DLIBS = \
		$(LIBDIR)/libCliUtil.a \
		$(LIBDIR)/libCliClient.a \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libSecuredShell.a \
		$(LIBDIR)/libKernelInterface.a \
		$(LIBDIR)/libKeyman.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libKernelSimu.a \
		$(LIBDIR)/libUserlandWrapper.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libKernelAPI.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libKernelUtil.a \
		$(LIBDIR)/liblicense.a \
		$(LIBDIR)/libCommandMan.a \
		$(LIBDIR)/libParentCtrl.a \
		$(LIBDIR)/librhcUtil.a \
		$(LIBDIR)/libTinyXml.a \
		$(LIBDIR)/libWanCM.a \
		-lcurses -lreadline \
		 -lrt
#
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

TARGETLIB = lib$(LIBNAME).a

#
# Rules for normal comile and link 
#
all:: $(TARGETEXE) 

$(TARGETEXE): $(OBJECTS) $(DLIBS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 

$(OBJECTS): $(OBJDIR)/%.o: %.cpp 
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE) 

