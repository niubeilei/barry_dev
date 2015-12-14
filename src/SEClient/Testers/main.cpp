////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
//#include "MySQL/Ptrs.h"
//#include "MySQL/DataStoreMySQL.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Porting/Process.h"
#include "WordMgr/WordMgr1.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IIL.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "SEClient/Testers/TestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
#include "SearchEngineAdmin/SengAdmin.h"
  

AosXmlTagPtr gAosAppConfig;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
//		OmnDataStoreMySQL::startMySQL("root", "chen0318", 
//		 		"mydb", 3306, "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int index = 1;
	int tries = 100;
	int rport = 5575;
	OmnString raddr = "127.0.0.1";
	int max_alarms = 10;
	bool restart = false;
	int createthrds = 0, totalcreate = 0;
	int querythrds = 0, totalquery = 0;
	int modthrds = 0, totalmod = 0;
	int delthrds = 0, totaldel = 0;

OmnScreen << "Process id: " << (int)OmnGetCrtPID() << endl;
	while (index < argc)
	{
		if (strcmp(argv[index], "-restart") == 0 && index < argc)
		{
			restart = true;
			index++;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-createthrds") == 0 && index < argc-1)
		{
			createthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalcreate") == 0 && index < argc-1)
		{
			totalcreate = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-delthrds") == 0 && index < argc-1)
		{
			delthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totaldel") == 0 && index < argc-1)
		{
			totaldel = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-modthrds") == 0 && index < argc-1)
		{
			modthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalmod") == 0 && index < argc-1)
		{
			totalmod = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-querythrds") == 0 && index < argc-1)
		{
			querythrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalquery") == 0 && index < argc-1)
		{
			totalquery = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			max_alarms = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rport") == 0 && index < argc-1)
		{
			rport = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-raddr") == 0 && index < argc-1)
		{
			raddr = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-f") == 0 && index < argc-1)
		{
			// Reading the config
			OmnFile file(argv[index+1], OmnFile::eReadOnly);
			if (!file.isGood())
			{
				cout << "********** Config file invalid!" << endl;
				return -1;
			}

			OmnString str;
			file.readToString(str);
			AosXmlParser parser;
			gAosAppConfig = parser.parse(str, "");
			if (!gAosAppConfig)
			{
				cout << "********** Invalid config file!" << endl;
				return -1;
			}

			index += 2;
			continue;
		}

		index++;
	}

	if (raddr == "")
	{
		cout << "*********** Missing remote address!" << endl;
		return -1;
	}

	if (rport < 1)
	{
		cout << "*********** Missing remote port!" << endl;
		return -1;
	}

	OmnAlarmMgr::setMaxAlarms(max_alarms);
	if (!gAosAppConfig)
	{
		AosXmlParser parser;
		OmnString str = "<config "
			"idx_file_name=\"doc_idx\""
			"remote_addr=\"127.0.0.1\""
			"remote_port=\"5555\""
			"doc_file_name=\"doc\""
			"dirname=\".\""
			"domain_addr=\"58.211.230.20/lps-4.7.2\""
			"idgenfname=\"idgen\""
			"wordid_hashname=\"wordid\""
			"wordid_tablesize=\"1000000\""
			"user_imgdir=\"/home1/AOS/users\""
			"doc_filename=\"doc\""
			"docidgen_name=\"docid\""
			"ignored_hashname=\"ignoredWords\""
			"wordnorm_hashname=\"wordNorm\""
			"server_repairing=\"true\""
			"max_filesize=\"100000000\""
			"large_filesize=\"10000\""
			"xmldoc_incsize=\"1000\""
			"xmldoc_max_headerfiles=\"1000\""
			"xmldoc_max_docfiles=\"2000\""
			"local_addr=\"loopback\""
	   		"local_port=\"5575-5575\""
	    	"service_rcd=\"true\""
		 	"service_name=\"ReqDistrTest\""
		  	"support_actions=\"true\""
		   	"req_delinerator=\"first_four_high\""
		    "max_proc_time=\"10000\""
		    "logfilename=\"zykielog\""
			"max_conns=\"400\">"
			"<convert_info "
			 	"dirname=\".\""
				"doc_filename=\"doc\"/>"
			"<iilmgr "
				"max_iils=\"1000\""
				"iilidgen_name=\"iilid\""
				"idblock_size=\"10\""
				"max_filesize=\"100000000\""
				"large_filesize=\"10000\""
				"iil_filename=\"iil\""
				"iil_incsize=\"300\""
				"iil_max_docfiles=\"2000 \""
				"iil_max_headerfiles=\"1000 \""
				"max_iilsize=\"200000\""
				"iilmgr_sanitycheck=\"false\""
			"/>"
			"<versionmgr "
				"filename=\"version\""
				"maxfsize=\"100000000\""
				"maxdocfiles=\"100\""
				"incsize=\"300\""
				"maxheaderperfile=\"1000000\""
				"maxheaderfiles=\"100\"/>"
			"<IdGens>"
				"<docid crtid=\"1000\""
					"crtbsize=\"5000000\""
					"nextbsize=\"5000000\"/>"
				"<iilid crtid=\"1000\""
					"crtbsize=\"5000000\""
					"nextbsize=\"5000000\"/>"
				"<imageid crtid=\"2200\""
					"crtbsize=\"5000000\""
					"nextbsize=\"5000000\"/>"
				"<zky_compid crtid=\"5000\""
					"crtbsize=\"5000000\""
					"nextbsize=\"5000000\"/>"
			"</IdGens>"
			"<admin "
				"backup_dir=\"/home1/AOS/Backup\""
				"datapath=\"/home1/AOS/Data\""
				"backup_freq=\"20000\"/>"
			"<session_mgr "
				"timed_unit=\"60\"/>"
		"</config>";

		gAosAppConfig = parser.parse(str, "");
		aos_assert_r(gAosAppConfig, -1);
	}

	gAosAppConfig->setAttr("raddr", raddr);
	gAosAppConfig->setAttr("rport", rport);
	gAosAppConfig->setAttr("createthrds", createthrds);
	gAosAppConfig->setAttr("totalcreate", totalcreate);
	gAosAppConfig->setAttr("querythrds", querythrds);
	gAosAppConfig->setAttr("totalquery", totalquery);
	gAosAppConfig->setAttr("modthrds", modthrds);
	gAosAppConfig->setAttr("totalmod", totalmod);
	gAosAppConfig->setAttr("delthrds", delthrds);
	gAosAppConfig->setAttr("totaldel", totaldel);
	if (restart) gAosAppConfig->setAttr("restart", "true");

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSEClientTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	AosSengAdmin::getSelf()->start();
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

