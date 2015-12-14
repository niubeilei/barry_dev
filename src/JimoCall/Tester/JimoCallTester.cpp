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
// 2014/12/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/Tester/JimoCallTester.h"

#include "Debug/Debug.h"
#include "JimoCall/JimoCall.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "JimoAPI/JimoHelloWorld.h"
#include "JimoAPI/JimoDocEngine.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"



AosJimoCallTester::AosJimoCallTester()
{
}


bool 
AosJimoCallTester::start()
{
	OmnScreen << "To start testing" << endl;
	basicTest();
	return true;
}


bool
AosJimoCallTester::basicTest()
{
	/*
	AosRundataPtr rdata = OmnNew AosRundata();

	*/

	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	rdata->setUserid(307);
	rdata->setCid("50003");

	bool rslt;
	OmnString value;
	OmnString message = "JimoCall_HelloWorld";

	u64 i = 0, j = 0;
	u64 tStart, tUse;
	tStart = OmnGetTimestamp();
	map<u64, u64> time_useds;

	while(++i <= 10)
	{
		rslt = Jimo::jimoCallSyncRead(rdata.getPtr(), message, value);
		aos_assert_r(rslt, false);
		OmnScreen << "i : " << i << ", value : " << value << endl;
		if(i%1000==0)
		{
			tUse = OmnGetTimestamp() - tStart;
			j++;
			//OmnScreen << "j : " << j << ", i : " << i << ", time: " << tUse << endl;
			time_useds[j] = tUse;
			tStart = OmnGetTimestamp();
		}
	}
	for(map<u64, u64>::iterator itr=time_useds.begin(); itr!=time_useds.end(); itr++)
	{
		OmnScreen << "times : " << itr->first << ", used : " << itr->second << endl;
	}


/*
	OmnString docstr = "<doc ";
	docstr << AOSTAG_OBJID << "=\"" << "test_objid" << "\" "
		<< AOSTAG_PARENTC << "=\"test_ctnr\" "
    	<< AOSTAG_CTNR_PUBLIC << "=\"true\" "
	    << AOSTAG_PUBLIC_DOC << "=\"false\" " << "/>";

	//AosXmlTagPtr doc = AosXmlParser::parse(docstr);
	if (!doc)
	{
		OmnScreen << "doc parse failed" << endl;
		return false;
	}
	
	rdata->setReceivedDoc(doc, false);
	AosXmlTagPtr cmd;
	bool resolve_objid = false;
	bool flag = Jimo::jimoCreateDoc1(rdata.getPtr(), cmd, "111113", doc, resolve_objid, 0, 0, true); 
	if(!flag)
	{
		OmnScreen << "doc create failed" << endl;
		return false;
	}
	AosXmlTagPtr doc = Jimo::jimoGetDocByObjid(rdata.getPtr(), "zky_objid");
	aos_assert_r(doc, false);
	OmnScreen << "call result : " << rslt << " data : " << doc->toString() << endl;

	rslt = Jimo::jimoCallSyncNorm( rdata.getPtr(), message, value );
	aos_assert_r( rslt, false );
	OmnScreen << "call result : " << rslt << " data : " << value << endl;

	u64 docid = 20000;
	rslt = Jimo::jimoDeleteObj( rdata.getPtr(), docid,  message, false );
	aos_assert_r( rslt, false );
	OmnScreen << "call result : " << rslt << " data : " << value << endl;
*/
	return true;
}

