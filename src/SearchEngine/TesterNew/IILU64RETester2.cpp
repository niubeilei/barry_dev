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
// 1. iilid % 4 == 0: value and docid are not unique
// 2. iilid % 4 == 1: value not unique, docid unique
// 3. iilid % 4 == 2: value and docid unique
// 4. iilid % 4 == 3: value and docid are not unique
//
// sgNumDocs[eMaxIILs]:  keep the number of docs for all IILs
// sgIILNames[eMaxIILs]: keep all the IIL names
//
//
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/TesterNew/IILU64RETester2.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IIL.h"
#include "IILClient/IILClient.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"

#define PERCENT(n)   (rand()%100 < n)

const OmnString sgSiteid = "100";
const int sgMaxTorIILs = 6;
const int sgMaxIILDocs = 1000000;
static u64 *		sgValues;
static u64			sgDocids[sgMaxIILDocs];
static u64 *		sgValues2;
static u64			sgDocids2[sgMaxIILDocs];

static u32			sgNumDocs[sgMaxTorIILs];
static OmnString 	sgIILNames[sgMaxTorIILs];
static u64			sgIILIDs[sgMaxTorIILs];
static int			sgNumIILs = 0;
static u32			sgLongestIIL = 0;
const int sgMaxStrLen = 20;
const int sgMaxCheckedIILs = 5;
const int sgMaxIILLen = 2000000; //const int sgMaxIILLen = 20000;
static int 			sgIi;
static int 			sgOpr;
// 'sgIILValues' is an array of array of fixed length (AosIILUtil::eMaxStrValueLen+2) 
// char strings:
// 	sgIILValues[sgMaxCheckedIILs][sgMaxIILLen][AosIILUtil::eMaxStrValueLen+2]
static u64 *		sgIILValues[sgMaxCheckedIILs];
static u64			sgIILDocids[sgMaxCheckedIILs][sgMaxIILLen];
const int sgVerifyIILstartpos = 50000; // Starting from the current value of the verification

AosIILU64RETester2::AosIILU64RETester2()
:
mNumDocs(0),
mVerifyGetObjidWeight(100),
mTotal(0),
mAddOpr(0),
mDelOpr(0),
mModOpr(0),
mVerifyOpr(0)
{
	mName = "DocServerTester";

	int b = 0;
	int c = 0;
	int d = 0;
	// Initialize sgValues
	sgValues = new u64[sgMaxIILDocs];
	for (int i=0; i<sgMaxIILDocs; i++)
	{
		sgValues[i] = 0;
		b+=AosIILUtil::eMaxStrValueLen+2;	
	}

	sgValues2 = new u64[sgMaxIILDocs];
	for (int i=0; i<sgMaxIILDocs; i++)
	{
		sgValues2[i] = 0;
		b+=AosIILUtil::eMaxStrValueLen+2;	
	}

	for (int i=0; i<sgMaxTorIILs; i++)
	{
		sgNumDocs[i] = 0;
	}

	for (int i=0; i<sgMaxCheckedIILs; i++)
	{
		sgIILValues[i] = new u64[sgMaxIILLen];
		c+=sgMaxIILLen;

		for (int k=0; k<sgMaxIILLen; k++)
		{
			sgIILDocids[i][k] = 0;
			sgIILValues[i][k] = 0;
			d += AosIILUtil::eMaxStrValueLen+2;
		}
	}
}


bool AosIILU64RETester2::start()
{
	mRunData =  OmnApp::getRundata();
	cout << "    Start OmnString Tester ..." << endl;
	// basicTest();
	initIILName();
	torture();
	return true;
}


bool AosIILU64RETester2::basicTest()
{
	return true;
}


bool
AosIILU64RETester2::torture()
{
	srand(8888);
	int tries = 100000000;
	for (int i=0; i<tries; i++)
	{
OmnScreen<<"tries:"<<i << endl;
		int addnum = rand()%99000 + 100;
OmnScreen << "addEntry:" << addnum << endl;
		for(int j=0; j<=addnum; j++)
		{
			aos_assert_r(addEntry1(), false);
		}

		u32 total = sgNumDocs[0];
		if (PERCENT(10) || total == 0)
		{
			//remove all Entry
OmnScreen <<"deleteEntry: " << total << endl;
			for (u32 j = 0;j<total; j++)
			{
				aos_assert_r(deleteEntry(), false);
			}
		}
		else
		{
			//remove 0-(total-1) entry
			u32 num = rand()%total;
OmnScreen <<"deleteEntry: " << num << endl;
			for (u32 j=0; j<num; j++)
			{
				aos_assert_r(deleteEntry(), false);
			}
		}
	}
	return true;
}


