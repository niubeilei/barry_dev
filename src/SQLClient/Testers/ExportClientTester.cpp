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
#include "SQLClient/Testers/ExportClientTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "SQLClient/SQLClient.h"
#include "SQLClient/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Rundata/Rundata.h"

#include <deque>
#include <vector>


AosExportClientTester::AosExportClientTester()
{
	mName = "ExportClientTester";
}


bool 
AosExportClientTester::start()
{
	cout << "Start AosLogClient Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosExportClientTester::basicTest()
{
	cout << endl <<"SqlExportClient Tester start...." <<endl;
	AosRundataPtr rdata = OmnNew AosRundata();
	rdata->resetForReuse(0);

	OmnString str;
	str << "<sdoc type=\"exportdata\" tablename=\"user\" >";
	str << "<query>";
	str << "</query>";
	str << "<data>";
	str << "<xml user_id=\"117\" user_name=\"HanHan\" user_fname=\"Mei\" user_lname=\"Han\" user_email=\"sql0102\" user_ophone=\"32156\" user_hphone=\"3456\" user_status=\"b\"/>";
	str << "<xml user_id=\"118\" user_name=\"NanNan\" user_fname=\"lei\" user_lname=\"Li\" user_email=\"sql0102\" user_ophone=\"321\" user_hphone=\"456\" user_status=\"a\"/>";
	str << "<xml user_id=\"119\" user_name=\"ShanShan\" user_fname=\"Mei\" user_lname=\"Li\" user_email=\"sql0102\" user_ophone=\"321\" user_hphone=\"456\" user_status=\"a\"/>";
	str	<< "</data>";
	str << "<actions>";
	str << "<action type=\"composestr\">"
		<< "    <substrs>"
		<< "        <substr zky_value_type=\"const\" datatype=\"string\">insert into </substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"tablename\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"receiveddoc\"/> "
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">user values(</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_id\" zky_data_type=\"int\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">, '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_name\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_fname\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_lname\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_email\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_ophone\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_hphone\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">', '</substr>"
		<< "        <substr zky_value_type=\"attr\" zky_xpath=\"user_status\" zky_data_type=\"string\">"
		<< "           <zky_docselector zky_docselector_type=\"sourcedoc\"/>"
		<< "		</substr>"
		<< "		<substr zky_value_type=\"const\" datatype=\"string\">')</substr>"
		<< "	</substrs>"
		<< "<setresult type=\"addattr\"></setresult>"
		<< "</action>"
		<< "</actions>";
	str << "</sdoc>";
	AosXmlParser parser;
	AosXmlTagPtr sdoc = parser.parse(str, "");

	AosSQLClientPtr client = OmnNew AosSQLClient();
	client->start(OmnApp::getAppConfig());
	client->convertData(sdoc, rdata);
	return true;
}

