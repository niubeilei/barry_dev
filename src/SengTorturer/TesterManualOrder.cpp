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
// 05/25/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterManualOrder.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SengTorturer/SengTesterFileMgr.h"
#include "SengTorturer/SengTesterThrd.h"
#include "SengTorturer/SengTesterMgr.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <stdlib.h>

static u32 sgnum = 0;

AosManualOrderTester::AosManualOrderTester(const bool regflag)
:
AosSengTester(AosSengTester_ManualOrder, AosTesterId::eManualOrder, regflag)
{
}

AosManualOrderTester::AosManualOrderTester()
:
AosSengTester(AosSengTester_ManualOrder, "manualorder", AosTesterId::eManualOrder)
{
}


AosManualOrderTester::~AosManualOrderTester()
{
}


bool 
AosManualOrderTester::test()
{
	// This function tests the request eManualOrder
	mCtrName = "img";
	mOrdName = "0sw02"; 

	int tries = rand() % eManualTryWeight + 1;
	tries = 100;
	while (tries-->0)
	{
		append();
		continue;
	}
	
	tries = 0;
	while (tries-->0)
	{
		swap();
		continue;
	}
	
	tries = 0;
	while (tries-->0)
	{
		del();
		continue;
	}
	
	tries = 50;
	while (tries-->0)
	{
		move();
		continue;
	}
	

	/*	while (tries-->0)
	{
		append();
		continue;
		////////////
	
		int idx = rand() % 100;
		if(idx < eAppendWeight)
		{
			rslt = append();
			aos_assert_r(rslt, false);
		}
		else if(idx < eSwapWeight)
		{
			rslt = swap();
			aos_assert_r(rslt, false);
		}
		else if(idx < eMoveWeight)
		{
			rslt = move();
			aos_assert_r(rslt, false);
		}                         
		else
		{
			rslt = del();	
			aos_assert_r(rslt, false);
		}
	}
*/	
//OmnScreen << "*********************************Error*************************" << sgnum << endl; 

	return true;
}


bool
AosManualOrderTester::append()
{
	// get docid;
	u64 value;
	u64 docid = (((u64)(rand() % 1000)) << 16) + rand() % 5000;
	map<u64, u64>::iterator iter = mDocid.find(docid);
	while (iter != mDocid.end())
	{
		docid = (((u64)(rand() % 1000)) << 16) + rand() % 5000;
		iter = mDocid.find(docid);
	}
	
	OmnString reqid = "crtcmol";
	OmnString args;
		args << "container_docid=" << mCtrName << ",docids=" << docid << ",ordername=" << mOrdName;
	u32 siteid = mThread->getSiteid();

	OmnString rslt;
	AosSengAdmin::getSelf()->manualOrder(siteid, reqid, args, rslt);
OmnScreen << "**********rslt*************" << rslt << endl;	

	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(rslt, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);

	AosXmlTagPtr record = child->getNextChild("record");
	while (record)
	{
		u64 vl = record->getAttrU64("value", 0);
		u64 dd = record->getAttrU64("did", 0);


		if (dd == docid)
		{
			value = vl;
		}
		record = child->getNextChild("record");
	}

	// Test
	if (mValue.size()>0)
	{
		u64 end = mValue.rbegin()->first;
		u64 temp = end + (((u64)1) << 32);
		if (temp != value) 
		{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
			return false;
		}
	}

	mValue[value] = docid;
	showRslt();

	return true;
}


