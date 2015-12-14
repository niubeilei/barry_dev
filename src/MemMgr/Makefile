# File: makefile
#
# This is a makefile for use with g++
#

TARGETEXE = MemMgr
LIBNAME = MemMgr

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -Wunused -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND -DAOS_KERNEL_SIMULATE
#FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I../rhcInclude

LIBDIR = ../../obj
OBJDIR = ../../obj/$(LIBNAME)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTUPPERDIR = /usr/local/rhc
OUTPUTDIR = /usr/local/rhc/bin
#OUTPUTDIR = /usr/local/rhc/bin

CREATETARGETDIR = $(shell if [ -d $(OUTPUTDIR) ]; then echo ""; else sudo mkdir $(OUTPUTUPPERDIR); sudo mkdir $(OUTPUTDIR); fi)

LIBS = -lnsl -lpthread -lcrypto -L../../rhcLib

DLIBS = \
		$(LIBDIR)/libTcpAppServer.a \
		$(LIBDIR)/libSecuredShell.a \
		$(LIBDIR)/libKeyman.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libUserlandWrapper.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/liblicense.a \
		$(LIBDIR)/libCommandMan.a \
		$(LIBDIR)/librhcUtil.a \
		-lcurses -lreadline \
		-lz -lrt
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

$(TARGETEXE): $(OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS) 
#	ln -s ../MemMgr/MemMgr ../SystemCli/MemMgr
#	sudo cp $(TARGETEXE) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp 
	$(CREATEOBJDIR)
#	$(CREATETARGETDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(TARGETEXE)
#	rm ../SystemCli/MemMgr
#	sudo rm -rf $(OUTPUTDIR)/$(TARGETEXE)
