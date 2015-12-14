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
// 04/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/Tester/RvgTester.h"

#include "Debug/Debug.h"
#include "rvg_c/rig_basic.h"
#include "SQLite/DataStoreSQLite.h"
#include "SQLite/Tester/User.h"
#include "SQLite/Tester/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "TorturerWrappers/RvgWrapper.h"
#include "Util/File.h"
#include "Util/OmnNew.h"


static std::string sgRigDef = 
"<Rig>"
	"<Name>operation</Name>"
	"<Type>RigBasic</Type>"
	"<Ranges>"
		"<Range>0, 0, 500</Range>"
		"<Range>1, 1, 450</Range>"
		"<Range>2, 2, 500</Range>"
		"<Range>3, 3, 1000</Range>"
		"<Range>4, 4, 1</Range>"
	"</Ranges>"
"</Rig>";

bool 
AosRvgWrapperTester::start()
{
	// 
	// Test default constructor
	//
	return basicTest();
}


bool 
AosRvgWrapperTester::basicTest()
{
	AosRvgWrapper t1;
	OmnTest_Assert(t1.removeAllFromDb()) << endtc;

	/*
	int opr;
	aos_xml_node_t *node = aos_xml_node_create_from_str(sgRigDef.data());
	aos_assert_r(node, false);
	aos_rig_t *rig = aos_rig_factory(node);
	aos_assert_r(rvg, false);

	int tries = 0;
	while (tries++ < 1000)
	{
		aos_assert_r(!rig->mf->next_int(rig, &opr), false);
		switch (opr)
		{
		case 0:
			 addRvg();
			 break;

		case 1:
			 deleteRvg();
			 break;

		case 2:
			 modifyRvg();
			 break;

		case 3:
			 removeAllRvgs();
			 break;

		case 4:
			 queryRvgs();
			 break;

		default:
			 OmnAlarm << "Unrecognized operator: " << opr << enderr;
			 break;
		}

		check();
	}
	*/
	OmnTest_Assert(addRvg()) << endtc;
	OmnTest_Assert(queryRvgs()) << endtc;

	return true;
}


bool 
AosRvgWrapperTester::addRvg()
{
	AosRvgWrapper wp;
	wp.setName("Chen");
	wp.setKeywords("Kw1 Kw2");
	wp.setXml("This is a test");
	OmnTest_Assert(wp.addToDb()) << endtc;
	return true;	
}


bool 
AosRvgWrapperTester::deleteRvg()
{
	return true;
}


bool 
AosRvgWrapperTester::modifyRvg()
{
	return true;
}


bool 
AosRvgWrapperTester::removeAllRvgs()
{
	return true;
}


bool
AosRvgWrapperTester::queryRvgs()
{
	AosRvgWrapper wp;
	OmnTest_Assert(wp.getRvg("Chen")) << endtc;
	OmnTest_Assert(wp.getName() == "Chen") << endtc;
	OmnTest_Assert(wp.getKeywords() == "Kw1 Kw2") << "Actual: " << wp.getKeywords() << endtc;
	OmnTest_Assert(wp.getXml() == "This is a test") << "Actual: " << wp.getXml() << endtc;
	return true;
}

