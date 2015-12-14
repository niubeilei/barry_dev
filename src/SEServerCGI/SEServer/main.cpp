
////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEServerCGI/FcgiThrd.h"
#include "SEServerCGI/FcgiReqProc.h"
//#include "SEServerCGI/FcgiThrd.h"
#include "SEServerCGI/SEServer/FcgiSeReqProc.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEUtilServer/LogMgr.h"
#include "SEUtilServer/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "SearchEngine/WordMgr.h"
#include "SearchEngine/IIL.h"
#include "SearchEngine/IILDocid.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/IILMgr.h"
#include "SeAdmin/SeAdmin.h"
#include "SEServer/Ptrs.h"
#include "SEServer/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "Util1/MemMgr.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
extern char ** environ;
#endif

#include "fcgio.h"
#include "fcgi_config.h"  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF

using namespace std;

// Maximum number of bytes allowed to be read from stdin
static const unsigned long STDIN_MAX = 1000000;

AosXmlTagPtr gAosAppConfig;
static OmnString sgAosOpenLzHome;

bool AosExitSearchEngine();

OmnString AosGetOpenLzHome()
{
	return sgAosOpenLzHome;
}


void aosSignalHandler(int value)
{
	if (value == 14) AosExitSearchEngine();
}


int gAosLogLevel = 1;

static bool setConfig(const char *fname)
{
	if (!fname)
	{
		OmnAlarm << "Missing file name!" << enderr;
		exit(-1);
	}

	OmnFile f(fname, OmnFile::eReadOnly);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the config file: " << fname << enderr;
		exit(-1);
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "Failed to read config: " << fname << enderr;
		exit(-1);
	}

	AosXmlParser parser;
	gAosAppConfig = parser.parse(contents, "");
	if (!gAosAppConfig)
	{
		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
		exit(-1);
	}

	gAosAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");
	return true;
}


int main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();

	OmnString domainaddr;
	setConfig("config_norm.txt");

	setConfig("config_norm.txt");

	if (gAosAppConfig->getAttrStr(AOSCONFIG_DOMAINADDR,"")=="")
		gAosAppConfig->setAttr(AOSCONFIG_DOMAINADDR, "218.64.170.28/lps-4.7.2");

OmnScreen << gAosAppConfig->getAttrStr("dirname","")<< endl;

	// Initialize IIL
	AosIIL::staticInit(gAosAppConfig);
	AosXmlDoc::staticInit(gAosAppConfig);
	AosImgProc::staticInit(gAosAppConfig);
	AosWordParser::init(gAosAppConfig);

	OmnApp theApp(argc, argv);
	OmnApp::setConfig("config_norm.txt");
	try
	{
		theApp.startSingleton(OmnNew AosQueryMgrSingleton());
		theApp.startSingleton(OmnNew AosWordMgrSingleton());
		theApp.startSingleton(OmnNew AosDocServerSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
		theApp.startSingleton(OmnNew AosLogMgrSingleton());
		theApp.startSingleton(OmnNew AosIdGenMgrSingleton());
		theApp.startSingleton(OmnNew AosSeAdminSingleton());
		theApp.startSingleton(OmnNew AosSecurityMgrSingleton());
		theApp.startSingleton(OmnNew AosSessionMgrSingleton());  

		AosIILMgrSelf->start(gAosAppConfig);
		AosDocServerSelf->start(gAosAppConfig);
		OmnString dirname = gAosAppConfig->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString logfname = gAosAppConfig->getAttrStr("logfilename");
		OmnString wordhashName = gAosAppConfig->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
		u32 wordidTablesize = gAosAppConfig->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0);
    	AosWordMgrSelf->start(dirname, wordhashName, wordidTablesize);
		AosLogMgrSelf->start(dirname, logfname);
		AosSeAdminSelf->start(gAosAppConfig);
		AosSecurityMgrSelf->start(gAosAppConfig);
		AosSessionMgrSelf->start(gAosAppConfig);

		theApp.startSingleton(OmnNew AosSeIdGenMgrSingleton());  
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosSeReqProc::config(gAosAppConfig);
	AosFcgiThrd::startThreads(OmnNew AosFcgiSeReqProc(OmnNew AosSeReqProc()));

	AosSeReqProcPtr proc = OmnNew AosSeReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(gAosAppConfig->toString()))
	{
		OmnAlarm << "Failed the configuration: " 
			<< gAosAppConfig->toString() << enderr;
		theApp.exitApp();
		return -1;
	}
	reqDistr.start();

	while (1)
	{
		OmnSleep(1000);
	}
	return 0;
}


static OmnMutex sgLock;

int AosFcgiGetRequest(FCGX_Request &request)
{
	sgLock.lock();
    int rc = FCGX_Accept_r(&request);
	sgLock.unlock();
	return rc;
}


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;

	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgrSelf->stop();
	OmnSleep(1);

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryMgrSelf->stop();

	OmnScreen << "To stop Requester!" << endl;
	AosSeReqProc::stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocServerSelf->exitSearchEngine();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocServer::getSelf()->getRepairingMaxDocid() << endl;

	AosDocServerSelf = 0;
	gAosAppConfig = 0;
    AosWordMgrSelf = 0;
	AosLogMgrSelf = 0;

	exit(0);
	return true;
}


