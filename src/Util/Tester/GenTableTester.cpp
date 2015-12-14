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
// 06/30/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/GenTableTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"
#include "XmlParser/XmlItem.h"
#include <limits.h>
#include <string.h>

static OmnString example1 = 
"<Table>"
	"<Name>gen_rule_table</Name>"
	"<MaxEntries>500</MaxEntries>"
	"<KeyFields>sip, sport, dip, dport, proto, ifname</KeyFields>"
	"<Fields>"
		"<Field>"
			"<Type>string</Type>"
			"<Name>sip</Name>"
		"</Field>"
		"<Field>"
			"<Type>int</Type>"
			"<Name>sport</Name>"
		"</Field>"
		"<Field>"
			"<Type>string</Type>"
			"<Name>dip</Name>"
		"</Field>"
		"<Field>"
			"<Type>int</Type>"
			"<Name>dport</Name>"
		"</Field>"
		"<Field>"
			"<Type>string</Type>"
			"<Name>proto</Name>"
		"</Field>"
		"<Field>"
			"<Type>string</Type>"
			"<Name>ifname</Name>"
		"</Field>"
		"<Field>"
			"<Type>string</Type>"
			"<Name>priority</Name>"
		"</Field>"
	"</Fields>"
"</Table>";


bool OmnGenTableTester::start()
{
	// 
	// Test default constructor
	//
	u32 tries = 10;
	basicTest(tries);
	return true;
}


bool
OmnGenTableTester::basicTest(const u32 tries)
{
	OmnXmlItemPtr item = OmnNew OmnXmlItem(example1);
	AosGenTablePtr table = OmnNew AosGenTable(item);

	for (u32 i=0; i<tries; i++)
	{
		AosGenRecordPtr rec = table->createRecord();
		rec->set("sip", "192.168.0.1");
		rec->set("sport", (int)i);
		rec->set("dip", "10.10.10.1");
		rec->set("dport", 2000);
		rec->set("proto", "udp");
		rec->set("ifname", "WANIF");
		rec->set("priority", "low");

		OmnTrace << "Record: " << rec->toString() << endl;

		OmnTCTrue(table->appendRecord(rec)) << "i = " << (int)i << endtc;
	}

	return true;
}