bool
AosManualOrderTester::swap()
{
	if (mValue.size()<2) return true;

	int i = mValue.size();
	int t1 = rand() % i;
	int t2 = rand() % i;

	while (t1 == t2)
	{
		t2 = rand() % i;
	}
	
	int ss = 0;
	u64 d1, v1, d2, v2;
	map<u64, u64>::iterator iter = mValue.begin();
	for (; iter != mValue.end(); iter++)
	{
		if (ss == t1) 
		{
			v1 = (*iter).first;
			d1 = (*iter).second;
		}
		
		if (ss == t2) 
		{
			v2 = (*iter).first;
			d2 = (*iter).second;
		}
	
		ss++;
	}

	OmnString reqid = "swpcmol";
	OmnString args;
		args << "container_docid=" << mCtrName << ",v1=" << v1 << ",d1=" << d1 
			 << ",v2=" << v2 << ",d2=" << d2 << ",ordername=" << mOrdName;
	u32 siteid = mThread->getSiteid();

	OmnString rslt;
	AosSengAdmin::getSelf()->manualOrder(siteid, reqid, args, rslt);
OmnScreen << "**********rslt*************" << rslt << endl;	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(rslt, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	AosXmlTagPtr record = child->getNextChild("record");
	while (record)
	{
		u64 vl = record->getAttrU64("value", 0);
		u64 dd = record->getAttrU64("did", 0);


		if (dd == d1)
		{
			if (vl != v2) 
			{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
				return false;
			}
		}
		
		if (dd == d2)
		{
			if (vl != v1) 
			{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
				return false;
			}
		}
	
		record = child->getNextChild("record");
	}
	
	mValue[v1] = d2;
	mValue[v2] = d1;

	showRslt();
	return true;
}


bool
AosManualOrderTester::move()
{
	if (mValue.size()<2) return true;

	int i = mValue.size();
	int t1 = rand() % i;
	int t2 = rand() % i;

	while (t1 == t2)
	{
		t1 = rand() % i;
	}

	int ss = 0;
	u64 d1, v1, d2, v2;
	map<u64, u64>::iterator iter = mValue.begin();
	for (; iter != mValue.end(); iter++)
	{
		if (ss == t1) 
		{
			v1 = (*iter).first;
			d1 = (*iter).second;
		}
		
		if (ss == t2) 
		{
			v2 = (*iter).first;
			d2 = (*iter).second;
		}
	
		ss++;
	}

	u64 value1;
	u64 value2;
	const OmnString flag = "before";
	OmnString reqid = "mvcmol";
	OmnString args;
		args << "container_docid=" << mCtrName << ",v1=" << v1 << ",d1=" << d1 
			 << ",v2=" << v2 << ",d2=" << d2 << ",ordername=" << mOrdName << ",flag=" << flag;
	u32 siteid = mThread->getSiteid();

	OmnString rslt;
	AosSengAdmin::getSelf()->manualOrder(siteid, reqid, args, rslt);
OmnScreen << "**********rslt*************" << rslt << endl;	
	
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(rslt, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	AosXmlTagPtr record = child->getNextChild("record");
	while (record)
	{
		u64 vl = record->getAttrU64("value", 0);
		u64 dd = record->getAttrU64("did", 0);


		if (dd == d1)
		{
			value1 = vl;
		}
		
		if (dd == d2)
		{
			value2 = vl;
		}
	
		record = child->getNextChild("record");
	}

	// Test
	if (flag == "before")
	{
		u64 temp1 = v1;
		bool flag = false;
		getNext(v1, flag);

		u64 value3;
		if (flag)
		{
			value3 = v1;	
		}
		else
		{
			value3 = (temp1 + v1) / 2;
		}
		if (value3 != value1) 
		{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
			return false;
		}
		if (value2 != temp1) 
		{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
			return false;
		}
	
		// Change map
		mValue.erase(v2);
		mValue[value1] = d1;
		mValue[value2] = d2;
	}

	if (flag == "after")
	{
		u64 temp1 = v1;
		bool flag = false;
		getNext(v1, flag);
	
		u64 value3;
		if (flag)
		{
			value3 = v1;
		}
		else
		{
			value3 = (temp1 + v1) / 2;
		}
		
		if (value3 != value2) 
		{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
			return false;
		}
		if (temp1 != value1)
		{
OmnScreen << "*********************************Error*************************" << ++sgnum << endl; 
			return false;
		}

		// Change Map
		mValue.erase(value2);
		mValue[value2] = d2;
	}

	showRslt();

	return true;
}


bool
AosManualOrderTester::del()
{
	if (mValue.size()<1) return true;

	int i = mValue.size();
	int t1 = rand() % i;

	int ss = 0;
	u64 d1, v1;
	map<u64, u64>::iterator iter = mValue.begin();
	for (; iter != mValue.end(); iter++)
	{
		if (ss == t1) 
		{
			v1 = (*iter).first;
			d1 = (*iter).second;
			break;
		}
		ss++;
	}

	//swpcmol,mvcmol,rmcmol
	OmnString reqid = "rmcmol";
	OmnString args;
		args << "container_docid=" << mCtrName << ",v1=" << v1 << ",d1=" << d1 
			 << ",ordername=" << mOrdName;
	u32 siteid = mThread->getSiteid();

	OmnString rslt;
	AosSengAdmin::getSelf()->manualOrder(siteid, reqid, args, rslt);
	
	mValue.erase(v1);

	showRslt();
	return true;
}

void
AosManualOrderTester::getNext(u64 &v, bool &flag)
{
	map<u64, u64>::iterator iter = mValue.find(v);
	aos_assert(iter != mValue.end());
	if (++iter == mValue.end())
	{
		v = v + (((u64)1) << 32);
		flag = true;
		return;
	}

	v = (*iter).first;
	flag = false;

	return;
}

void
AosManualOrderTester::showRslt()
{
	map<u64, u64>::iterator iter = mValue.begin();
	while (iter != mValue.end())
	{
		OmnScreen << (*iter).first << ":::" << (*iter).second << endl;
		iter++;
	}
}
