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
#include "SearchEngine/TesterNew/IILU64Tester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILU64.h"
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
static u64			sgValues[sgMaxIILDocs];
static u64			sgDocids[sgMaxIILDocs];
static u64			sgValues2[sgMaxIILDocs];
static u64			sgDocids2[sgMaxIILDocs];

static u32			sgNumDocs[sgMaxTorIILs];
static OmnString 	sgIILNames[sgMaxTorIILs];
static u64			sgIILIDs[sgMaxTorIILs];
static int			sgNumIILs = 0;
static u32			sgLongestIIL = 0;
const int sgMaxStrLen = 20;
const int sgMaxCheckedIILs = 6;
const int sgMaxIILLen = 20000; //const int sgMaxIILLen = 20000;
static int 			sgIi;
static int 			sgOpr;
// 'sgIILValues' is an array of array of fixed length (AosIILU64::eDftMaxStrLen+2) 
// char strings:
// 	sgIILValues[sgMaxCheckedIILs][sgMaxIILLen][AosIILU64::eDftMaxStrLen+2]
static u64			sgIILValues[sgMaxCheckedIILs][sgMaxIILLen];
static u64			sgIILDocids[sgMaxCheckedIILs][sgMaxIILLen];
const int sgVerifyIILstartpos = 50000; // Starting from the current value of the verification

AosIILU64Tester::AosIILU64Tester()
:
mVerifyGetObjidWeight(100),
mTotal(0),
mAddOpr(0),
mDelOpr(0),
mModOpr(0),
mVerifyOpr(0)
{
	mName = "DocServerTester";

	for (int i=0; i<sgMaxIILDocs; i++)
	{
		sgValues[i] = 0;
		sgValues2[i] = 0;
		sgDocids[i] = 0;
		sgDocids2[i] = 0;
	}

	for (int i=0; i<sgMaxTorIILs; i++)
	{
		sgNumDocs[i] = 0;
	}

	for (int i=0; i<sgMaxCheckedIILs; i++)
	{
		for (int k=0; k<sgMaxIILLen; k++)
		{
			sgIILDocids[i][k] = 0;
			sgIILValues[i][k] = 0;
		}
	}
}


bool AosIILU64Tester::start()
{
	mRunData =  OmnApp::getRundata();
	cout << "    Start OmnString Tester ..." << endl;
	// basicTest();
	torture();
	return true;
}

bool AosIILU64Tester::basicTest()
{
	/*
	aos_assert_r(verifyObjidDuplicatedEntries(), false);
	aos_assert_r(verifyObjidIIL(), false);
	aos_assert_r(verifyAllIILS(5000, 600000), false);
	
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	aos_assert_r(loadStrIIL(iilname), false);
	aos_assert_r(copyData(mIILName), false);
	aos_assert_r(tortureNextDocidSafe(1000000, true, true, 
		sgValues, sgDocids, mNumDocs), false);
	*/
	return true;
}


bool
AosIILU64Tester::torture()
{
	srand(8888);
	int tries = 100000000;
	int group = 0;
	mTotal = 0;
	mAddOpr = 0;
	mDelOpr = 0;
	mModOpr = 0;
	for (int i=0; i<tries; i++)
	{
		if (++group == 1000)
		{
			mTotal += group;
			toString();
			group = 0;
		}
		sgIi = i;
if(i == 6)
	OmnMark;
		int opr;
		if (PERCENT(80)) opr = eAddEntry;
		else opr = rand() % eOperationMax;
		sgOpr = opr;
		switch (opr)
		{
		case eVerifyGetObjid:
		 	 //aos_assert_r(verifyGetObjid(), false);
			 aos_assert_r(addEntry(), false);
			 mAddOpr++;
		 	 break;
		case eAddEntry:
			 aos_assert_r(addEntry(), false);
			 mAddOpr++;
			 break;

		case eDeleteEntry:
			 aos_assert_r(deleteEntry(), false);
			 mDelOpr++;
			 break;

		case eModifyEntry:
			 aos_assert_r(modifyEntry(), false);
			 mModOpr++;
			 break;

		default:
		 	 OmnAlarm << "Unrecognized operation: " << opr << enderr;
		 	 break;
		}
	}
	return true;
}


AosIILU64Ptr
AosIILU64Tester::pickIIL(int &idx,const bool create_new)
{
	// This function randomly picks an IIL. It can either pick an existing
	// IIL or create a new IIL. 
	idx = -1;
//	if(!create_new && sgNumIILs <= 0 )
//	{
//		return 0;
//	}
	if ((PERCENT(80) && sgNumIILs > 10 && !create_new) || sgNumIILs >= sgMaxTorIILs)
	{
		// Pick an existing one. 
		idx = rand() % sgNumIILs;
		OmnString iilname = sgIILNames[idx];
		u64 iilid = sgIILIDs[idx];

		AosIILType iiltype = eAosIILType_Invalid;
		AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
							iilid,
							iiltype,
							false,
							true,
							mRunData);
//		AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
		aos_assert_r(iil, 0);
		if(iil->getIILType() != eAosIILType_U64)
		{
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return 0;
		}
		AosIILU64Ptr striil = (AosIILU64 *)iil.getPtr();
		return striil;
	}

	// 1. get the length of the random str. 
	// here we have length problem
	int len = (rand() % 20) + 5;
	char data[40];
