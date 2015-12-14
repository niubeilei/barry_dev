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
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/DocFileMgr.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SearchEngineAdmin/SystemDb.h"
#include "SEClient/SEClient.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "util2/global_data.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool sgIsBigIILServer = false;
int gAosShowNetTraffic = 0;
static bool AosExitApp();
void aosSignalHandler(int value)
{
	if (value == 14) AosExitApp();
}

bool AosExitApp()
{
	return true;
}

int gAosLogLevel = 1;
static u32 sgSiteid = 100;
static bool setConfig(const char *fname);

#include <dirent.h>

AosXmlTagPtr gAosAppConfig;


// extern bool AosDataReader_convertData();
extern bool AosConvertXmlDoc(OmnString &xml);
extern bool AosDataReader_sendReq(const OmnString &contents);

int 
main(int argc, char **argv)
{
	int index = 1;
	aos_global_data_init();
	
	OmnApp::appStart(argc, argv);             
	OmnApp::setVersion("0.10");

	OmnString pattern, value;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			//OmnApp::setConfig(argv[index+1]);
			setConfig(argv[index+1]);
			break;
		}
		index ++;
	}
	
	OmnApp::setConfig(argv[index+1]);

	//OmnThread thread(pthread_self(), "main");
	//OmnThreadMgr::setThread(&thread);
	OmnApp theApp(argc, argv);

	AosNetworkMgrObj::config();
	
	AosSengAdmin::getSelf()->config(OmnApp::getAppConfig());
	AosSengAdmin::getSelf()->start();
	AosXmlDoc::staticInit(OmnApp::getAppConfig());
	try
	{
		OmnString dirname = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString logfname = OmnApp::getAppConfig()->getAttrStr("logfilename");
		OmnString wordhashName = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
	}
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	index = 1;
	while(index < argc)
	{
		if (strcmp(argv[index], "-reread") == 0 && index + 2 < argc)
		{
			// It is to re-read all the docs 
			// 	-reread <max_docid> <siteid> <doc_idx_fname> <doc_fname>
			u64 max_docid = atoi(argv[index+1]);
			//OmnString siteid = argv[index+2];
			u32 siteid = atoi(argv[index+2]);
			OmnString idxfname = argv[index+3];
			OmnString docfname = argv[index+4];
			AosSengAdmin::getSelf()->rereadDocs(siteid, max_docid, idxfname, docfname);
			break;
		}

		if (strcmp(argv[index], "-readdocid") == 0)
		{
			u64 docid = (u64)atoi(argv[index+1]);
			u32 siteid = atoi(argv[index+2]);
			AosSengAdmin::getSelf()->start();
			AosXmlTagPtr doc = AosSengAdmin::getSelf()->readDocByDocid(docid, siteid);
			if (!doc)
			{
				cout << "Failed to read the doc: " << docid << endl;
				exit(0);
			}
			cout << doc->toString().data() << endl;
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-buildbyobjid") == 0)
		{
			// Read a doc by objid 
			// 	admin -readobjid objid maxdocid siteid idxfname docfname
			OmnString objid = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			//OmnString siteid = argv[index+3];
			u32 siteid = atoi(argv[index+3]);
			OmnString idxfname = argv[index+4];
			OmnString docfname = argv[index+5];
			bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			AosSengAdmin::getSelf()->buildDocByObjid(objid, maxdocid, siteid, 
					idxfname, docfname, readDeleted);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-readobjid") == 0)
		{
			// Read a doc by objid 
			// 	admin -readobjid objid maxdocid siteid
			OmnString objid = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			//OmnString siteid = argv[index+3];
			u32 siteid = atoi(argv[index+3]);
			//OmnString idxfname = argv[index+4];
			//OmnString docfname = argv[index+5];
			//bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::readDocByObjid(objid, maxdocid, siteid, false);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-scandocs") == 0)
		{
			// Scan all the docs
			// 	admin -scandocs maxdocid siteid
			u64 startdocid = atoi(argv[index+1]);
			u64 maxdocid = atoi(argv[index+2]);
			//OmnString siteid = argv[index+3];
			u32 siteid = atoi(argv[index+3]);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->scanDocs(startdocid, maxdocid, siteid);
			exit(0);
			break;
		}
		
		if (strcmp(argv[index], "-scandocs1") == 0)
		{
			// Scan all the docs
			// 	admin -scandocs maxdocid siteid
			u64 startdocid = atoi(argv[index+1]);
			u64 maxdocid = atoi(argv[index+2]);
			//OmnString siteid = argv[index+3];
			u32 siteid = atoi(argv[index+3]);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->scanDocs1(startdocid, maxdocid, siteid);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-retrdocid") == 0)
		{
			u64 docid = atoi(argv[index+1]);
			//OmnString siteid = argv[index+2];
			u32 siteid = atoi(argv[index+2]);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->retrieveDocByDocid(siteid, docid);
			break;
		}
		
		if (strcmp(argv[index], "-createLog") == 0)
		{
			OmnString siteid = argv[index+1];
			OmnString ctnr_objid = argv[index+2];
			OmnString logname = argv[index+3];
			AosSengAdmin::getSelf()->start();
//			AosSengAdmin::getSelf()->createLog(siteid, logname, ctnr_objid);
			exit(0);
			break;
		}
	
		if (strcmp(argv[index], "-sdshm") == 0)
		{
			AosSengAdmin::getSelf()->start();
			//OmnString siteid(argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			OmnString receiver(argv[index+2]);
			OmnString msg(argv[index+3]);
			AosSengAdmin::getSelf()->sendShortMsg(msg,receiver, siteid);
		}
		
		if (strcmp(argv[index], "-chkmdfdoc") == 0)
		{
			AosSengAdmin::getSelf()->start();
			//OmnString siteid(argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			OmnString objid(argv[index+2]);
			int tries = atoi(argv[index+3]);
			AosSengAdmin::getSelf()->modifyDocTest(tries, objid, siteid);
		}

		if (strcmp(argv[index], "-readctnr") == 0)
		{
			// Read docs by containers 
			// 	admin -readctnr container maxdocid siteid idxfname docfname deleted
			OmnString container = argv[index+1];
			u64 maxdocid = atoi(argv[index+2]);
			//OmnString siteid = argv[index+3];
			u32 siteid = atoi(argv[index+3]);
			OmnString idxfname = argv[index+4];
			OmnString docfname = argv[index+5];
			bool readDeleted = (strcmp(argv[index+6], "true") == 0);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->readDocsByContainer(container, maxdocid, siteid, 
					idxfname, docfname, readDeleted);
			exit(0);
			break;
		}

		if (strcmp(argv[index], "-deletedoc") == 0)
		{
			// Delete doc:
			// 	admin -config <config> -s -deletedoc <docid> <siteid>
			//
			u64 docid = (u64)atoi(argv[index+1]);
			OmnString siteid = argv[index+2];
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::deleteDocVersion201012(docid);
			exit(0);
		}

		if (strcmp(argv[index], "-rebuilddoc") == 0)
		{
			// Rebuild doc:
			// 	admin -s -rebuilddoc <docid> <siteid> <idxfname> <docfname>
			//
			// Example:
			// 	admin -s -rebuilddoc 12345 100 /home1/AOS/Data/doc_idx 
			// 		/home1/AOS/Data/doc
			u64 docid = (u64)atoi(argv[index+1]);
			//OmnString siteid = argv[index+2];
			u32 siteid = atoi(argv[index+2]);
			OmnString idxfname = argv[index+3];
			OmnString docfname = argv[index+4];
			bool rslt = AosSengAdmin::getSelf()->rebuildDoc(docid, siteid, 
					idxfname, docfname);
			cout << "Rebuild doc: " << docid  << ":" << rslt << endl;
			exit(0);
			break;
		}
		
		if (strcmp(argv[index], "-sendobjbycontainer") == 0)
		{
			// Format: -rebuilddb siteid start_docid end_docid 
			// 		start_version end_version start_log end_log
			//OmnString siteid 	= argv[index+1];
			u32 siteid = atoi(argv[index+1]);
			u64 startDocid 		= atoll(argv[index+2]);
			u64 endDocid 		= atoll(argv[index+3]);
			OmnString ctnr 	= argv[index+4];
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->sendDocByContainerToRemServer(
					ctnr, startDocid, endDocid,siteid);
		}

		if (strcmp(argv[index], "-torturer_getobjid") == 0)
		{
			// 	admin -s -torturer_getobjid <siteid> <tested_objid> <startdocid> <enddocid>
			//OmnString siteid = argv[index+1];
			u32 siteid = atoi(argv[index+1]);
			OmnString tested_objid = argv[index+2];
			u64 startdocid = atoll(argv[index+3]);
			u64 enddocid = atoll(argv[index+4]);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->tortureGetObjid(
				siteid, tested_objid, startdocid, enddocid);
			exit(0);
		}
		
		if (strcmp(argv[index], "-sendobj") == 0)
		{
			// OmnString siteid = argv[index+1];
			// u64 startdocid = atoll(argv[index+2]);
			// u64 enddocid = atoll(argv[index+3]);
			// OmnString ctnr = argv[index+4];
			// AosSengAdmin::getSelf()->start(gAosAppConfig);

			// No such function. Chen Ding, 07/31/2011
			// AosSengAdmin::getSelf()->createObjByContainer(
			// 		startdocid,enddocid,siteid,ctnr);
			OmnAlarm << "No such function!" << enderr;
			exit(0);
		}

		if (strcmp(argv[index], "-createuser") == 0)
		{
			// 'uname' was not declared. It is an error. 
			// Commented out by Chen Ding, 07/31/2011
			/*
			OmnString user = "";
			user << "<embedobj zky_heditor=\"9\" zky_category=\"other\""
				   "zky_siteid=\"100\"  zky_unit=\"zykie\" zky_sex=\"man\" zky_usrtp"
				   "=\"zky_root\" zky_uname=\""
				<< uname 
				<< "\" login_pass_pw=\"12345\"  zky_pfolder=\"yunyuyan_folder\" "
					"zky_usrctnr=\"yunyuyan_account\"  zky_realnm=\""
				<< uname
				<< "\" zky_objimg=\"a2/et14430.jpg\"  zky_objnm=\""
				<< uname 
				<< "\"  _zt1t=\"5\"  zky_otype=\"zky_uact\" zky_place=\"anhui\" "
				   "zky_usrst=\"active\" zky_pctrs=\"yunyuyan_account,ssx_account\"  "
				   "zky_objimg1=\"img/ds800.png\" zkytkt_access=\"zkytkt_mgr\" "
				   "zky_email=\"453757465@qq.com\" zky_email1=\"453757465@qq.com\">"
				   "<zky_passwd>12345</zky_passwd></embedobj>";
			OmnString siteid = argv[index+1];
			u64 startdocid = atoll(argv[index+2]);
			u64 enddocid = atoll(argv[index+3]);
			OmnString ctnr = argv[index+4];
			AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->createObjByContainer(
					startdocid,enddocid,siteid,ctnr);
			*/
			exit(0);
		}

		if (strcmp(argv[index], "-imagepath") == 0 && index + 1 < argc)
		{
			// It is to replace texts
			OmnString path = argv[index+1];
			index += 2;
			AosSengAdmin::getSelf()->setImagePath(path);
			continue;
		}
		

		if (strcmp(argv[index], "-rebuilddb") == 0)
		{
			// Format: -rebuilddb siteid start_docid end_docid 
			// 		start_version end_version start_log end_log
			//OmnString siteid 	= argv[index+1];
			u32 siteid = atoi(argv[index+1]);
			u64 startDocid 		= atoll(argv[index+2]);
			u64 endDocid 		= atoll(argv[index+3]);
			int	startVersion 	= atoi(argv[index+4]);
			int endVersion 		= atoi(argv[index+5]);
			int startLog 		= atoi(argv[index+6]);
			int endLog 			= atoi(argv[index+7]);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->rebuildDb(startDocid, endDocid, 
					startVersion, endVersion, startLog, endLog, siteid);
			break;
		}
		
		if (strcmp(argv[index], "-createSystemDb") == 0)
		{
			// Format: -rebuilddb siteid start_docid end_docid 
			// 		start_version end_version start_log end_log
			//OmnString siteid 	= argv[index+1];
			u32 siteid = atoi(argv[index+1]);
			u64 startDocid 		= atoll(argv[index+2]);
			u64 endDocid 		= atoll(argv[index+3]);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			OmnString ctnrStr = gAosAppConfig->getAttrStr("ctnrStr");
			AosSystemDb sysDB;
			sysDB.createSystemDb(startDocid, endDocid, ctnrStr, siteid);
			break;
		}

		if (strcmp(argv[index], "-modifyCtnr") == 0)
		{
			u64 startdocid = atoll(argv[index+1]);
			u64 enddocid = atoll(argv[index+2]);
			const char *fname = argv[index+3];
			//OmnString siteid(argv[index+4]);
			u32 siteid = atoi(argv[index+4]);
			AosRundataPtr rdata = OmnApp::getRundata();
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
		    AosSengAdmin::getSelf()->convertSystemDocs(
					startdocid, enddocid, fname, siteid, rdata);
		    return 0;
		}
		
		if (strcmp(argv[index], "-send_doc_multi_times") == 0)
		{
			// Format: -rebuilddb siteid start_docid end_docid 
			// 		start_version end_version start_log end_log
			//OmnString siteid 	= argv[index+1];
			u32 siteid = atoi(argv[index+1]);
			u64 docid 		= atoll(argv[index+2]);
			u64 times 		= atoll(argv[index+3]);
			int	startVersion 	= atoi(argv[index+4]);
			int endVersion 		= atoi(argv[index+5]);
			int startLog 		= atoi(argv[index+6]);
			int endLog 			= atoi(argv[index+7]);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->sendDocMultiTimes(docid, times, 
					startVersion, endVersion, startLog, endLog, siteid);
			break;
		}

		if (strcmp(argv[index], "-checkquery") == 0)
		{
			//-checkquery siteid startdocid  enddocid 
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			//OmnString siteid (argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			u64 startdocid = atoll(argv[index+2]);
			u64 enddocid = atoll(argv[index+3]);
			AosSengAdmin::getSelf()->checkQuery(siteid, startdocid, enddocid, 1001);

		}
		
		if (strcmp(argv[index], "-checkversion") == 0)
		{
			//-checkVersion siteid startdocid  enddocid 
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			//OmnString siteid (argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			u64 startdocid = atoll(argv[index+2]);
			u64 enddocid = atoll(argv[index+3]);
			AosSengAdmin::getSelf()->checkVersion(siteid, startdocid, enddocid, 1000);

		}

		if (strcmp(argv[index], "-checkrebuild") == 0)
		{
			// -checkrebuild siteid startdocid enddocid
			//OmnString siteid(argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			u64 startdocid = atoll(argv[index+2]);
			u64 enddocid = atoll(argv[index+3]);

			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->checkRebuild(siteid, startdocid, enddocid);
			return 0;
		}

		if (strcmp(argv[index], "-fixcreator") == 0)
		{
			// -fixcreator siteid start end
			//OmnString siteid(argv[index+1]);
			u32 siteid = atoi(argv[index+1]);
			u64 startdocid = atoll(argv[index+2]);
			u64 enddocid = atoll(argv[index+3]);
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->fixCreatorProblem(siteid, startdocid, enddocid);
			return 0;
		}

		if (strcmp(argv[index], "-changecreator") == 0)
		{
			u64 start_docid = atoll(argv[index+1]);
			u64 end_docid = atoll(argv[index+2]);
			OmnString value(argv[index+3]);
			AosRundataPtr rdata;
			rdata = OmnApp::getRundata();
			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->changeCreator(start_docid, end_docid, value, sgSiteid, rdata);
			return 0;
		}

		if (strcmp(argv[index], "-addone") == 0)
		{
			u64 docid = atoi(argv[index+1]);
			//OmnString siteid(argv[index+2]);
			u32 siteid = atoi(argv[index+2]);
			OmnString dirname(argv[index+3]);
			OmnString idxfname(argv[index+4]);
			OmnString docfname(argv[index+5]);

			//AosSengAdmin::getSelf()->start(gAosAppConfig);
			AosSengAdmin::getSelf()->start();
			AosSengAdmin::getSelf()->addOne(docid, siteid, dirname, idxfname, docfname);
			break;
		}
		index++;
	}

	return 0;
} 


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
	gAosAppConfig = parser.parse(contents, "" AosMemoryCheckerArgs);
	if (!gAosAppConfig)
	{
		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
		exit(-1);
	}

	gAosAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");
	AosXmlDoc::staticInit(gAosAppConfig);
	return true;
}