bool
AosIILU64RETester2::initIILName()
{
	while(mIILName1 == "" || mIILName2 == "" ||mIILName1 == mIILName2)
	{
		int len = (rand() % 20) + 5;
		char data[40];
		AosRandomLetterStr(len, data);
		mIILName1 = data;
	
		
		len = (rand() % 20) + 5;
		AosRandomLetterStr(len, data);
		mIILName2 = data;
	}
	return true;
}



int
AosIILU64RETester2::findValue(
		const u64 &value, 
		u64* values, 
//		const u64 *docids,
		const int num_values, 
		bool &found)
{
	// It uses the binary search to check whether 'value' is in 
	// the array 'values'. If not, it returns 'idx' in front of which 
	// the value should be inserted. 
	if(num_values == 0)
	{
		found = false;
		return -1;
	}
	
	int left = 0;
	int right = num_values-1;
	found = false;
	int cur = 0;
	while (right - left > 1)
    {
        cur = (right + left)/2;
		int rslt = values[cur] - value;
		if (rslt == 0)
		{
			found = true;

			// Need to find the first one that equals to 'value'
			if (cur == 0)
			{
				// It is the first entry. Return it.
				return cur;
			}

			// Search backwards until it finds one that does not equal to 'value'
			while(1)
			{
				if(cur <= 0) break;
				rslt = values[cur - 1] - value;
				if (rslt != 0)
				{
					break;
				}				
				cur --;
			}
			return cur;
		}

		if (rslt < 0)
		{
			// 'values[cur]' < value. 
			left = cur;
		}
		else
		{
			right = cur;
		}
	}

	int rslt = values[left] - value;
	if(rslt == 0)
	{
		found = true;
		return left;
	}
	else if(rslt > 0)
	{
		return left;
	}
	else // rslt < 0
	{
		int rslt2 = values[right] - value;
		if(rslt2 == 0)
		{
			found = true;
			return right;
		}
		else if(rslt2 > 0)
		{
			return right;
		}
		else // rslt < 0	
		{
			return right +1;
		}
	}
	return -1;
}

int 
AosIILU64RETester2::compareValueDocid(const u64 str1,
								   const u64 docid1,
								   const u64 str2,
								   const u64 docid2)
{
	int rslt = str1 - str2;
	if(rslt != 0)
	{
		return rslt;
	}
	return docid1 - docid2; 
}

int
AosIILU64RETester2::findValueDocid(
		const u64 &value,
		const u64 &docid, 
		u64* values, 
		const u64 *docids,
		const int num_values, 
		bool &found)
{
	// It uses the binary search to check whether 'value' is in 
	// the array 'values'. If not, it returns 'idx' in front of which 
	// the value should be inserted. 
	if(num_values == 0)
	{
		found = false;
		return -1;
	}
	
	int left = 0;
	int right = num_values-1;
	found = false;
	int cur = 0;
	while (right - left > 1)
    {
        cur = (right + left)/2;
		int rslt = compareValueDocid(values[cur], docids[cur] , value,docid);
		if (rslt == 0)
		{
			found = true;

			// Need to find the first one that equals to 'value'
			if (cur == 0)
			{
				// It is the first entry. Return it.
				return cur;
			}

			// Search backwards until it finds one that does not equal to 'value'
			while(1)
			{
				if(cur <= 0) break;
				rslt = compareValueDocid(values[cur-1], docids[cur-1] , value,docid);
				if (rslt != 0)
				{
					break;
				}				
				cur --;
			}
			return cur;
		}

		if (rslt < 0)
		{
			// 'values[cur]' < value. 
			left = cur;
		}
		else
		{
			right = cur;
		}
	}

	int rslt = compareValueDocid(values[left], docids[left] , value,docid);
	if(rslt == 0)
	{
		found = true;
		return left;
	}
	else if(rslt > 0)
	{
		return left;
	}
	else // rslt < 0
	{
		int rslt2 = compareValueDocid(values[right], docids[right] , value,docid);
		if(rslt2 == 0)
		{
			found = true;
			return right;
		}
		else if(rslt2 > 0)
		{
			return right;
		}
		else // rslt < 0	
		{
			return right +1;
		}
	}
	return -1;
}