//	while(1)
//	{
//		AosRandomLetterStr(len, data);
//		// make sure the string is not in the existing list
//		// better we can use a string hash instead of using IILMgr functions
//		AosIILPtr check_iil = AosIILMgr::getSelf()->retrieveIIL(data,len, eAosIILType_U64, false);
//		if(!check_iil)
//		{
//			break;
//		}
//	}	

	// 2. create the iil
	idx = sgNumIILs ++;	
	sgIILNames[idx] = data;
	
	
	AosIILType iiltype = eAosIILType_Str;
	sgIILIDs[idx] = AosIILMgr::getSelf()->createIILPublic(
						iiltype,
						false,
						mRunData);


//	AosIILPtr iil = AosIILClient::getSelf()->getIILPublic(
//			data, true , eAosIILType_Str);

//	AosIILPtr iil = AosIILMgr::getSelf()->retrieveIIL(data,len, eAosIILType_U64, true);
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
						sgIILIDs[idx],
						iiltype,
						false,
						true,
						mRunData);
	if (!iil)
	{
		OmnAlarm << "Failed to retrieve the IIL!" << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_U64)
	{
		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		OmnAlarm << "IIL type is wrong!" << enderr;
		return 0;
	}
	
	// 3. add the iil into list
	AosIILU64Ptr striil = (AosIILU64 *)iil.getPtr();
	return striil;
}

int  
AosIILU64Tester::u64cmp(u64 u1, u64 u2)
{
	if (u1 < u2) return -1;
	if (u1 == u2) return 0;
	if (u1 > u2) return 1;
	return 0;
}


