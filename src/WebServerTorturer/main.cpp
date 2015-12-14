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
#include "WebServerTorturer/WebClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

int gAosLogLevel = 1;
static OmnString sgSiteid = "100";

const OmnString sgConfig =
    "<config "
		//"remote_addr=\"218.64.170.28\""
		//"remote_addr=\"119.75.218.45\""
		"remote_addr=\"192.168.99.98\""
		"remote_port=\"80\""
		"dirname=\"/home1/AOS/Data\""
		"idgenfname=\"idgen\""
		"serverReadonly=\"true\""
		"wordid_hashname=\"wordid\""
		"wordid_tablesize=\"1000000\""
		"doc_filename=\"doc\""
		"iil_filename=\"iil\""
		"docidgen_name=\"docid\""
		"iilidgen_name=\"iilid\""
		"ignored_hashname=\"ignoredWords\""
		"wordnorm_hashname=\"wordNorm\""
		"max_iils=\"1000\""
		"max_iilsize=\"50000\""
		"max_filesize=\"100000000\""
		"xmldoc_incsize=\"1000\""
		"iil_incsize=\"300\""
		"xmldoc_max_headerfiles=\"1000\""
		"xmldoc_max_docfiles=\"2000\""
		"iil_max_headerfiles=\"1000 \""
		"iil_max_docfiles=\"2000 \""
		"version_filename=\"version\""
		"local_addr=\"loopback\""
   		"local_port=\"5565-5565\""
    	"service_rcd=\"true\""
	 	"service_name=\"ReqDistrTest\""
	  	"support_actions=\"true\""
	   	"req_delinerator=\"first_four_high\""
	    "max_proc_time=\"10000\""
	    "logfilename=\"zykielog\""
		"max_conns=\"400\">"
		"<versionmgr "
			"filename=\"version\""
			"dirname=\"/home/AOS/Data\""
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
			"<imageid crtid=\"1000\""
				"crtbsize=\"5000000\""
				"nextbsize=\"5000000\"/>"
		"</IdGens>"
	"</config>";

#include <dirent.h>

AosXmlTagPtr gAosAppConfig;


int 
main(int argc, char **argv)
{
	OmnString config = sgConfig;
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(config, "");
	gAosAppConfig = root->getFirstChild();

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew AosWebClientSingleton());

		AosWebClientSelf->start(gAosAppConfig);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	while (1)
	{
		OmnSleep(100);
	}


	theApp.exitApp();
	return 0;
} 

