LIBDIR = ../../../obj
DLIBS = \
		$(LIBDIR)/libDataCalc.a \
		$(LIBDIR)/libRundata.a \
		$(LIBDIR)/libMessage.a \
		$(LIBDIR)/libAlarm.a \
		$(LIBDIR)/libOmnUtil.a \
		$(LIBDIR)/libMultiLang.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libTracer.a \
		$(LIBDIR)/libThread.a \
		$(LIBDIR)/libUtil1.a \
		$(LIBDIR)/libUtilComm.a \
		$(LIBDIR)/libXmlUtil.a \
		$(LIBDIR)/libWordParser.a \
		$(LIBDIR)/libSEBase.a \
		$(LIBDIR)/libSEUtil.a \
		$(LIBDIR)/libErrorMgr.a \
		$(LIBDIR)/libAppMgr.a \
		$(LIBDIR)/libConfig.a \
		$(LIBDIR)/libAosApi.a \
		$(LIBDIR)/libUserMgmt.a \
		$(LIBDIR)/libSingleton.a \
		$(LIBDIR)/libDocUtil.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libSEInterfaces.a \
		$(LIBDIR)/libBase64.a \
		$(LIBDIR)/libXmlParser.a \
		$(LIBDIR)/libDebug.a \
		$(LIBDIR)/libPorting.a \
		$(LIBDIR)/libRandom.a \
		$(LIBDIR)/libaosUtil.a \
		$(LIBDIR)/libaos_util_c.a \
		$(LIBDIR)/libaos_util2.a \
		$(LIBDIR)/libObj.a \
		$(LIBDIR)/libaos_alarms.a 


all:
	gcc -L/usr/local/scws/lib -rdynamic -o Chen Chen.cpp -I../.. -DOMN_PLATFORM_UNIX $(DLIBS) $(DLIBS) -lpthread -lssl -lcrypto -ldl -lz -lscws -lstdc++
