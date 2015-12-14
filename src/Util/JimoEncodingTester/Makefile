# File: makefile
#
# This is a makefile for use with g++
#
JESIA_TOOL = JESIATorturer.exe
JEUIA_TOOL = JEUIATorturer.exe

JESIA_TOOL_BATCH = JESIATorturer_batch.exe
JEUIA_TOOL_BATCH = JEUIATorturer_batch.exe

CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -DOMN_PLATFORM_UNIX $(Optimize)
#FLAGS = -O3 -DOMN_PLATFORM_UNIX $(Optimize)

INCDIRS = -I../../../src -I.. -I/usr/local/mysql/include

CUSDIR = ../../../Lib

LIBDIR = ../../../obj

OBJDIR = ../../../obj/

CREATEOBJDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../../bin

LIBS = -lboost_system -lboost_date_time  -lssl -lboost_regex -lboost_thread -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ -lcrypto -ldl -lthrift


DLIBS = \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libUtilTime.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libTester.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libJimo.a \
		$(LIBDIR)/libAosApi.a \
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
		$(CUSDIR)/libsnappy.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libDataTypes.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libTransMgr.a \
		$(LIBDIR)/libporting.a \
		$(LIBDIR)/libaos_thread.a \
		$(LIBDIR)/libTransUtil.a  
#		/usr/local/mysql/lib/libmysqlclient.a 
#
# objects and targets
#
JESIA_TOOL_FILES = JESIATester.cpp JESIATorturer.cpp
JESIA_TOOL_OBJECTS = $(JESIA_TOOL_FILES:.cpp=.o)

JESIA_TOOL_BATCH_FILES = JESIATester_batch.cpp JESIATorturer_batch.cpp
JESIA_TOOL_BATCH_OBJECTS = $(JESIA_TOOL_BATCH_FILES:.cpp=.o)

JEUIA_TOOL_FILES = JEUIATester.cpp JEUIATorturer.cpp
JEUIA_TOOL_OBJECTS = $(JEUIA_TOOL_FILES:.cpp=.o)

JEUIA_TOOL_BATCH_FILES = JEUIATester_batch.cpp JEUIATorturer_batch.cpp
JEUIA_TOOL_BATCH_OBJECTS = $(JEUIA_TOOL_BATCH_FILES:.cpp=.o)


#
# Rules for normal compile and link 
#
all:: $(JESIA_TOOL) $(JEUIA_TOOL) $(JESIA_TOOL_BATCH) $(JEUIA_TOOL_BATCH)  

$(JESIA_TOOL): $(JESIA_TOOL_OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

$(JEUIA_TOOL): $(JEUIA_TOOL_OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

$(JESIA_TOOL_BATCH): $(JESIA_TOOL_BATCH_OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

$(JEUIA_TOOL_BATCH): $(JEUIA_TOOL_BATCH_OBJECTS)
	$(CC) -o $@ $^ $(DLIBS) $(DLIBS) $(LIBS)

#$(OBJECTS): $(OBJDIR)/%.o: %.cpp
#	$(CREATEOBJDIR)
#	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

.cpp.o:
	$(CC) -c $(FLAGS) $(INCDIRS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(JESIA_TOOL) $(JEUIA_TOOL) $(JESIA_TOOL_BATCH)  $(JEUIA_TOOL_BATCH) $(JESIA_TOOL_OBJECTS) $(JEUIA_TOOL_OBJECTS) $(JESIA_TOOL_BATCH_OBJECTS) $(JEUIA_TOOL_BATCH_OBJECTS) 
