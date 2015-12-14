# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = rawse_tester.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall  -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../ -I../../../Include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -ldl -lboost_filesystem -lboost_system


DLIBS = \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libDLLMgr.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libConfig.a \
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
		$(LIBDIR)/libaos_random.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libTransMgr.a \
		$(LIBDIR)/libTransUtil.a \
		$(LIBDIR)/libRawSE.a \
#		/usr/local/mysql/lib/libmysqlclient.a
#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))

#
# Rules for normal comile and link
#
all:: $(TARGETEXE)

$(TARGETEXE): $(OBJECTS) $(DLIBS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEOBJDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

$(DLIBS):
	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
	cd $$p&&make all;cd -;\
	done
clean:
	rm -rf $(OBJECTS) $(TARGETEXE)
distclean:
	make clean;\
	cd ..;make distclean;cd -
	for p in $(patsubst $(LIBDIR)/lib%.a, ../../%/,$(DLIBS));do \
	cd $$p&&make clean;cd -;\
	done
