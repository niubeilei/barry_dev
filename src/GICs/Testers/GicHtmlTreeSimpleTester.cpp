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
// 08/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GICs/Testers/GicHtmlTreeSimpleTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "GICs/Ptrs.h"
#include "GICs/GicHtmlTreeSimple.h"
#include "HtmlServer/HtmlCode.h"
#include "HtmlServer/HtmlReqProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"


AosGicHtmlTreeSimpleTester::AosGicHtmlTreeSimpleTester()
{

}


bool AosGicHtmlTreeSimpleTester::start()
{
	cout << "    Start OmnString Tester ..." << endl;
	basicTest();
	return true;
}


bool AosGicHtmlTreeSimpleTester::basicTest()
{
	AosHtmlReqProcPtr htmlPtr = OmnNew AosHtmlReqProc();

	OmnString vpdstr = "<vpd gic_type=\"";
	vpdstr << AOSGIC_HTMLSIMPLETREE << "\"/>";
	AosXmlTagPtr vpd = AosXmlParser::parse(vpdstr);	
	AosHtmlCode code;

	bool rslt = AosGic::createGic(htmlPtr, vpd, 0, "001", code);
	return rslt;
}