bool
AosIILU64RETester2::addEntry1()
{
	// This function randomly picks an IIL, and then adds an entry
	// to that IIL. 
	
	bool value_unique = false;
	bool docid_unique = false;
//	getUnique(iilid, value_unique, docid_unique);

	// 3. Generate a value. 
	u64 value;
	if (value_unique)
	{
		// generate a value which is not in the iil
		while (1)
		{
			value = rand();

			// Check whether the value is used in the list.
			// need to read iilstr
			bool found;
			findValue(value, sgValues, mNumDocs, found);
			if (!found)
			{
				break;
			}
		}
	}
	else
	{
		// some rate we pick value from iil, otherwise random generate string
		int r1 = rand() % 100;
		if(r1 >= 50 || mNumDocs == 0)
		{
			// random pick a string, but it may be in the list
			value = rand();
		}
		else
		{
			// pick one value from the list
			int seqno1 = rand() % mNumDocs;
			value = sgValues[seqno1];
		}
	}

	// Generate docid
	u64 docid = 0;
	int r1 = rand() % 100;
	if(r1 >= 20 || mNumDocs == 0)
	{
		docid = rand();			
	}
	else
	{
		// pick one value from the list
		int seqno1 = rand() % mNumDocs;
		docid = sgDocids[seqno1];
	}

	// Copy the data again
	bool rslt = AosIILClient::getSelf()->addU64ValueDoc(mIILName1, value, false, docid, value_unique, docid_unique, mRunData);
	if(!rslt)
	{
		OmnAlarm << "fail to add doc: " << docid << ":" << value<< enderr;
		return false;
	}
	
	rslt = AosIILClient::getSelf()->addU64ValueDoc(mIILName2, value, false, docid, value_unique, docid_unique, mRunData);
	if(!rslt)
	{
		OmnAlarm << "fail to add doc: " << docid << ":" << value<< enderr;
		return false;
	}

	addValueToArray(value, docid, sgIILValues[0], sgIILDocids[0], sgNumDocs[0]);
	sgNumDocs[0] ++;

	return true;
}

/*
bool
AosIILU64RETester2::addEntry()
{
	// This function randomly picks an IIL, and then adds an entry
	// to that IIL. 

	// 1. Pick an IIL
	AosIILU64Ptr iil;
	int iilpos = 0;

//	u64 iilid = iil->getIILID();
//	iilid = iilid-5001;
	
	//sgNumDocs[iilpos] >sgMaxIILLen-1 	not add Entry
	if (sgNumDocs[iilpos] >= (u32)sgMaxIILLen-1) 
	{
		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		return true;
	}

	// 2. Determine value uniqueness
	bool value_unique;
	bool docid_unique;
	getUnique(iilid, value_unique, docid_unique);

	// 3. Copy the data
	iil->copyData(
		sgValues, 
		sgDocids, 
		(int)AOSSENG_TORTURER_MAX_DOCS, 
		mNumDocs,
		mRunData);
	
//	// verify before add
//	if (iilpos < sgMaxCheckedIILs)
//	{
//		// It is an IIL whose values are kept in memory	
//		bool rslt = compareValueDocid(iilid, mNumDocs, iilpos);
//		if (!rslt)
//		{
//			OmnAlarm << "Failed to verify entries " << iilid << ":" <<sgOpr << enderr;
//			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
//			return false;
//		}
//	}

	// 3. Generate a value. 
	OmnString value;
	if (value_unique)
	{
		// generate a value which is not in the iil
		while (1)
		{
			value = rand();

			// Check whether the value is used in the list.
			// need to read iilstr
			bool found;
			findValue(value, sgValues, mNumDocs, found);
			if (!found)
			{
				break;
			}
		}
	}
	else
	{
		// some rate we pick value from iil, otherwise random generate string
		int r1 = rand() % 100;
		if(r1 >= 50 || mNumDocs == 0)
		{
			// random pick a string, but it may be in the list
			value = rand();
		}
		else
		{
			// pick one value from the list
			int seqno1 = rand() % mNumDocs;
			value = sgValues[seqno1];
		}
	}

	// Generate docid
	u64 docid = 0;
	if (docid_unique)
	{
		while (1)
		{
			docid = rand();
			if (!doesDocidExist(iil, value, docid)) break;
		}
	}
	else
	{
		//1. 20% we get the docid from existing record, otherwise we randomly generate a docid
		int r1 = rand() % 100;
		if(r1 >= 20 || mNumDocs == 0)
		{
			docid = rand();			
		}
		else
		{
			// pick one value from the list
			int seqno1 = rand() % mNumDocs;
			docid = sgDocids[seqno1];
		}
				
	}
	
	// Copy the data again
	bool rslt = iil->addDocSafe(value, docid, value_unique, docid_unique, false,mRunData);
	if(!rslt)
	{
		OmnAlarm << "fail to add doc: " << docid << ":" << value<< enderr;
		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		return false;
	}
	sgNumDocs[iilpos]++;
//	if (sgNumDocs[iilpos] > sgLongestIIL) sgLongestIIL = sgNumDocs[iilpos];

	if (iilpos < sgMaxCheckedIILs)
	{
		// Need to add the value and docid into the array.
		rslt = addValueToArray(value, docid, sgIILValues[iilpos], sgIILDocids[iilpos], mNumDocs);
		if(!rslt)
		{
			OmnAlarm << "fail to add value to array: " << docid << ":" << value<< enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}

//		iil->copyData(sgValues2, sgDocids2, (int)AOSSENG_TORTURER_MAX_DOCS, mNumDocs);
//		rslt = compareValueDocid2(iilid, mNumDocs, iilpos);
//		if (!rslt)
//		{
//			OmnAlarm << "Failed to verify entries " << iilid << ":" << sgOpr<< enderr;
//			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
//			return false;
//		}
	}

//	rslt = verifyIIL(iil, iilpos);
//	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
//	aos_assert_r(rslt, false);
	return true;
}
*/

