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
// 2012/03/07 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataFormat/Tester/DataFormatTester.h"

#include "DataFormat/DataFormatter.h"
#include "alarm_c/alarm.h"
#include "Util/Buff.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "DataFormat/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "SEUtil/ValueDefs.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"


AosDataFormatTester::AosDataFormatTester()
{
}


bool AosDataFormatTester::start()
{
	cout << "    Start AosDataFormat Tester ..." << endl;
	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 1000;
	OmnScreen << "Tries Total : " << tries << "\n" << endl;
	
	for (int i=0; i<tries; i++)
	{

		OmnScreen << "--------------------Tries: " << i << " start------------------" << endl;
		basicTest();
		OmnScreen << "--------------------Tries: " << i << " end------------------" << endl;
		OmnSleep(1);
	}
	return true;
}


bool 
AosDataFormatTester::basicTest()
{
	OmnString docstr = "";
	OmnString formatestr = createFormatXml(docstr);
	
	AosXmlTagPtr formateroot = AosXmlParser::parse(formatestr AosMemoryCheckerArgs);
	aos_assert_r(formateroot, false);
	AosXmlTagPtr formatexml = formateroot->getFirstChild();
	
	AosXmlTagPtr docxml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(docxml, false);
	AosXmlTagPtr doc = docxml->getFirstChild();

	OmnScreen << "format:" << formatestr << endl;
	OmnScreen << "doc:" << docstr << endl;
	
	AosRundataPtr rdata = OmnApp::getRundata(); 
	AosDataFormatterPtr format = OmnNew AosDataFormatter(formatexml, rdata);
	
	AosBuffPtr buff = format->serializeTo(doc, rdata);
	OmnScreen << "buff:" << buff->data() << endl;
	
	AosXmlTagPtr xml = format->serializeFrom(buff->data(), buff->dataLen(), rdata);
	aos_assert_r(xml, false);
	OmnString xmlstr = xml->toString();
	OmnScreen << "xml:" << xmlstr << endl;
	
	aos_assert_r(docstr == xmlstr, false);

	return true;
}


OmnString
AosDataFormatTester::createFormatXml(OmnString &doc)
{
	OmnString str;
	doc = "";
	OmnString tagname = OmnRandom::letterStr(5, 10);
	str << "<format " << AOSTAG_NAME << "=\"" << tagname << "\"><elements>";
	doc << "<" << tagname << ">";
	int i = rand() % 10 + 1;
	while(i--)
	{
		OmnString subtag = OmnRandom::letterStr(5, 10);
		str << "<element " << AOSTAG_NAME << "=\"" << subtag << "\" " << AOSTAG_TYPE << "=\"" << getElemType() << "\" />";
		doc << "<" << subtag << "><![CDATA[" << OmnRandom::nextU64() << "]]></" << subtag << ">";
	}
	str	<< "</elements></format>";
	doc << "</" << tagname << ">";
	return str;
}


OmnString 
AosDataFormatTester::getElemType()
{
	return "u64";
}