int
AosIILU64Tester::findValue(
		const u64 &value, 
		u64* values, 
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
		int rslt = u64cmp(values[cur], value);
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
				rslt = u64cmp(values[cur - 1], value);
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

	int rslt = u64cmp(values[left], value);
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
		int rslt2 = u64cmp(values[right], value);
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
AosIILU64Tester::compareValueDocid(const u64 str1,
								   const u64 docid1,
								   const u64 str2,
								   const u64 docid2)
{
	int rslt = u64cmp(str1, str2);
	if(rslt != 0)
	{
		return rslt;
	}
	return docid1 - docid2; 
}

int
AosIILU64Tester::findValueDocid(
		const u64 &value,
		const u64 &docid, 
		u64 *values, 
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
		int rslt = compareValueDocid(values[cur], docids[cur] , value, docid);
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
				rslt = compareValueDocid(values[cur-1], docids[cur-1] , value, docid);
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

	int rslt = compareValueDocid(values[left], docids[left] , value, docid);
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
		int rslt2 = compareValueDocid(values[right], docids[right] , value, docid);
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
AosIILU64Tester::addEntry()
{
	// This function randomly picks an IIL, and then adds an entry
	// to that IIL. 

	// 1. Pick an IIL
	AosIILU64Ptr iil;
	int iilpos = 0;
	iil = pickIIL(iilpos, false);
	
	aos_assert_r(iil,false);
	u64 iilid = iil->getIILID();
	iilid = iilid-5001;
	
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
	//iil->copyData(sgValues, sgDocids, mNumDocs);
	iil->copyData(
		sgValues, 
		sgDocids, 
		(int)AOSSENG_TORTURER_MAX_DOCS, 
		mNumDocs,
		mRunData);
	
	// verify before add
	if (iilpos < sgMaxCheckedIILs)
	{
		// It is an IIL whose values are kept in memory	
		bool rslt = compareValueDocid(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << iilid << ":" <<sgOpr << enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}

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
			// random pick a u64, but it may be in the list
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
	if (sgNumDocs[iilpos] > sgLongestIIL) sgLongestIIL = sgNumDocs[iilpos];

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

		//iil->copyData(sgValues2, sgDocids2, mNumDocs);
		iil->copyData(
			sgValues2, 
			sgDocids2, 
			(int)AOSSENG_TORTURER_MAX_DOCS, 
			mNumDocs,
			mRunData);
		rslt = compareValueDocid2(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << iilid << ":" << sgOpr<< enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}

	rslt = verifyIIL(iil, iilpos);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILU64Tester::verifyIIL(const AosIILU64Ptr &iil, const int iilpos)
{
	//u64 iilid = iil->getIILID()-5001;
	int num_docs = sgNumDocs[iilpos];

	if (iilpos < sgMaxCheckedIILs)
	{
		aos_assert_r(verifyIIL(num_docs, iil, sgIILValues[iilpos], sgIILDocids[iilpos]), false);
	}
	else
	{
		//iil->copyData(sgValues, sgDocids, mNumDocs);
		iil->copyData(
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
AosIILU64Tester::doesDocidExist(
		const AosIILU64Ptr &iil, 
		u64 &value,
		const u64 &docid)
{
	int idx = -10;
	int iilidx = -10;
	bool isunique;
	u64 did = 0;

	u64 prev = 0;
	while (1)
	{
		bool rs = iil->nextDocidSafe(idx, iilidx, false, 
				eAosOpr_eq, value, did, isunique,mRunData);
		aos_assert_r(rs, false);
		if (idx < 0)
		{
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return false;
		}
		aos_assert_r(did > 0, false);
		aos_assert_r(iilidx >= 0, false);
		aos_assert_r(prev <= did, false);
		if (did == docid) return true;
		prev = did;
    }

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosIILU64Tester::deleteEntry()
{
	// Pick an existing one. 
	// 1. Pick an IIL
	if (sgNumIILs <= 0) return true;

	int iilpos;
	AosIILU64Ptr iil = pickIIL(iilpos, false);
	aos_assert_r(iil, false);

	u64 iilid = iil->getIILID()-5001;
	//u64 iilid = iil->getIILID();

	// 2. Determine value uniqueness
	bool value_unique;
	bool docid_unique;
	getUnique(iilid, value_unique, docid_unique);

	bool rslt;
	u32 num_docs = sgNumDocs[iilpos];

	// Copy the IIL data 
//	aos_assert_r(copyData(iilname), false);
	// Clean();
	//iil->copyData(sgValues, sgDocids,mNumDocs);
	iil->copyData(
			sgValues, 
			sgDocids,
			(int)AOSSENG_TORTURER_MAX_DOCS, 
			mNumDocs,
			mRunData);

	//delete <10000 verify(1)(2)
	if (iilpos < sgMaxCheckedIILs)
	{
	
		rslt = compareValueDocid(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << iilid  <<":"<< value_unique << ":" << docid_unique <<":" << mNumDocs <<":"<< sgNumDocs[iilpos] << ":" << mNumDocs << ", i:" << sgIi<< enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}
	
	
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
					docid = sgDocids[num];  //Linda,
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
					docid = sgDocids[num];  //Linda,
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

	rslt = iil->removeDocSafe(value, docid, mRunData);

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

			//iil->copyData(sgValues2, sgDocids2, mNumDocs);
			iil->copyData(
					sgValues2, 
					sgDocids2, 
					(int)AOSSENG_TORTURER_MAX_DOCS, 
					mNumDocs,
					mRunData);
			rslt = compareValueDocid2(iilid, mNumDocs, iilpos);
			if (!rslt)
			{
				OmnAlarm << "Failed to verify entries " << iilid << enderr;
				AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
				return false;
			}
		}
	}
	else
	{
		aos_assert_r(!rslt, false);
	}

	aos_assert_r(verifyIIL(iil, iilpos), false);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	return true;
}


bool
AosIILU64Tester::modifyEntry()
{
	//1. Pick iil
	if (sgNumIILs <= 0) return true;
	int iilpos;
	AosIILU64Ptr iil = pickIIL(iilpos, false);
	aos_assert_r(iil,false);

	u64 iilid = iil->getIILID()-5001;
	//u64 iilid = iil->getIILID();

	// 2. Determine value uniqueness
	bool value_unique;
	bool docid_unique;
	getUnique(iilid, value_unique, docid_unique);

	// 3. Copy the IIL data 
	// Clean();
	//iil->copyData(sgValues, sgDocids,mNumDocs);
	iil->copyData(
		sgValues, 
		sgDocids,
		(int)AOSSENG_TORTURER_MAX_DOCS, 
		mNumDocs,
		mRunData);
	// <10000 verify (1)(2)
	if (iilpos < sgMaxCheckedIILs)
	{
		
		bool rslt = compareValueDocid(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << iilid << enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}
	
	// 4. Prepare value/docid
	
	// 4.1 Pick entry
	u32 num_docs = sgNumDocs[iilpos];
	if (num_docs == 0)  return true;

	int idx = rand() % num_docs;
	u64 oldvalue = sgValues[idx];
	u64 olddocid = sgDocids[idx];
	
	u64 newvalue;
	while(1)
	{
		newvalue = rand();
		
		bool found;
		if(value_unique)
		{
			findValue(newvalue, sgValues, mNumDocs, found);
			if (found)
			{
				continue;
			}
		}
		if(docid_unique)
		{
			findValueDocid(newvalue, olddocid, sgValues, sgDocids, num_docs, found);
			if (found)
			{
				// The value is not in the list. 
				continue;
			}
		}
		break;
	}
	
/*	// 5. Modify the value, it may modify value only, docid only, or both
	// there are 2 ways to gen invalid data
	// 1. wrong value
	// 2. wrong docid
	
	OmnString newvalue = oldvalue;
	u64 newdocid = olddocid;
	bool use_way_1 = (rand() % 100) < 50;
	if(use_way_1)
	{
		while (1)
		{
			int len = (rand() % 20) + 5;
			char data[40];
			AosRandomLetterStr(len, data);
			newvalue = data;
			bool found;
			findValueDocid(newvalue, olddocid, sgValues, sgDocids, num_docs, found);
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
			newdocid = rand();
			bool found = false;
			findValueDocid(oldvalue, newdocid, sgValues, sgDocids, num_docs, found);
			if (!found)
			{
				// The value is not in the list. 
				break;
			}
		}
	}
*/
	//6 . Modify the list
	//modify (1)
	bool rslt = iil->modifyDocSafe(oldvalue, newvalue,
			olddocid, value_unique, docid_unique, false, mRunData);
	aos_assert_r(rslt, false);
	
	if (iilpos < sgMaxCheckedIILs)
	{
		// Need to modify the value and docid from the array.
		rslt = modifyValueFromArray(oldvalue, olddocid, 
				newvalue, olddocid, sgIILValues[iilpos],sgIILDocids[iilpos], mNumDocs);
		aos_assert_r(rslt, false);

		//iil->copyData(sgValues2, sgDocids2, mNumDocs);
		iil->copyData(
			sgValues2, 
			sgDocids2, 
			(int)AOSSENG_TORTURER_MAX_DOCS, 
			mNumDocs,
			mRunData);
		rslt = compareValueDocid2(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			OmnAlarm << "Failed to verify entries " << iilid << enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}

	aos_assert_r(verifyIIL(iil, iilpos), false);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	return true;

}


bool
AosIILU64Tester::getUnique(u64 iilid, bool &value_unique, bool &docid_unique)
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




bool
AosIILU64Tester::loadStrIIL(const OmnString &iilname)
{
	if (mStriil)
	{
		AosIILMgr::getSelf()->returnIILPublic(mStriil,mRunData);
	}

//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	u64 iilid = AosIILClient::getSelf()->getIILId(iilname,mRunData);
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
					iilid,
					iiltype,
					false,
					true,
					mRunData);



	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILU64 *)iil.getPtr();
	mIILName = iilname;
	return true;
}


bool
AosIILU64Tester::verifyObjidDuplicatedEntries()
{
	/*
	OmnScreen << "Check objid uniqueness ..." << endl;
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILU64 *)iil.getPtr();
	aos_assert_r(mStriil->verifyDuplicatedEntries(), false);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	OmnScreen << "Check objid uniqueness done!" << endl;
	*/
	return true;
}


AosOpr
AosIILU64Tester::pickOperator()
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
AosIILU64Tester::tortureNextDocidSafe(
		const int tries,
		const bool vunique, 
		const bool dunique, 
		u64* values, 
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
if (sgIi ==18 && i == 7)
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
			rslt = searchGE(value, reverse, vunique, dunique, docid, 
						eAosOpr_ge, value_valid);
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
rslt = searchGE(value, reverse, vunique, dunique, docid, eAosOpr_ge, value_valid);
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
AosIILU64Tester::searchGE(
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
AosIILU64Tester::searchReverseGE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	// This function assumes that it searches in the reverse order
	// for entries that are >= 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	u64 prev_value = 0;
	u64 value1 = value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, opr, value1, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			//OmnAlarm << "Failed: " << ":" << value << ":" 
			//	<< docid << enderr;
			return false;
		}

		// It found a valid entry. Check whether 
		// 		'value' <= mStriil->getValueSafe(idx, iilidx, mRunData)
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(crtvalue, prev_value) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = u64cmp(value1, crtvalue);
		if (rr < 0) 
		{
			// It is not what we are looking for. Keep on searching
			continue;
		}

		aos_assert_r(rr == 0, false);


		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			// If we continue searching, it should find no more.
			aos_assert_r(docid == expected_docid, false);
			u64 did;
			rslt = mStriil->nextDocidSafe(idx, iilidx, true, opr, value1, did, isunique,mRunData);
			aos_assert_r(rslt, false);
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			aos_assert_r(did == AOS_INVDID, false);
			return true;
		}

		// Check whether docids are sorted correctly.
		if (prev_docid != 0) aos_assert_r(docid <= prev_docid, false);
		prev_docid = docid;
			
		// It is not value_unique, check whether 'docid == expected_docid'.
		if (docid == expected_docid) 
		{
			// Check whether docid should be unique.
			if (docid_unique)
			{
				// We will keep on searching for a number of times. 
				// It should not find an entry whose docid is 'expected_docid'
				int num = rand() % 10;
				for (int i=0; i<num; i++)
				{
				 	rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
					 			opr, value1, docid, isunique,mRunData);
					aos_assert_r(rslt, false);
					aos_assert_r(docid != expected_docid, false);
					if (docid == AOS_INVDID)
					{
						// Finished
						aos_assert_r(idx == -5, false);
						aos_assert_r(iilidx == -5, false);
						return true;
					}
				}
				return true;
			}

			// docid is not unique. Simply return.
			return true;
		}

		// Value is not unique. Since it is searching in reverse order, 
		// the docid should be larger than the one we are looking for.
		aos_assert_r(expected_docid < docid, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64Tester::searchNormalGE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	// This function assumes that it searches in the normal order
	// for entries that are >= 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	// If it did not find the entry, it returns false.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	bool found = false;
	u64 prev_value = 0;
	u64 value1 = value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
				opr, value1, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// Did not find the entry
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return found;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);

		
		// It found a valid entry. Check whether the found value
		// is 'value'. If not, it is an error.
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);

		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(prev_value, crtvalue) <= 0, false);
		}
		prev_value = crtvalue;
	
		// mod by shawn
		int rr = u64cmp(value1, crtvalue);
		if (rr < 0)
		{
			continue;
		}
		if (rr > 0)
		{
			aos_assert_r(found, false);
			break;
		}
		//if(value1 != crtvalue)
		//{
		//	continue;
		//}



		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			aos_assert_r(!found, false);
			found = true;
			// If we continue searching, it should find no more.
			aos_assert_r(docid == expected_docid, false);
		}
		else
		{
			if (prev_docid > 0) aos_assert_r(prev_docid <= docid, false);
			prev_docid = docid;
			
			// value_unique is not unique.  Check whether docid should be unique.
			// Linda,2011/01/14
			if (docid_unique)
			{
				if (docid == expected_docid)
				{
					aos_assert_r(!found, false);
					found = true;
				}
			}
			else
			{
				if (docid == expected_docid) 
				{
					found = true;
					return found;
				}
			}
		}
	}

	return false;
}


