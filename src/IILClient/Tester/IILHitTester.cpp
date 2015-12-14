////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/07/11	Created by ken Lee	
////////////////////////////////////////////////////////////////////////////
#include "IILClient/Tester/IILHitTester.h"

#include "AppMgr/App.h"
#include "IILClient/IILClient.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

const int sgIILNum = 2000;
const int sgMaxDocs = 10000000;
const int sgMaxStrLen = 80;

static char *       sgValues[sgMaxDocs];
static u64          sgDocids[sgMaxDocs];


AosIILHitTester::AosIILHitTester()
:
mDocid(10000),
mTotal(0),
mAdd(0),
mDel(0),
mCheck(0),
mNumDocs(0)
{
	for (int i=0; i<sgMaxDocs; i++)
	{
		sgDocids[i] = 0;
		sgValues[i] = new char[sgMaxStrLen+2];
	}
}


AosIILHitTester::~AosIILHitTester()
{
}


bool      	
AosIILHitTester::start()
{
	cout << " IIL Hit Test start ..." << endl;
	basicTest();
	return true;
}

bool 
AosIILHitTester::basicTest()
{
	while(mNumDocs < sgMaxDocs)
	{
		mTotal++;
		if(mTotal % 1000 == 0)
		{
			OmnScreen << "mTotal:" << mTotal 
					  << ", mAdd:" << mAdd 
					  << ", mDel:" << mDel
					  << ", mCheck:" << mCheck << endl;
		}
		int opr = rand() % 100;
		if(opr < 50)
		{
//u64 t1 = OmnGetTimestamp();
			aos_assert_r(addDoc(), false);
//OmnScreen << "total time: " << OmnGetTimestamp() - t1 << endl;	
			mAdd++;
		}
		else if(opr < 75)
		{
			aos_assert_r(removeDoc(), false);
			mDel++;
		}
		else
		{
			aos_assert_r(checkDocid(), false);
			mCheck++;
		}
	}
	
	OmnSleep(100000);
	return true;
}


OmnString
AosIILHitTester::createIILName()
{
	int i = rand() % sgIILNum + 1;
	OmnString word = "tester";
	word << i;
	return word;
}


bool 		
AosIILHitTester::addDoc() 
{
	OmnString iilname = createIILName();;

	u64 docid = mDocid;
	AosRundataPtr rdata = OmnApp::getRundata();

//OmnScreen << "addDoc, docid:" << docid << ",word:" << word << endl;

	//bool rslt = AosIILClient::getSelf()->addDoc(iilname, false, docid, rdata);
	bool rslt = false;
	if(!rslt)
	{
		OmnAlarm << "addDoc failed, docid:" << docid << ",iilname:" << iilname << enderr;
		return false;
	}
	strcpy(sgValues[mNumDocs], iilname.data());
	sgDocids[mNumDocs] = mDocid;
	
	mNumDocs++;
	mDocid++;
	return true;
}


bool
AosIILHitTester::removeDoc()
{
	if(mNumDocs == 0) return true;
	int idx = rand() % mNumDocs;
	u64 docid = sgDocids[idx];
	char * word = sgValues[idx];
	AosRundataPtr rdata = OmnApp::getRundata();

//OmnScreen << "remove doc, docid:" << docid << ",word:" << word << ", dix:" << idx << ",mNum:" << mNumDocs << endl;

	bool rslt = false;
	//bool rslt = AosIILClient::getSelf()->removeDoc(word, strlen(word), docid, rdata);
	if(!rslt)
	{
		OmnScreen << "remove doc failed, docid:" << docid << ",word:" << word << ", dix:" << idx << ",mNum:" << mNumDocs << endl;
		return false;
	}
	for(int i = idx; i<mNumDocs; i++)
	{
		sgDocids[i] = sgDocids[i+1];
		strcpy(sgValues[i], sgValues[i+1]);
	}
	sgDocids[mNumDocs-1] = 0;        
	strcpy(sgValues[mNumDocs-1], "");
	mNumDocs--;
	return true;
}


bool
AosIILHitTester::checkDocid()
{
	if(mNumDocs == 0) return true;

	int idx = rand() % mNumDocs;
	u64 docid = sgDocids[idx];
	char * word = sgValues[idx];
	AosRundataPtr rdata = OmnApp::getRundata();

//OmnScreen << "check doc, docid:" << docid << ",word:" << word << ", dix:" << idx << ",mNum:" << mNumDocs << endl;
	
	bool rslt = false;
	// = AosIILClient::getSelf()->docExistSafe(word, strlen(word), docid, rdata);
	if(!rslt)
	{
		OmnScreen << "check doc failed, docid:" << docid << ",word:" << word << ", dix:" << idx << ",mNum:" << mNumDocs << endl;
		return false;
	}
	return true;
}


