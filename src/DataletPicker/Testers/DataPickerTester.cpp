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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataPickerEngine/Testers/DataPickerTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"

AosDataPickerTester::AosDataPickerTester()
{
}


bool 
AosDataPickerTester::start()
{
	config();
	return basicTest();
}


bool
AosDataPickerTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("data_picker_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	
	return true;
}


bool 
AosDataPickerTester::basicTest()
{
	AosDataPickerObjPtr thisptr(this, false);
	for (int i=0; i<mTries; i++)
	{
		OmnString query = "docid:";
		query << mDocids[i];
		DQ(rdata, query, thisptr);
	}
	return true;
}


bool 
AosDataPickerTester::procDatalet(
		const AosRundataPtr &rdata, 
		const AosDataletPtr &datalet, 
		const OmnString &query)
{
	// It modifies the datalet.
	aos_assert_rr(datalet, rdata, false);
	u64 value = datalet->getU64("field1", 0);
	value++;
	datalet->set("field1", value);
	return true;
}


bool 
AosDataPickerTester::procDatalet(
		const AosRundataPtr &rdata, 
		const vector<AosDataletPtr> &datalets, 
		const OmnString &query)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataPickerTester::dataProcFailed(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg, 
		const OmnString &query)
{
	OmnShouldNeverComeHere;
	return false;
}