bool
AosIILU64Tester::searchEQ(
		const u64 &value, 
		const bool reverse,
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
{
	// It searches the list for values that are equal to 'value', in 
	// either normal order or reversed order (determined by 'reverse').
	// If 'value_unique' is true, it should find the entry in the first
	// query. Otherwise, it will loop over the search until it finds
	// one.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;

	bool found = false;
	u64 prev_docid = 0;
	u64 value1 = value;
	while (1)
	{
		// OmnScreen << "Search EQ: " << value << ":" << expected_docid << endl;
		// Ready to search the list
		rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
				eAosOpr_eq, value1, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
	
		if (idx < 0)
		{
			// Did not find it. 
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return false;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);
		aos_assert_r(mStriil->getValueSafe(idx, iilidx, mRunData) == value1, false);

		if (value_unique)
		{
			// It should find it. Otherwise, it is an error. 
			found = false;
			u64 did1;
			aos_assert_r(isunique, false);
	
			// Searching next should find none
			rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
					eAosOpr_eq, value1, did1, isunique,mRunData);
			//aos_assert_r(rslt, false);
			aos_assert_r(did1 == AOS_INVDID, false);
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return docid == expected_docid;
		}

		if (docid == expected_docid)
		{
			found = true;
			return true;
		}

		if (prev_docid == 0) 
		{
			prev_docid = docid;
		}
		else
		{
			// Value is not unique. 
			if (docid_unique)
			{
				if (reverse)
				{
					aos_assert_r(docid < prev_docid, false);
				}
				else
				{
					aos_assert_r(prev_docid < docid, false);
				}
			}
			else
			{
				if (reverse)
				{
					aos_assert_r(docid <= prev_docid, false);
				}
				else
				{
					aos_assert_r(prev_docid <= docid, false);
				}
			}
			prev_docid = docid;
		}
	}
	return false;
}