bool
AosIILU64RETester2::verifyIIL(const AosIILU64Ptr &iil, const int iilpos)
{
	//u64 iilid = iil->getIILID()-5001;
	int num_docs = sgNumDocs[iilpos];

	if (iilpos < sgMaxCheckedIILs)
	{
		aos_assert_r(verifyIIL(num_docs, iil, sgIILValues[iilpos], sgIILDocids[iilpos]), false);
	}
	else
	{
		copyData(
			mIILName1,
			sgValues, 
			sgDocids, 
			(int)AOSSENG_TORTURER_MAX_DOCS, 
			mNumDocs,
			mRunData);
		aos_assert_r(verifyIIL(num_docs, iil, sgValues, sgDocids), false);
	}
	return true;
}


bool
AosIILU64RETester2::doesDocidExist(
		const AosIILU64Ptr &iil, 
		const u64 &value,
		const u64 &docid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosIILU64RETester2::deleteEntry()
{
	// Pick an existing one. 
	// 1. Pick an IIL
	// if (sgNumIILs <= 0) return true;

	//bool value_unique = false;
	//bool docid_unique = false;
	int iilpos = 0;
	
	bool rslt;
	u32 num_docs = sgNumDocs[iilpos];

	// Copy the IIL data
	
	copyData(
		mIILName1,
		sgValues, 
		sgDocids,
		(int)AOSSENG_TORTURER_MAX_DOCS, 
		mNumDocs,
		mRunData);
	//delete <10000 verify(1)(2)
//	if (iilpos < sgMaxCheckedIILs)
//	{
	
//		rslt = compareValueDocid(iilid, mNumDocs, iilpos);
//		if (!rslt)
//		{
//			OmnAlarm << "Failed to verify entries " << iilid  <<":"<< value_unique << ":" << docid_unique <<":" << mNumDocs <<":"<< sgNumDocs[iilpos] << ":" << mNumDocs << ", i:" << sgIi<< enderr;
//			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
//			return false;
//		}
//	}
	
	
	// Determine whether to remove a valid entry or an invalid entry
	// now we only remove the right one
	if (num_docs == 0)  return true;//valid_entry = false;
	bool valid_entry = (rand() % 100) < 100;
	u64 value;
	u64 docid;
	int idx = 0;
	if (valid_entry)
	{
		idx = rand() % num_docs;
		value = sgValues[idx];
		docid = sgDocids[idx];
	}
	else
	{
		// there are 2 ways to gen invalid data
		// 1. wrong value
		// 2. wrong docid
		bool use_way_1 = PERCENT(50);
		if(use_way_1)
		{
			while (1)
			{
				value = rand();

				if(PERCENT(50))
				{
					docid = rand();
				}
				else
				{
					int num = rand() % num_docs;
					docid = sgDocids[num];  
				}
				
				bool found;
				findValueDocid(value, docid, sgValues, sgDocids, num_docs, found);
				if (!found)
				{
					// The value is not in the list. 
					break;
				}
			}
		}
		else// use way 2
		{
			//1. pick one value
			while (1)
			{
				int num = rand() % num_docs;
				value = sgValues[num];
				if(PERCENT(50))
				{
					docid = rand();
				}
				else
				{
					int num = rand() % num_docs;
					docid = sgDocids[num];  
				}
				bool found = false;
				findValueDocid(value, docid, sgValues, sgDocids, num_docs, found);
				if (!found)
				{
					// The value is not in the list. 
					break;
				}
			}
		}
		// Generate a value that is not in the list
	}

	// Remove the entry
	//delete (3)

//	rslt = iil->removeDocSafe(value, docid,mRunData);
	rslt = AosIILClient::getSelf()->removeU64ValueDoc(
				mIILName1, 
				value, 
				docid, 
				mRunData);

	bool rslt2 = AosIILClient::getSelf()->removeU64ValueDoc(
				mIILName2, 
				value, 
				docid, 
				mRunData);

	aos_assert_r(rslt == rslt2, false);

	bool flag = false;
	if(valid_entry)
	{
		aos_assert_r(rslt, false);
		sgNumDocs[iilpos]--;
		if (iilpos < sgMaxCheckedIILs)
		{
			// Need to remove the value and docid from the array.
			rslt = removeValueFromArray(value, docid, 
					sgIILValues[iilpos], sgIILDocids[iilpos], mNumDocs);
			aos_assert_r(rslt, false);

			flag = AosIIL::isIILMerged();
			//if (sgNumDocs[iilpos]==0) and flag ==1 //Linda
			if (sgNumDocs[iilpos]==0 && flag) 
			{
				copyData(
					mIILName1,
					sgValues2,
					sgDocids2,
					(int)AOSSENG_TORTURER_MAX_DOCS,
					mNumDocs,
					mRunData);
				rslt = compareValueDocid2(0, mNumDocs, iilpos);
				if (!rslt)
				{
					OmnAlarm << "Failed to verify entries " << enderr;
					//AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
					return false;
				}
			}
		}
	}
	else
	{
		aos_assert_r(!rslt, false);
	}

	//if (sgNumDocs[iilpos]==0) and flag ==1 //Linda
	if (sgNumDocs[iilpos]==0 || flag)
	{
		copyData(
			mIILName1,
			sgValues2,					sgDocids2,
			(int)AOSSENG_TORTURER_MAX_DOCS,
			mNumDocs,
			mRunData);
		rslt = compareValueDocid2(0, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << enderr;
			//AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
//		aos_assert_r(verifyIIL(0, iilpos), false);
//		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	}
	return true;
}

bool
AosIILU64RETester2::getUnique(u64 iilid, bool &value_unique, bool &docid_unique)
{
	//iilid % 4 == 0: value and docid are not unique
	//iilid % 4 == 1: value not unique, docid unique
	//iilid % 4 == 2: value unique, docid is not unique
	//iilid % 4 == 3: value and docid are unique
	int value = iilid % 4;
	switch(value)
	{
		case 0: 
			value_unique = false;
			docid_unique = false;
			break;
		case 1: 
			value_unique = false;
			docid_unique = true;
			break;
		case 2: 
			value_unique = true;
			docid_unique = false;
			break;
		case 3: 
			value_unique = true;
			docid_unique = true;
			break;

		default:
			break;

	}
	return true;
}

/*
bool
AosIILU64RETester2::copyData1(const OmnString &iilname)
{
	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILU64 *)iil.getPtr();
	int max_doc_num = AOSSENG_TORTURER_MAX_DOCS;
	aos_assert_r(mStriil->copyData(
							sgValues,
	 						sgDocids,
	 						max_doc_num,
	 						mNumDocs,
							mRunData), false);
	return true;
}
*/

bool
AosIILU64RETester2::verifyNextDocidSafe(
		const bool reverse, 
		const bool value_unique) 
{
	OmnScreen << "Total Docs: " << mNumDocs << " ... " << endl;
	int group = 0;
	u64 prev_value;
	for (int aa=0; aa<mNumDocs; aa++)
	{
		if (group++ == 1000)
		{
			cout << ":" << aa << flush;
			group = 0;
		}

		// 1. Preparing
		int idx = -10;
		int iilidx = -10;
		u64 value = sgValues[aa];
		u64 docid;
		bool isunique;

		if (value_unique)
		{
			// 2. Retrieving the doc
			bool rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
				eAosOpr_eq, value, docid, isunique,mRunData);
			aos_assert_r(rslt, false);

			if (docid != sgDocids[aa]) 
			{
				OmnAlarm << "Failed: " << aa << ":" << value << ":" 
					<< docid << ":" << sgDocids[aa] << enderr;
				return false;
			}
		}
		else
		{
			// If 'value_unique' is not true, it is possible that 
			// the value is duplicated. It needs to repeatedly call 
			// nextDocidSafe(...) until it finds the expected docid.
			while (1)
			{
				// Retrieve the docid
				bool rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
					eAosOpr_eq, value, docid, isunique,mRunData);
				aos_assert_r(rslt, false);

				if (docid == AOS_INVDID)
				{
					// It failed the testing.
					aos_assert_r(idx == -5, false);
					aos_assert_r(iilidx == -5, false);
					OmnAlarm << "Failed: " << aa << ":" << value << ":" 
						<< docid << enderr;
					return false;
				}

				if (docid == sgDocids[aa]) break;
			}	
		}

		u64 vv = mStriil->getValueSafe(idx, iilidx, mRunData);
		prev_value = vv;
	}

	OmnScreen << "Total Docs: " << mNumDocs << " ... Done!" << endl;
	return true;
}


AosOpr
AosIILU64RETester2::pickOperator()
{
	switch (rand() % 6)
	{
	case 0: return eAosOpr_gt;
	case 1: return eAosOpr_ge;
	case 2: return eAosOpr_eq;
	case 3: return eAosOpr_lt;
	case 4: return eAosOpr_le;
	default: break;
	}
	return eAosOpr_ne;
}


bool
AosIILU64RETester2::tortureNextDocidSafe(
		const int tries,
		const bool vunique, 
		const bool dunique, 
		u64 *values, 
		const u64 *docids, 
		const int num_docs)
{
	// This function randomly tests the given list
	if (num_docs == 0) return true;
	aos_assert_r(num_docs > 0, false);
	int entry = 0;
	for (int i=0; i<tries; i++)
	{
		// 2. Determine the operator
		AosOpr opr = pickOperator();
		// 3. Determine reverse
		bool reverse = rand() % 2;

		bool value_valid = false;
		u64 value;
		u64 docid = 0;
if (sgIi ==18 && i ==7)
OmnMark;

		// 4. Determine whether to look up an existing value
		if (PERCENT(20))
		{
			while(1)
			{
				// Will randomly pick a word
				value = rand();
				
				bool found = true;
				// Check whether the value is in the list
				findValue(value, values, num_docs, found);
				if (!found)
				{
					// The value is not in the list.
					break;
				}
			}
			value_valid = false;
		}
		else
		{
			// 1. Determine the entry 
			entry = rand() % num_docs;
			value = values[entry];
			docid = docids[entry];
			value_valid = true;
		}

		bool rslt;
		switch (opr)
		{
		case eAosOpr_ge:   // >=
			 rslt = searchGE(value, reverse, vunique, dunique, docid, eAosOpr_ge, value_valid);
			 break;

		case eAosOpr_eq:  //==
			 rslt = searchEQ(value, reverse, vunique, dunique, docid);
			 break;
				 
		case eAosOpr_gt: // >
			 //rslt = searchGE(value, reverse, vunique, dunique, docid, eAosOpr_gt);
			 if (value_valid)
				 rslt = true;
			 else
				 rslt = false;
			 break;

		case eAosOpr_lt: // <
			 //rslt = searchLE(value, reverse, vunique, dunique, docid, eAosOpr_lt);
			 if (value_valid)
				 rslt = true;
			 else
				 rslt = false;
			 break;

		case eAosOpr_le: //<=
			 rslt = searchLE(value, reverse, vunique, dunique, docid, eAosOpr_le, value_valid);
			 break;

		case eAosOpr_ne: //!=
			 //rslt = searchLE(value, reverse, vunique, dunique, docid);
			 if (value_valid)
				 rslt = true;
			 else
				 rslt = false;
			 break;

		default:
			 OmnAlarm << "Invalid operator: " << opr << enderr;
			 rslt = false;
			 break;
		}

		if (value_valid)
		{
			if (!rslt)
			{
				OmnAlarm << "Failed: " << ":" << value << ":" 
					<< docid << enderr;
			}
			aos_assert_r(rslt, false);
		}
		else
		{
			aos_assert_r(!rslt, false);
		}
	}

	return true;
}


bool
AosIILU64RETester2::searchGE(
		const u64 &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	bool rslt;
	if (reverse)
	{
		rslt = searchReverseGE(value, 
			value_unique, docid_unique, expected_docid, opr, value_valid);
		return rslt;
	}
	
	rslt = searchNormalGE(value, 
			value_unique, docid_unique, expected_docid, opr, value_valid);
	return rslt;
}


bool
AosIILU64RETester2::searchReverseGE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64RETester2::searchNormalGE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64RETester2::searchEQ(
		const u64 &value, 
		const bool reverse,
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
{
	OmnShouldNeverComeHere;
	return false;
}



bool
AosIILU64RETester2::searchLE(
		const u64 &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	bool rslt;
	if (reverse)
	{
		rslt = searchReverseLE(value, 
			value_unique, docid_unique, expected_docid, opr, value_valid);
		return rslt;
	}
	
	rslt = searchNormalLE(value, 
			value_unique, docid_unique, expected_docid, opr, value_valid);
	return rslt;
}


bool
AosIILU64RETester2::searchNormalLE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64RETester2::searchReverseLE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64RETester2::addValueToArray(
		const u64 &value, 
		const u64 &docid,
		u64*	values,
		u64* docids,
		const int num_docs)
{
	bool found;
	int idx = findValueDocid(value, docid, values, docids, num_docs, found);
	//Linda start
	if (idx ==-1) idx = 0;
	else 
	aos_assert_r(idx >= 0, false);  
	//end
	for(int i = num_docs; i > idx; i--)
	{
		values[i] = values[i-1];
		docids[i] = docids[i-1];
	}
	
	values[idx] = value;
	docids[idx] = docid;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool
AosIILU64RETester2::removeValueFromArray(
		const u64 &value, 
		const u64 &docid,
		u64*	values,
		u64* docids,
		const int num_docs)
{
	aos_assert_r(num_docs > 0, false);
	bool found;
	int idx = findValueDocid(value, docid,values,docids,num_docs,found);
	//Linda start
	if (idx ==-1) idx = 0;
	else 
	aos_assert_r(idx >= 0, false);  
	//end
	for(int i = idx; i <num_docs -1; i++)
	{
		values[i] = values[i+1];
		docids[i] = docids[i+1];
	}
	
	values[num_docs -1] = 0;
	docids[num_docs -1] = 0;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool 
AosIILU64RETester2::modifyValueFromArray(
		const u64 &oldvalue, 
		const u64 &olddocid,
		const u64 &newvalue, 
		const u64 &newdocid,
		u64*	values,
		u64* docids,
		const int num_docs)
{
	bool rslt = false;
	rslt = removeValueFromArray(oldvalue,
								olddocid,
								values,
								docids,
								num_docs);
	aos_assert_r(rslt,false);
	rslt = addValueToArray(newvalue,
								newdocid,
								values,
								docids,
								num_docs-1);
	return rslt;	
}


bool
AosIILU64RETester2::verifyIIL(
		const u32 num_docs,
		const AosIILU64Ptr &iil, 
		u64* values, 
		const u64 *docids)
{
	//Skip verify IIL
	//if(sgIi >= sgVerifyIILstartpos)
	//{
		aos_assert_r(verifyEntries(num_docs, iil, values, docids), false);
	//}

//	u64 iilid = iil->getIILID()-5001;
//	bool value_unique = false;
//	bool docid_unique = false;
//	getUnique(iilid, value_unique, docid_unique);
//	int tries = rand() % 10;
//	mStriil = NULL;
//	mStriil = iil;
//	aos_assert_r(tortureNextDocidSafe(tries, 
//		value_unique, docid_unique, values, docids, num_docs), false);
	return true;
}

bool
AosIILU64RETester2::verifyEntries(
		const u32 num_docs,
		const AosIILU64Ptr &iil, 
		u64* values, 
		const u64 *docids)
{
	// This function does the following verification:
	// 1. Sequentially verify entries, one by one, in the normal order
	// 2. Sequentially verify entries, one by one, in the reversed order
	// 3. Randomly pick some entries and verify it.
	
	if (num_docs == 0) return true;
	// 1. Sequentially verify entries, one by one, in the normal order
	int idx = -10;
	int iilidx = -10;
	bool isunique;
	u64 value;
	u64 docid;
	
	//if (PERCENT(10))
	//{
		for (int i=0; i<(int)num_docs; i++)
		{
			// bool rs = iil->nextDocidSafe(idx, iilidx, false, 
			// 		eAosOpr_an, value, docid, isunique,mRunData);
			bool rs = iil->nextDocidSafe(idx, iilidx, false, value, docid,mRunData);
			if (!rs)
			{
				OmnAlarm << "Failed to verify entries " << iilidx <<":" 
					<< sgIi <<":" << value <<":" << docid << ":" 
					<< idx << ":" << num_docs << ":" << i << ":" << values[i] 
					<< ":" << docids[i] << ":" << sgOpr<< enderr;
				return false;
			}

			if (values[i] != value)
			{
				OmnAlarm << "Failed to verify entries " << iilidx << ":" 
					<< idx << ":" << i << ":" << 
						values[i] <<":" << value <<":" << docids[i] <<":" << docid 
						<<":" << sgOpr<< enderr;
				return false;
			}

			if (docids[i] != docid)
			{
				OmnAlarm << "Failed to verify entries " << iilidx << ":" 
					<< idx << ":" << i << ":" << 
						values[i] <<":" << value <<":" << docids[i] <<":"
						<< sgOpr << ":"<< docid << enderr;
				return false;
			}
		}

		// 2. Sequentially verify entries, one by one, in the reversed order
		// Verify it in the reversed order
		idx = -10;
		iilidx = -10;
		isunique = false;
		value = 0;
		docid = 0;
		for (int i=(int)num_docs-1; i>=0; i--)
		{
			// bool rs = iil->nextDocidSafe(idx, iilidx, true, 
			// 			eAosOpr_an, value, docid, isunique,mRunData);
			bool rs = iil->nextDocidSafe(idx, iilidx, true, value, docid,mRunData);
			if (!rs)
			{
				OmnAlarm << "Failed to verify entries " << iilidx << ":" <<sgOpr << enderr;
				return false;
			}
		
			if (values[i] !=  value)
			{
				OmnAlarm << "Failed to verify entries " << iilidx <<":" << sgIi << ":" 
					<< value <<":" << docid << ":" << idx << ":" << num_docs << ":" 
					<< i << ":" << values[i] << ":" << docids[i]<< ":" <<sgOpr << enderr;
				return false;
			}
			aos_assert_r(values[i] == value, false);
	
			if (docids[i] != docid)
			{
				OmnAlarm << "Failed to verify entries " << iilidx << ":" << i << ":" << 
						values[i] <<":" << value <<":" << docids[i] <<":" << docid 
						<< ":" <<sgOpr << enderr;
				return false;
			}
			aos_assert_r(docids[i] == docid, false);
		}
	//}

	return true;
}

bool
AosIILU64RETester2::compareValueDocid(const int iilid, const int num_docs, const int iilpos)
{
	// This function assumes that 'iilid' is an in-memory IIL. Its
	// values are stored in sgIILValues[iilid], its docids are stored
	// in sgIILDocids[iilid]. The caller has loaded the iil's value
	// to 'sgValues' and 'sgDocids'. 
	aos_assert_r(num_docs < sgMaxIILLen && num_docs >= 0, false);
	aos_assert_r(iilpos <= sgMaxCheckedIILs, false);
	for (int i = 0; i < num_docs;i++)
	{
		if (sgValues[i] != sgIILValues[iilpos][i])
		{
			OmnAlarm << "Failed to verify entries " << iilpos << ":" << i << ":" << 
					sgValues[i] <<":" << sgIILValues[iilpos][i] <<":" 
					<< sgDocids[i] <<":" << sgIILDocids[iilpos][i] << ":" << sgOpr << enderr;
			return false;
		}

		if (sgDocids[i] != sgIILDocids[iilpos][i])
		{
			OmnAlarm << "Failed to verify entries " << iilpos << ":" << i << ":" << 
					sgValues[i] <<":" << sgIILValues[iilpos][i] <<":" 
					<< sgDocids[i] <<":" << sgIILDocids[iilpos][i] <<":" << sgOpr<< enderr;
			return false;
		}
	}
	return true;
}

bool
AosIILU64RETester2::compareValueDocid2(const int iilid, const int num_docs, const int iilpos)
{
	// This function assumes that 'iilid' is an in-memory IIL. Its
	// values are stored in sgIILValues[iilid], its docids are stored
	// in sgIILDocids[iilid]. The caller has loaded the iil's value
	// to 'sgValues' and 'sgDocids'. 
	aos_assert_r(num_docs < sgMaxIILLen && num_docs >= 0, false);
	aos_assert_r(iilpos <= sgMaxCheckedIILs, false);
	for (int i = 0; i < num_docs;i++)
	{
		if (sgValues2[i] != sgIILValues[iilpos][i])
		{
			OmnAlarm << "Failed to verify entries " << iilpos << ":" << i << ":" << 
					sgValues2[i] <<":" << sgIILValues[iilpos][i] <<":" 
					<< sgDocids2[i] <<":" << sgIILDocids[iilpos][i] << ":" << sgOpr << enderr;
			return false;
		}

		if (sgDocids2[i] != sgIILDocids[iilpos][i])
		{
			OmnAlarm << "Failed to verify entries " << iilpos << ":" << i << ":" << 
					sgValues2[i] <<":" << sgIILValues[iilpos][i] <<":" 
					<< sgDocids2[i] <<":" << sgIILDocids[iilpos][i] <<":" << sgOpr<< enderr;
			return false;
		}
	}
	return true;
}


void
AosIILU64RETester2::toString()
{
	cout << mTotal 
		<< " Add=" << mAddOpr
		<< " Del=" << mDelOpr
		<< " Mod=" << mModOpr
		<< " IILs=" << sgNumIILs 
		<< " Max IIL Length: " << sgLongestIIL << endl;
}


bool
AosIILU64RETester2::copyData(
					 const OmnString &iilname,
					 u64* values, 
					 u64 *docids,
					 const int buff_len, 
					 int &num_docs,
					 const AosRundataPtr &rdata)
{
	vector<u64> values_v;
	vector<u64> docids_v;
	

	AosIILClient::getSelf()->exportIILU64Safe(
						mIILName1,
						values_v,
						docids_v,
						mRunData);
	cout << "=====================" << endl;
	cout << values_v.size() << endl;
	bool finished = false;
	for(int i = 0; i < values_v.size();i++)
	{
		docids[i] = docids_v[i];
		values[i] = values_v[i];
	}
	return true;
}
