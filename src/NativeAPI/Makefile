# File: makefile
#
# This is a makefile for use with g++
#


LIBNAME = NativeAPI

CC = g++

.SUFFIXES: .c .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize)  -DAOS_USERLAND -fpermissive#-DAOS_SEMANTICS

INCDIRS = -I. -I../ 

OBJDIR = ../../obj/$(LIBNAME)

CREATEDIR = $(shell if [ -d $(OBJDIR) ]; then echo ""; else mkdir $(OBJDIR); fi)

OUTPUTDIR = ../../obj

LIBDIR = ../../obj
LIBDAR = ../../obj

#LIBS = -lsybdb -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lpython2.6 -lMagick++ \
#	   -lboost_system -lboost_date_time  -lssl -lboost_regex -lcrypto -ldl -lz -lutil -lrt -lthrift -lboost_thread -lreadline

LIBS = -lpthread -L/usr/local/scws/lib -lscws -lssl -lreadline -lcrypto


#DLIBS = \
#		../NativeAPI_DepLib/libNativeAPI_DepLib.so \
#		$(LIBDIR)/libSEClient.a \
#		$(LIBDIR)/libTinyXml.a \
#		$(LIBDIR)/libaos_util.a \
#		$(LIBDIR)/libaos_util_c.a \
#		$(LIBDIR)/libaos_alarms.a 


DOBJS = \
	../../obj/aos_alarms/alarm.o \
	../../obj/aos_util_c/*.o \
	../../obj/TinyXml/*.o

#
# objects and targets
#
OBJECTS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard *.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Porting/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Util/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Thread/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard UtilComm/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Tracer/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Debug/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Alarm/*.cpp))
OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Util1/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard TinyXml/*.cpp))
#OBJECTS += $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard alarm_c/*.c))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard AppMgr/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Rundata/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard XmlUtil/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard WordParser/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard Message/*.cpp))
#OBJECTS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(wildcard SEBase/*.cpp))

TARGETLIB = lib$(LIBNAME).a


#
# Rules for normal comile and link 
#
all:: lib
	
lib: $(OBJECTS) 
	ar rvu $(TARGETLIB) $^ $(DOBJS)
	mv $(TARGETLIB) $(OUTPUTDIR)

$(OBJECTS): $(OBJDIR)/%.o: %.cpp
	$(CREATEDIR)
	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

#$(OBJDIR)/alarm_c/alarm.o: alarm_c/alarm.c
#	$(CREATEDIR)
#	$(CC) -c $(FLAGS) $< $(INCDIRS) -o $@

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/$(TARGETLIB) 