bool
AosIILU64Tester::searchLE(
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
AosIILU64Tester::searchNormalLE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	// This function assumes that it searches in the Normal order
	// for entries that are <= 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	u64 prev_value = 0;
	bool found = false;
	u64 value1= value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, false, opr, value1, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return found;
		}

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, continue searching.
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(prev_value, crtvalue) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = u64cmp(value1, crtvalue);
		if (rr > 0)
		{
			// Did not find the one we want. Keep on searching
			continue;
		}

		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			// If we continue searching, it should find no more.
			aos_assert_r(!found, false);
			aos_assert_r(docid == expected_docid, false);
			rslt = mStriil->nextDocidSafe(idx, iilidx, false, opr, value1, docid, isunique,mRunData);
			aos_assert_r(rslt, false);
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			aos_assert_r(docid == AOS_INVDID, false);
			return true;
		}

		if (docid == expected_docid)
		{
			found = true;
		}

		// Check whether docids are sorted correctly.
		if (prev_docid != 0) 
		{
			if (docid_unique)
			{
				aos_assert_r(prev_docid < docid, false);
			}
			else
			{
				aos_assert_r(prev_docid <= docid, false);
			}
		}
		prev_docid = docid;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64Tester::searchReverseLE(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const bool value_valid)
{
	// This function assumes that it searches in the reverse order
	// for entries that are <= 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	// If it did not find the entry, it returns false.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	u64 prev_value = 0;
	bool found = false;
	u64 value1 = value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
				opr, value1, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// Did not find the entry
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return found;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);

		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(crtvalue, prev_value) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = u64cmp(value1, crtvalue);
		if (rr > 0)
		{
			// It is not what we are looking for. Keep on searching
			continue;
		}

		if (prev_docid > 0) aos_assert_r(docid <= prev_docid, false);
		prev_docid = docid;
	
		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			aos_assert_r(!found, false);
			found = true;
			aos_assert_r(docid == expected_docid, false);
		}
		else
		{
			// value_unique is not unique.  Check whether docid should be unique.
			// Linda,2011/01/14
			if (docid_unique)
			{
				if (docid == expected_docid)
				{
					aos_assert_r(!found, false);
					found = true;
				}
			}
			else
			{
				if (docid == expected_docid) 
				{
					found = true;
					return found;
				}
			}
		}
	}

	OmnShouldNeverComeHere;
	return false;
}

bool
AosIILU64Tester::addValueToArray(
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
		values[i]=values[i-1];
		docids[i] = docids[i-1];
	}
	
	values[idx] = value;
	docids[idx] = docid;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool
