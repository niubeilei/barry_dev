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
// 04/17/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "MySQL/DataStoreMySQL.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "TorturerWrappers/ObjectWrapper.h"
#include "SearchEngine/SeXmlParser.h"
#include "SearchEngine/XmlTag.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/IILMgr.h"
#include "SearchEngine/IdGenMgr.h"
#include "SearchEngine/LogMgr.h"
#include "SearchEngine/WordMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "XmlInterface/Server/WebReqProc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/DbConvert.h"
#include "XmlInterface/Server/CmdLine.h"


#include <stdlib.h>


const OmnString sgConfig =
    "<config local_addr=\"loopback\""
		"dirname=\"/AOS/Data\""
	 	"idgenfname=\"idgen\""
   		"local_port=\"5555-5555\""
    	"service_rcd=\"true\""
	 	"service_name=\"ReqDistrTest\""
	  	"support_actions=\"true\""
	   	"req_delinerator=\"first_word_high\""
	    "max_proc_time=\"10000\""
		"max_conns=\"400\">"
		"<IdGens>"
			"<docid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
			"<iilid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
		"</IdGens>"
	"</config>";

AosXmlTagPtr gAosAppConfig;

#include <dirent.h>
int aos_getFiles()
{
	struct dirent **namelist;
	int n;
	n = scandir(".", &namelist, 0, alphasort);
	if (n < 0)
		OmnAlarm << "Failed to scan" << enderr;
	else
	{
		while(n--)
		{
			printf("%s\n", namelist[n]->d_name);
			free(namelist[n]);
		}
		free(namelist);
	}

	return 0;
}

static OmnString sgAosOpenLzHome;
OmnString AosGetOpenLzHome()
{
	return sgAosOpenLzHome;
}


int 
main(int argc, char **argv)
{
	aos_global_data_init();


    sgAosOpenLzHome = getenv("LPS_HOME");
    if (sgAosOpenLzHome == "")
    {
        cout << "*******************************\n"
			<< "Missing 'OpenLzHome' environment variable!" << endl;
        exit(0);
    }

	int index = 1;
	OmnString passwd;
	int port = -1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-passwd") == 0 && index < argc-1)
		{
			passwd = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-port") == 0 && index < argc-1)
		{
			port = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	if (passwd == "")
	{
		cout << "********** Missing pasword!" << endl;
		return -1;
	}

	if (port == -1)
	{
		cout << "********** Missing port!" << endl;
		return -1;
	}

	OmnString config = sgConfig;
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(config, "");
	gAosAppConfig = root->getFirstChild();

	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", passwd, 
		 		"mydb", port, "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
		theApp.startSingleton(OmnNew AosSiteMgrSingleton());
		theApp.startSingleton(OmnNew AosQueryMgrSingleton());
		theApp.startSingleton(OmnNew AosWordMgrSingleton());
		theApp.startSingleton(OmnNew AosDocServerSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
		theApp.startSingleton(OmnNew AosLogMgrSingleton());
		theApp.startSingleton(OmnNew AosIdGenMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosWSProcCmdLine(argc, argv);


	AosWebReqProcPtr proc = OmnNew AosWebReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(sgConfig))
	{
		OmnAlarm << "Failed the configuration: " << sgConfig << enderr;
		theApp.exitApp();
		return -1;
	}
	reqDistr.start();


	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 
