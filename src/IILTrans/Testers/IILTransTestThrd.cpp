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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/Testers/IILTransTestThrd.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILClient/IILClient.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "TransUtil/BuffTrans.h"
#include "TransUtil/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "Porting/Sleep.h"


AosIILTransTestThrd::AosIILTransTestThrd(const int &idx, const AosTransServerPtr &server)
:
mGroup(0),
mTransid(0),
mDocid(0),
mNumHitIILs(0),
mNumStrIILs(0),
mStrValue(0),
mNumU64IILs(0),
mU64Value(0),
mTransServer(server)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", idx, true, true, __FILE__, __LINE__);
	mThread->start();
}

	
AosIILTransTestThrd::~AosIILTransTestThrd()
{
}


bool
AosIILTransTestThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter OmnThreadMgr::threadFunc.threadId:" << mThread->getLogicId() << endl;
	
	while (state == OmnThrdStatus::eActive)
	{
		basicTest();
	}
	return true;
}
	

bool
AosIILTransTestThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosIILTransTestThrd::checkThread(OmnString &err, const int thrdLogicId) const
{	
	return true;
}


bool 
AosIILTransTestThrd::basicTest()
{
	AosRundataPtr rdata = OmnApp::getRundata();

	int num_phs = AosIILClient::getSelf()->getNumPhysicals();
	int buffsize = rand() % 100;
	if(buffsize == 0) return true;

static int sgTotalTrans = 0;
	
	sgTotalTrans += buffsize;
	mGroup += buffsize;
	if(mGroup >= 10000)
	{
OmnScreen << "total trans:" << sgTotalTrans << endl;
		OmnSleep(1);
		mGroup -= 10000;
	}

	mDocid++;
	vector<AosBuffPtr> buffs(num_phs);
	for(int i=0; i<num_phs; i++)
	{
		buffs[i] = OmnNew AosBuff(eBuffSize, eBuffIncSize AosMemoryCheckerArgs);
	}
	
	bool rslt;
	for (int i=0; i<buffsize; i++)
	{
		int opr = rand() % 3;
		//int opr = 0;
		switch (opr)
		{
		case 0:
			 rslt = addHitDoc(buffs, rdata);
			 aos_assert_r(rslt, false);
		 	 break;

		case 1:
			 rslt = addStrDoc(buffs, rdata);
			 aos_assert_r(rslt, false);
		 	 break;
		 	 
		case 2:
		 	 rslt = addU64Doc(buffs, rdata);
			 aos_assert_r(rslt, false);
		 	 break;

		case 4:
		 //	 iilname = getU64IILName(ispersis, value_unique, docid_unique);
		 //	 docid = getDocid();
		 //	 parent_docid = rand();
		 //	 total_trans++;
		 //	 rslt = AosIILClient::getSelf()->addAncestor(transid++, buffs, iilname, 
		//		 docid, parent_docid, rdata);
		 	 break;

		case 5:
		 //	 iilname = getDescendantIILName(ancestoriilname);
		 //	 docid = getDocid();
		 //	 parent_docid = rand();
		 //	 total_trans++;
		 //	 rslt = AosIILClient::getSelf()->addDescendant(transid++, buffs, iilname, 
		//		 ancestoriilname, docid, parent_docid, rdata);
		 	 break;
	
		default:
			 break;
		}
	}

	for(int i=0; i<num_phs; i++)
	{
		AosBuffPtr buff = buffs[i];
		AosXmlParser parser;
		OmnString trans_str;
		OmnString buffstr(buff->data(), buff->dataLen());
		trans_str << "<trans><![CDATA[" << buffstr <<  "]]></trans>";
		AosXmlTagPtr xml = parser.parse(trans_str, "" AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		AosBuffTransPtr buffTran = OmnNew AosBuffTrans(xml, buff AosMemoryCheckerArgs);
		mTransServer->procTransBuff(buffTran, rdata);
	}

	return true;
}


bool
AosIILTransTestThrd::addHitDoc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata)
{
	bool isPersis = false;
	OmnString iilname = getHitIILName(isPersis);
	return AosIILClient::getSelf()->addDoc(mTransid++, buffs, iilname, isPersis, mDocid, rdata);
}


OmnString
AosIILTransTestThrd::getHitIILName(bool &isPersis)
{
//	if (mNumHitIILs > 0 && (rand() % 100) > 20)
//	{
//		int idx = rand() % mNumHitIILs;
//		isPersis = mHitPersis[idx];
//		return mHitIILNames[idx];
//	}

	OmnString iilname = "hit_";
	iilname << OmnRandom::nextVarName(5, 20, false);
	isPersis = (rand() % 2);
	if (mNumHitIILs < eMaxHitIILs)
	{
//		mHitIILNames[mNumHitIILs] = iilname;
//		mHitPersis[mNumHitIILs] = isPersis;
		mNumHitIILs++;
	}
	return iilname;
}



