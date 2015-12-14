# File: makefile
#
# This is a makefile for use with g++
#
TARGETEXE = createrawdata.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I/usr/local/mysql/include

LIBDIR = ../../../obj

OBJDIR = ../../../obj/$(TARGETEXE)

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -lboost_date_time


DLIBS = \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libSecUtil.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libQueryRslt.a \
		$(LIBDIR)/libQueryUtil.a \
		$(LIBDIR)/libDocClient.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libDataStore.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libNetwork.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libSecurity.a \
		$(LIBDIR)/libStatistics.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libMessage.a \
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
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libTransMgr.a 
#		/usr/local/mysql/lib/libmysqlclient.a 
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
