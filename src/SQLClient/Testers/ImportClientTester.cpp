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
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SQLClient/Testers/ImportClientTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SQLClient/SQLClient.h"
#include "SQLClient/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Rundata/Rundata.h"

#include <deque>
#include <vector>


AosImportClientTester::AosImportClientTester()
{
	mName = "SqlClientTester";
}


bool 
AosImportClientTester::start()
{
	cout << "Start AosLogClient Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosImportClientTester::basicTest()
{
	cout << endl <<"SqlImportClient Tester start...." <<endl;
	AosRundataPtr rdata = OmnNew AosRundata();
	rdata->resetForReuse(0);

	OmnString str = "<request>";
	str << "<sdoc type=\"importdata\">";
	str << "<query>select * from user;</query>";
	str << "<actions>";
	str	<< "<action type=\"setattrfromdb\" db_fidx=\"0\" zky_xpath=\"stu_id\" zky_datatype=\"int\">"
		<< "    <docselector zky_docselector_type=\"createdoc\"/>"
		<< "</action>";
	str	<< "<action type=\"setattrfromdb\" db_fidx=\"1\" zky_xpath=\"stu_name\" zky_datatype=\"string\">"
		<< "    <docselector zky_docselector_type=\"createdoc\"/>"
		<< "</action>";
	str	<< "<action type=\"setattrfromdb\" db_fidx=\"2\" zky_xpath=\"stu_fname\" zky_datatype=\"string\">"
		<< "    <docselector zky_docselector_type=\"createdoc\"/>"
		<< "</action>";
	str	<< "<action type=\"setattrfromdb\" db_fidx=\"3\" zky_xpath=\"stu_lname\" zky_datatype=\"string\">"
		<< "    <docselector zky_docselector_type=\"createdoc\"/>"
		<< "</action>";
	str	<< "</actions>";
	str << "</sdoc>";
	str << "</request>";
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");

	AosSQLClientPtr client = OmnNew AosSQLClient();
	client->start(OmnApp::getAppConfig());
	client->convertData(sdoc, rdata);
	return true;
}