bool
AosIILTransTestThrd::addStrDoc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata)
{
	bool isPersis = false;
	bool valueUnique = true;
	bool docidUnique = true;
	OmnString iilname = getStrIILName(isPersis, valueUnique, docidUnique);
	OmnString strvalue = getStrValue(valueUnique);
	u64 docid = mDocid;
	if(docidUnique || (rand() % 2 == 0))
	{
		docid = ++mDocid;
	}
	else
	{
		docid = mDocid;
	}
	
	return AosIILClient::getSelf()->addStrValueDoc(mTransid++, buffs,
			iilname, isPersis, strvalue, docid, valueUnique, docidUnique, rdata);
}


OmnString
AosIILTransTestThrd::getStrIILName(
		bool &isPersis,
		bool &valueUnique,
		bool &docidUnique)
{
//	if (mNumStrIILs > 0 && (rand() % 100) > 20)
//	{
//		int idx = rand() % mNumStrIILs;
//		isPersis = mStrPersis[idx];
//		valueUnique = mStrValueUnique[idx];
//		docidUnique = mStrDocidUnique[idx];
//		return mStrIILNames[idx];
//	}

	OmnString iilname = "str_";
	iilname << OmnRandom::nextVarName(5, 20, false);
	isPersis = (rand() % 2);
	valueUnique = (rand() % 2);
	docidUnique = (rand() % 2);
	if (mNumStrIILs < eMaxStrIILs)
	{
//		mStrIILNames[mNumStrIILs] = iilname;
//		mStrPersis[mNumStrIILs] = isPersis;
//		mStrValueUnique[mNumStrIILs] = valueUnique;
//		mStrDocidUnique[mNumStrIILs] = docidUnique;
		mNumStrIILs++;
	}
	return iilname;
}


OmnString
AosIILTransTestThrd::getStrValue(const bool &valueUnique)
{
	OmnString value = "ken";
	bool createNew = false;
	if(valueUnique || mStrValue == 0) createNew = true;
	if(createNew)
	{
		value << ++mStrValue;
	}
	else
	{
		value << (rand() % mStrValue);
	}
	return value;
}

	
bool
AosIILTransTestThrd::addU64Doc(vector<AosBuffPtr> &buffs, const AosRundataPtr &rdata)
{
	bool isPersis = false;
	bool valueUnique = true;
	bool docidUnique = true;
	OmnString iilname = getU64IILName(isPersis, valueUnique, docidUnique);
	u64	u64value = getU64Value(valueUnique);
	u64 docid = mDocid;
	if(docidUnique || (rand() % 2 == 0))
	{
		docid = ++mDocid;
	}
	else
	{
		docid = mDocid;
	}
	return AosIILClient::getSelf()->addU64ValueDoc(mTransid++, buffs,
			iilname, isPersis, u64value, docid, valueUnique, docidUnique, rdata);
}


OmnString
AosIILTransTestThrd::getU64IILName(
		bool &isPersis,
		bool &valueUnique,
		bool &docidUnique)
{
//	if (mNumU64IILs > 0 && (rand() % 100) > 20)
//	{
//		int idx = rand() % mNumU64IILs;
//		isPersis = mU64Persis[idx];
//		valueUnique = mU64ValueUnique[idx];
//		docidUnique = mU64DocidUnique[idx];
//		return mU64IILNames[idx];
//	}

	OmnString iilname = "u64_";
	iilname << OmnRandom::nextVarName(5, 20, false);
	isPersis = (rand() % 2);
	valueUnique = (rand() % 2);
	docidUnique = (rand() % 2);
	if (mNumU64IILs < eMaxU64IILs)
	{
//		mU64IILNames[mNumU64IILs] = iilname;
//		mU64Persis[mNumU64IILs] = isPersis;
//		mU64ValueUnique[mNumU64IILs] = valueUnique;
//		mU64DocidUnique[mNumU64IILs] = docidUnique;
		mNumU64IILs++;
	}
	return iilname;
}

	
u64
AosIILTransTestThrd::getU64Value(const bool &valueUnique)
{
	if(valueUnique || mU64Value == 0)
	{
		return ++mU64Value;
	}
	
	u64 value = rand() % mU64Value + 1;
	return value;
}