AosIILU64Tester::removeValueFromArray(
		const u64 &value, 
		const u64 &docid,
		u64*	values,
		u64* docids,
		const int num_docs)
{
	aos_assert_r(num_docs > 0, false);
	bool found;
	int idx = findValueDocid(value, docid, values, docids, num_docs, found);
	//Linda start
	if (idx ==-1) idx = 0;
	else 
	aos_assert_r(idx >= 0, false);  
	//end
	for(int i = idx; i <num_docs -1; i++)
	{
		values[i] =values[i+1];
		//strcpy(values[i],values[i+1]);
		docids[i] = docids[i+1];
	}
	
	values[num_docs -1] = 0;
	//strcpy(values[num_docs -1],"");
	docids[num_docs -1] = 0;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool 
AosIILU64Tester::modifyValueFromArray(
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
AosIILU64Tester::verifyIIL(
		const u32 num_docs,
		const AosIILU64Ptr &iil, 
		u64 *values, 
		const u64 *docids)
{
	//Skip verify IIL 
//	if (sgIi >= sgVerifyIILstartpos)	
//	{
		aos_assert_r(verifyEntries(num_docs, iil, values, docids), false);
//	}

	u64 iilid = iil->getIILID()-5001;
	bool value_unique;
	bool docid_unique;
	getUnique(iilid, value_unique, docid_unique);
	int tries = rand() % 10;
	mStriil = NULL;
	mStriil = iil;
	aos_assert_r(tortureNextDocidSafe(tries, 
		value_unique, docid_unique, values, docids, num_docs), false);

	return true;
}

bool
AosIILU64Tester::verifyEntries(
				const u32 num_docs,
				const AosIILU64Ptr &iil, 
				u64 *values, 
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
	
	for (int i=0; i<(int)num_docs; i++)
	{
	//	bool rs = iil->nextDocidSafe(idx, iilidx, false, 
		// 		eAosOpr_an, value, docid, isunique,mRunData);
			if (sgIi == 6744 && i == 500)
				OmnMark;
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
		
		if (values[i] != value)
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
	return true;
}

bool
AosIILU64Tester::compareValueDocid(const int iilid, const int num_docs, const int iilpos)
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
AosIILU64Tester::compareValueDocid2(const int iilid, const int num_docs, const int iilpos)
{
	// This function assumes that 'iilid' is an in-memory IIL. Its
	// values are stored in sgIILValues[iilid], its docids are stored
	// in sgIILDocids[iilid]. The caller has loaded the iil's value
	// to 'sgValues' and 'sgDocids'. 
	aos_assert_r(num_docs < sgMaxIILLen && num_docs >= 0, false);
	aos_assert_r(iilpos <= sgMaxCheckedIILs, false);
	for (int i = 0; i < num_docs;i++)
	{
		if (sgValues2[i]!= sgIILValues[iilpos][i])
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
AosIILU64Tester::toString()
{
	cout << mTotal 
		<< " Add=" << mAddOpr
		<< " Del=" << mDelOpr
		<< " Mod=" << mModOpr
		<< " IILs=" << sgNumIILs 
		<< " Max IIL Length: " << sgLongestIIL << endl;
}

/*
bool
AosIILU64Tester::searchGT(
		const u64 &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const int num_docs)
{
	bool rslt;
	if (reverse)
	{
		rslt = searchReverseGT(value, 
			value_unique, docid_unique, expected_docid, opr, num_docs);
		return rslt;
	}
	
	rslt = searchNormalGT(value, 
			value_unique, docid_unique, expected_docid, opr, num_docs);
	return rslt;
}


bool
AosIILU64Tester::searchReverseGT(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const int num_docs)
{
	// This function assumes that it searches in the reverse order
	// for entries that are > 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	//u64 prev_docid = 0;
	u64 prev_value = 0;
	int endidx = -10;
	int endiilidx= -10;
	int endidx1 = -10;
	int endiilidx1= -10;
	bool found = false;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, opr, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			int idx1 = -10;
			int iilidx1 = -10;
			while (1)
			{
				rslt = mStriil->nextDocidSafe(idx1, iilidx1, false, 
						eAosOpr_eq, value, docid, isunique,mRunData);
				aos_assert_r(rslt, false);

				if (docid == AOS_INVDID)
				{
					// Did not find the entry
					aos_assert_r(idx1 == -5, false);
					aos_assert_r(iilidx1 == -5, false);
					if (endidx == -10 && endiilidx == -10 && endidx1 != -10)
					{
						for (int i = 0; i< endiilidx1; i++) 
						{
							endidx1 = endidx1 + mStriil->getNumEntries(i);
						}
						aos_assert_r(endidx1 == num_docs-1, false);
						return true;
					}
					return found;
				}
				
				if (endidx == -10 && endiilidx == -10)
				{
					endidx1 = idx1;
					endiilidx1 = iilidx1;

					if(idx1!= num_docs-1)
						continue;
					else
						return true;
				}

				// Found an entry whose value matches 'value'. If 'value_unique'
				// is true, 'docid' must be 'expected_docid'.
				if (value_unique)
				{
					if(iilidx1 == endiilidx) 
					{
						aos_assert_r(idx1 == endidx-1, false);
					}
					else
					{
						aos_assert_r(idx1 == mStriil->getNumEntries(iilidx1)-1, false);
					}
					// If we continue searching, it should find no more.
					aos_assert_r(docid == expected_docid, false);
					aos_assert_r(!found, false);
					found = true;
					break;
				}
				else
				{
					if (docid_unique && docid == expected_docid)
					{
						if(iilidx1 == endiilidx) 
						{
							aos_assert_r(idx1 == endidx-1, false);
						}
						else 
						{
							aos_assert_r(idx1 == mStriil->getNumEntries(iilidx1)-1, false);
						}
						found = true;
						break;
					}
					if (!docid_unique)
					{
						if (docid == expected_docid) found = true;
						if (idx1 == endidx-1) 
						{
							//aos_assert_r(docid == expected_docid, false);
							aos_assert_r(iilidx1 == endiilidx, false);
							break;
						}

					}
				}
			}
			return true;
		}

		// It found a valid entry. Check whether 
		// 		'value' <= mStriil->getValue(idx, iilidx, mRunData)
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(crtvalue, prev_value) <= 0, false);
		}
		prev_value = crtvalue;

		endidx = idx;
		endiilidx = iilidx;
		int rr = u64cmp(value, crtvalue);
		if (rr < 0) 
		{
			// It is not what we are looking for. Keep on searching
			continue;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64Tester::searchNormalGT(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr,
		const int num_docs)
{
	// This function assumes that it searches in the normal order
	// for entries that are > 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	// If it did not find the entry, it returns false.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	bool found = false;
	u64 prev_value = 0;
	int firstidx = -10;
	int firstiilidx = -10;
	int firstidx1 = -10;
	int firstiilidx1 = -10;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
				opr, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// Did not find the entry
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);

			int idx1 = -10;
			int iilidx1 = -10;
			while(1)
			{
				rslt = mStriil->nextDocidSafe(idx1, iilidx1, false, 
						eAosOpr_eq, value, docid, isunique,mRunData);
				aos_assert_r(rslt, false);
			
				if (docid == AOS_INVDID)
				{
					// Did not find the entry
					aos_assert_r(idx1 == -5, false);
					aos_assert_r(iilidx1 == -5, false);
					if (firstidx == -10 && firstiilidx == -10 && firstidx1 !=-10)
					{
						for (int j = 0; j< firstiilidx1; j++) 
						{
							firstidx1 = firstidx1 + mStriil->getNumEntries(j);
						}
						aos_assert_r(firstidx1 == num_docs-1, false);
						return true;
					}
					return found;
				}
			
				if (firstidx == -10 && firstiilidx == -10)
				{
					firstidx1 = idx1;
					firstiilidx1 = iilidx1;
					if(idx1 != num_docs-1)
						continue;
					else
						return true;
				}
				
				if (prev_docid <=0) prev_docid = docid;
				//aos_assert_r(idx1 == firstidx+1, false);
				//aos_assert_r(iilidx1 == firstiilidx, false);

				// Found an entry whose value matches 'value'. If 'value_unique'
				// is true, 'docid' must be 'expected_docid'.
				if (value_unique)
				{
					aos_assert_r(!found, false);
					found = true;
					// If we continue searching, it should find no more.
					aos_assert_r(docid == expected_docid, false);	

					if(iilidx1 == firstiilidx) 
					{
						aos_assert_r(idx1 == firstidx-1, false);
					}
					else 
					{
						int iii =  mStriil->getNumEntries(iilidx1)-1;
						aos_assert_r(idx1 == iii, false);
					}
					break;
				}
				else
				{
					if (prev_docid > 0) aos_assert_r(prev_docid <= docid, false);
				
					// value_unique is not unique.  Check whether docid should be unique.
					if (docid_unique &&docid == expected_docid)
					{
						if(iilidx1 == firstiilidx) 
						{
							aos_assert_r(idx1 == firstidx-1, false);
						}
						else 
						{
							aos_assert_r(idx1 == mStriil->getNumEntries(iilidx1)-1, false);
						}
						found = true;
						break;
					}
					if (!docid_unique)
					{
						if (docid == expected_docid) found = true;
						if (idx1 == firstidx -1)
						{
							aos_assert_r(iilidx1 == firstiilidx, false);
							break;
						}
					}
				}
			}

			return found;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, it is an error.
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);

		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(prev_value, crtvalue) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = u64cmp(value, crtvalue);
	
		if (firstidx == -10 || firstiilidx == -10) 
		{
			firstidx = idx;
			firstiilidx = iilidx;
		}

		if (rr < 0)
		{
			continue;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}

bool
AosIILU64Tester::searchLT(
		const u64 &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr)
{
	bool rslt;
	if (reverse)
	{
		rslt = searchReverseLT(value, 
			value_unique, docid_unique, expected_docid, opr);
		return rslt;
	}
	
	rslt = searchNormalLT(value, 
			value_unique, docid_unique, expected_docid, opr);
	return rslt;
}

bool
AosIILU64Tester::searchNormalLT(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr)
{
	// This function assumes that it searches in the Normal order
	// for entries that are < 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	u64 prev_value = 0;
	bool found = false;
	int endidx = -10;
	int endiilidx = -10;
	int endidx1 = -10;
	int endiilidx1 = -10;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, false, opr, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);

			int idx1 = -10;
			int iilidx1 = -10;
			while(1)
			{
				rslt = mStriil->nextDocidSafe(idx1, iilidx1, false, 
						eAosOpr_eq, value, docid, isunique,mRunData);
				aos_assert_r(rslt, false);

				if (docid == AOS_INVDID)
				{
					// Did not find the entry
					aos_assert_r(idx1 == -5, false);
					aos_assert_r(iilidx1 == -5, false);
					if (endidx == -10 && endiilidx == -10 && endidx1 != -10)
					{
						aos_assert_r(endidx1 == 0, false);
					}
					return found;
				}
				
				if (endidx == -10 && endiilidx == -10)
				{
					endidx1 = idx1;
					endiilidx1 = iilidx1;

					if(idx1!= 0)
						continue;
					else
						return true;
				}

				// Found an entry whose value matches 'value'. If 'value_unique'
				// is true, 'docid' must be 'expected_docid'.
				if (value_unique)
				{
					// If we continue searching, it should find no more.
					if(iilidx1 == endiilidx)
					{
						aos_assert_r(idx1 == endidx+1, false);
					}
					else
					{
						aos_assert_r(idx1 == 0, false);
					}
					aos_assert_r(docid == expected_docid, false);
					aos_assert_r(!found, false);
					found = true;
					break;
				}	
				else
				{
					if (docid_unique && docid == expected_docid)
					{
						if(iilidx1 == endiilidx)
						{
						aos_assert_r(idx1 == endidx+1, false);
						}
						else
						{
						aos_assert_r(idx1 == 0, false);
						}
						found = true;
						break;
					}
					if (!docid_unique)
					{
						if (docid == expected_docid) found = true;
						if (idx1 == endidx-1)
						{
							aos_assert_r(iilidx1 == endiilidx, false);
							break;
						}
					}
				}
				
				// Check whether docids are sorted correctly.
				if (prev_docid != 0) 
				{
					if (docid_unique)
					{
						aos_assert_r(prev_docid < docid, false);
					}
					else
					{
						aos_assert_r(prev_docid <= docid, false);
					}
				}

				prev_docid = docid;

			}
			return found;
		}
			
		endidx = idx;
		endiilidx = iilidx;
		//prev_docid = docid;
		// It found a valid entry. Check whether the found value
		// is 'value'. If not, continue searching.
		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(prev_value, crtvalue) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = u64cmp(value, crtvalue);
		if (rr > 0)
		{
			// Did not find the one we want. Keep on searching
			continue;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILU64Tester::searchReverseLT(
		const u64 &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid, 
		const AosOpr opr)
{
	// This function assumes that it searches in the reverse order
	// for entries that are < 'value' until it finds an entry
	// whose value == 'value' and docid == 'expected_docid'. 
	// When this entry is found, if 'value_unique'
	// is true, the docid must be 'expected_docid'. Otherwise, it should
	// keep on searching until it finds one. 
	//
	// If it did not find the entry, it returns false.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	u64 prev_docid = 0;
	u64 prev_value = 0;
	bool found = false;
	int firstidx= -10;
	int firstiilidx = -10;
	int firstidx1= -10;
	int firstiilidx1 = -10;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
				opr, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// Did not find the entry
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);

			if (prev_docid <=0) prev_docid = docid;
			
			while(1)
			{
				int idx1 = -10;
				int iilidx1 = -10;
				rslt = mStriil->nextDocidSafe(idx1, iilidx1, false, 
						eAosOpr_eq, value, docid, isunique,mRunData);
				aos_assert_r(rslt, false);
			
				if (docid == AOS_INVDID)
				{
					aos_assert_r(idx1 == -5, false);
					aos_assert_r(iilidx1 == -5, false);
					if (firstidx == -10 && firstiilidx == -10 && firstidx1 !=-10)
					{
						aos_assert_r(firstidx1 == 0, false);
						return true;
					}
					return found;
				}

				if (firstidx == -10 && firstiilidx == -10)
				{
					firstidx1 = idx1;
					firstiilidx1 = iilidx1;
					if(idx1 != 0)
						continue;
					else
						return true;
				}
			
				if (prev_docid > 0) aos_assert_r(docid <= prev_docid, false);
				prev_docid = docid;
	
				// Found an entry whose value matches 'value'. If 'value_unique'
				// is true, 'docid' must be 'expected_docid'.
				if (value_unique)
				{
					aos_assert_r(!found, false);
					found = true;
					aos_assert_r(docid == expected_docid, false);
					if (iilidx1 == firstiilidx)
					{
						aos_assert_r(idx1 == firstidx+1, false);
					}
					else
					{
						aos_assert_r(idx1 == 0, false);
					}
					break;
				}
				else
				{
					if (docid == expected_docid) found = true;
	
					// value_unique is not unique.  Check whether docid should be unique.
					if (docid_unique && docid == expected_docid)
					{
						if (iilidx1 == firstiilidx)
						{
							aos_assert_r(idx1 == firstidx+1, false);
						}
						else
						{
							aos_assert_r(idx1 == 0, false);
						}
					}	
					if (!docid_unique)
					{
						if (docid == expected_docid) found = true;
						if (idx1 == firstidx +1)
						{
							aos_assert_r(iilidx1 == firstiilidx, false);
							break;
						}
					}
				}

			}

			return found;
		}

		
		if (firstidx == -10 && firstiilidx == -10)
		{
			firstidx = idx;
			firstiilidx = iilidx;
		}
		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);

		u64 crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != 0)
		{
			aos_assert_r(u64cmp(crtvalue, prev_value) <= 0, false);
		}

		prev_value = crtvalue;
		int rr = u64cmp(value, crtvalue);
		if (rr > 0)
		{
			// It is not what we are looking for. Keep on searching
			continue;
		}
		
	}

	OmnShouldNeverComeHere;
	return false;
}

*/
