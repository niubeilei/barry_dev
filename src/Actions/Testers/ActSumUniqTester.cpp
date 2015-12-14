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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActSumUniqTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DataColComp/Ptrs.h"
#include "DataColComp/DataColComp.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosActSumUniqTester::AosActSumUniqTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	//mName = "ActSumUniqTester";
	mNum = 99999;
}


AosActSumUniqTester::~AosActSumUniqTester()
{
}


bool 
AosActSumUniqTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActSumUniqTester::basicTest()
{
	AosXmlTagPtr tag = config();
	int tries = 0;
	while (tries < 10000)
	{
OmnScreen << "Tries   ======  " << tries << endl; 
		AosXmlTagPtr sdoc = tag->getFirstChild("sumuniq");
		aos_assert_r(sdoc, 0);
		bool rslt = false;
		AosActSumUniq action(sdoc, mRundata);

		rslt = createData(action);
		aos_assert_r(rslt, false);

		finish(action);

		rslt = verify(tag);
		clearData();
		tries++;
	}
	return true;
}

void
AosActSumUniqTester::clearData()
{
	mStrs.clear();
	mInts.clear();
	mObjid = "";
	mKeys.clear();
	mValues.clear();
}


AosXmlTagPtr
AosActSumUniqTester::config()
{
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, 0);
	AosXmlTagPtr conf = app_conf->getFirstChild("testers");

	return conf;
}


bool
AosActSumUniqTester::createData(AosActSumUniq &action)
{
	OmnString docstr = "ionsjflsik";
	OmnString str, num;
	//num << mNum --;
	num << OmnRandom::lowcaseStr(5, 5);
	for (u32 i = 0; i< 1000; i++)
	{
		int r = rand() %5;
		if (r == 1)
		{
			num = "";
			//num << mNum --;
			num = OmnRandom::lowcaseStr(5, 5);
		}
		str << docstr << num;
		str << "hpjrkwerlsjdfkshndf";
		bool rslt = runAction(action, str);
		aos_assert_r(rslt, false);
		mStrs.push_back(num);
		str = "";
	}
	return true;
}

bool
AosActSumUniqTester::runAction(AosActSumUniq &action, const OmnString &str)
{
	aos_assert_r(mRundata->getSiteid() > 0, false);

	//return action.run(str.data(),  str.length(), mRundata);
	int len  = str.length();
	return action.run(str.data(),  len, mRundata);
}

bool
AosActSumUniqTester::finish(AosActSumUniq &action)
{
	mObjid = "test";
	mObjid << mNum --;
	action.finish(mObjid, mRundata);
	aos_assert_r(mObjid != "", false);
	return true;
}

bool
AosActSumUniqTester::verify(const AosXmlTagPtr &conf)
{
	bool rslt = verifyMapDoc();
	aos_assert_r(rslt, false);

	AosDataType::E data_type;
	rslt = verifyToInt(conf, data_type);
	aos_assert_r(rslt, false);
	rslt = verifyToStr(conf, data_type);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosActSumUniqTester::verifyMapDoc()
{
	AosXmlTagPtr doc = AosRetrieveDocByObjid(mObjid, false, mRundata);
	aos_assert_r(doc, false);
	AosXmlTagPtr tags = doc->getFirstChild("records");
	aos_assert_r(doc, false);
	AosXmlTagPtr tag = tags->getFirstChild();
	aos_assert_r(doc, false);
	OmnString a, f;
	while (tag)
	{
		OmnString key = tag->getAttrStr(AOSTAG_KEY, "");
		aos_assert_r(key != "", false);

		OmnString value = tag->getAttrStr(AOSTAG_VALUE, "");
		aos_assert_r(value != "", false);

		//aos_assert_r(atoll(key.data()) == num, false);
		u64 vv = atoll(value.data());

		mKeys.insert(make_pair(key, vv));
		mValues.insert(make_pair(vv, key));
		
		a = f;
		f = key;
		if (f !="")
		{
			aos_assert_r(a < f, false);
		}
//OmnScreen << "key : " << key << endl;	
		tag = tags->getNextChild(); 
	}
	return true;
}



bool
AosActSumUniqTester::verifyToInt(
		const AosXmlTagPtr &def, 
		AosDataType::E &data_type)
{
	AosDataColCompPtr colComp; 
	AosXmlTagPtr tag = def->getFirstChild("colcomp1");
	if (tag)
	{
		aos_assert_r(mObjid != "", false);
		tag->setAttr("zky_map_objid", mObjid);
		colComp = AosDataColComp::createDataColComp(tag, mRundata);
		if (!colComp)
		{
			AosSetErrorU(mRundata, "failed_create_columncompression") << ": " << tag->toString();
			AosLogError(mRundata);
			return false;
		}
	}
	
	AosValueRslt valueRslt;
	for (u32 i = 0; i < mStrs.size(); i++)
	{
		OmnString ss = mStrs[i];
		aos_assert_r(ss != "", false);
		bool rslt = colComp->convertToInteger(ss.data(), ss.length(), valueRslt, data_type, mRundata);
		aos_assert_r(rslt, false);
		u64 vv = 0;
		rslt = valueRslt.getU64Value(vv, mRundata);
		aos_assert_r(rslt, false);

		u64 v1 = mKeys[ss];
		switch (data_type)
		{
		case AosDataType::eU8:
			aos_assert_r((u8)v1 == (u8)vv, false);
			break;
		case AosDataType::eU16:
			aos_assert_r((u16)v1 == (u16)vv, false);
			break;
		default:
			OmnAlarm << "Invalid data type:" << enderr;
			return false;
		}

		aos_assert_r(v1 == vv, false);
		aos_assert_r(mValues[vv] == ss, false);

		mInts.push_back(vv);
		valueRslt.reset();
	}
	return true;
}

bool
AosActSumUniqTester::verifyToStr(
		const AosXmlTagPtr &def, 
		const AosDataType::E &data_type)
{
	AosDataColCompPtr colComp; 
	AosXmlTagPtr tag = def->getFirstChild("colcomp2");
	if (tag)
	{
		aos_assert_r(mObjid != "", false);
		tag->setAttr("zky_map_objid", mObjid);
		colComp = AosDataColComp::createDataColComp(tag, mRundata);
		if (!colComp)
		{
			AosSetErrorU(mRundata, "failed_create_columncompression") << ": " << tag->toString();
			AosLogError(mRundata);
			return false;
		}
	}

	int len = AosDataType::getValueSize(data_type);
	AosValueRslt valueRslt;
	for (u32 i = 0; i < mInts.size(); i++)
	{
		u64 vv = mInts[i];
		bool rslt = colComp->convertToStr((char *) &vv, len, valueRslt, mRundata);
		aos_assert_r(rslt, false);

		OmnString str = valueRslt.getValueStr1();
		aos_assert_r(mValues[vv] == str, false); 

		u64 v1 = mKeys[str];
		switch (data_type)
		{
		case AosDataType::eU8:
			aos_assert_r((u8)v1 == (u8)vv, false);
			break;
		case AosDataType::eU16:
			aos_assert_r((u16)v1 == (u16)vv, false);
			break;
		default:
			OmnAlarm << "Invalid data type:" << enderr;
			return false;
		}
		aos_assert_r(v1 == vv, false);
		valueRslt.reset();	
	}
	return true;
}
