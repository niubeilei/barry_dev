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
#include "SearchEngine/TesterNew/IILStrTester.h"

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
static char **		sgValues;
static u64			sgDocids[sgMaxIILDocs];
static char **		sgValues2;
static u64			sgDocids2[sgMaxIILDocs];

static u32			sgNumDocs[sgMaxTorIILs];
static OmnString 	sgIILNames[sgMaxTorIILs];
static u64			sgIILIDs[sgMaxTorIILs];


static int			sgNumIILs = 0;
static u32			sgLongestIIL = 0;
const int sgMaxStrLen = 20;
const int sgMaxCheckedIILs = 5;
const int sgMaxIILLen = 20000; //const int sgMaxIILLen = 20000;
static int 			sgIi;
static int 			sgOpr;
// 'sgIILValues' is an array of array of fixed length (AosIILUtil::eMaxStrValueLen+2) 
// char strings:
// 	sgIILValues[sgMaxCheckedIILs][sgMaxIILLen][AosIILUtil::eMaxStrValueLen+2]
static char **		sgIILValues[sgMaxCheckedIILs];
static u64			sgIILDocids[sgMaxCheckedIILs][sgMaxIILLen];
const int sgVerifyIILstartpos = 50000; // Starting from the current value of the verification

AosIILStrTester::AosIILStrTester()
:
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
	sgValues = new char*[sgMaxIILDocs];
	for (int i=0; i<sgMaxIILDocs; i++)
	{
		sgValues[i] = new char[AosIILUtil::eMaxStrValueLen+2];
		b+=AosIILUtil::eMaxStrValueLen+2;	
	}

	sgValues2 = new char*[sgMaxIILDocs];
	for (int i=0; i<sgMaxIILDocs; i++)
	{
		sgValues2[i] = new char[AosIILUtil::eMaxStrValueLen+2];
		b+=AosIILUtil::eMaxStrValueLen+2;	
	}

	for (int i=0; i<sgMaxTorIILs; i++)
	{
		sgNumDocs[i] = 0;
	}

	for (int i=0; i<sgMaxCheckedIILs; i++)
	{
		sgIILValues[i] = new char*[sgMaxIILLen];
		c+=sgMaxIILLen;

		for (int k=0; k<sgMaxIILLen; k++)
		{
			sgIILDocids[i][k] = 0;
			sgIILValues[i][k] = new char[AosIILUtil::eMaxStrValueLen+2];
			d += AosIILUtil::eMaxStrValueLen+2;
		}
	}

	for(int i = 0;i < sgMaxTorIILs; i++)
	{
		sgIILIDs[i] = 0;
	}
}


bool AosIILStrTester::start()
{
	mRunData =  OmnApp::getRundata();
	cout << "    Start OmnString Tester ..." << endl;
	// basicTest();
	torture();
	return true;
}


bool AosIILStrTester::basicTest()
{
	aos_assert_r(verifyObjidDuplicatedEntries(), false);
	aos_assert_r(verifyObjidIIL(), false);
	aos_assert_r(verifyAllIILS(5000, 600000), false);
	
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	aos_assert_r(loadStrIIL(iilname), false);
	aos_assert_r(copyData(mIILName), false);
	aos_assert_r(tortureNextDocidSafe(
					1000000, 
					true, 
					true, 
					sgValues, 
					sgDocids, 
					mNumDocs), false);
	return true;
}


bool
AosIILStrTester::torture()
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
if(i == 6579)
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


AosIILStrPtr
AosIILStrTester::pickIIL(int &idx,const bool create_new)
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
		AosIILType iiltype = eAosIILType_Str;
		// Pick an existing one. 
		// idx === one list
		idx = rand() % sgNumIILs;
		OmnString iilname = sgIILNames[idx];

		u64 iilid = sgIILIDs[idx];
		
		// create IIL if not exist
		if(iilid == 0)
		{
			sgIILIDs[idx] = AosIILMgr::getSelf()->createIILPublic(
								iiltype,
								false,
								mRunData);
			iilid = sgIILIDs[idx];
		}
//		AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
		AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
						iilid,
						iiltype,
						false,
						true,
						mRunData);

		aos_assert_r(iil, 0);
		if(iil->getIILType() != eAosIILType_Str)
		{
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return 0;
		}
		AosIILStrPtr striil = (AosIILStr *)iil.getPtr();
		return striil;
	}

	// 1. get the length of the random str. 
	// here we have length problem
//	while(1)
//	{
//		AosRandomLetterStr(len, data);
//		// make sure the string is not in the existing list
//		// better we can use a string hash instead of using IILMgr functions
//		AosIILPtr check_iil = AosIILMgr::getSelf()->retrieveIIL(data,len, eAosIILType_Str, false);
//		if(!check_iil)
//		{
//			break;
//		}
//	}	

	// 2. create the iil
	idx = sgNumIILs++;
//	sgIILNames[sgNumIILs++] = data;
	AosIILType iiltype = eAosIILType_Str;
	sgIILIDs[idx] = AosIILMgr::getSelf()->createIILPublic(
								iiltype,
								false,
								mRunData);
	u64 iilid = sgIILIDs[idx];
//	AosIILPtr iil = AosIILClient::getSelf()->getIILPublic(
//			data, true , eAosIILType_Str);

	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
					iilid,
					iiltype,
					false,
					true,
					mRunData);
	if (!iil)
	{
		OmnAlarm << "Failed to retrieve the IIL!" << enderr;
		return 0;
	}

	if (iil->getIILType() != eAosIILType_Str)
	{
		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		OmnAlarm << "IIL type is wrong!" << enderr;
		return 0;
	}
	
	// 3. add the iil into list
	AosIILStrPtr striil = (AosIILStr *)iil.getPtr();
	return striil;
}



int
AosIILStrTester::findValue(
		const OmnString &value, 
		char **values, 
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
		int rslt = strcmp(values[cur], value.data());
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
				rslt = strcmp(values[cur - 1], value.data());
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

	int rslt = strcmp(values[left], value.data());
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
		int rslt2 = strcmp(values[right], value.data());
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
AosIILStrTester::compareValueDocid(const char* str1,
								   const u64 docid1,
								   const char* str2,
								   const u64 docid2)
{
	int rslt = strcmp(str1, str2);
	if(rslt != 0)
	{
		return rslt;
	}
	return docid1 - docid2; 
}

int
AosIILStrTester::findValueDocid(
		const OmnString &value,
		const u64 &docid, 
		char **values, 
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
		int rslt = compareValueDocid(values[cur], docids[cur] , value.data(),docid);
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
				rslt = compareValueDocid(values[cur-1], docids[cur-1] , value.data(),docid);
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

	int rslt = compareValueDocid(values[left], docids[left] , value.data(),docid);
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
		int rslt2 = compareValueDocid(values[right], docids[right] , value.data(),docid);
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
AosIILStrTester::addEntry()
{
	// This function randomly picks an IIL, and then adds an entry
	// to that IIL. 

	// 1. Pick an IIL
	AosIILStrPtr iil;
	int iilpos = 0;
	iil = pickIIL(iilpos, false);
	
	aos_assert_r(iil,false);
	u64 iilid = iil->getIILID();
	iilid = iilid-5001;
	
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
	// Clean();
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
	OmnString value;
	if (value_unique)
	{
		// generate a value which is not in the iil
		while (1)
		{
			int len = (rand() % 20) + 5;
			char data[40];
			AosRandomLetterStr(len, data);
			value = data;

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
			int len = (rand() % 20) + 5;
			char data[40];
			AosRandomLetterStr(len, data);
			value = data;
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
	//if (docid_unique && !value_unique)
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
	bool rslt = iil->addDocSafe(value, docid, value_unique, docid_unique, false, mRunData);
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
AosIILStrTester::verifyIIL(const AosIILStrPtr &iil, const int iilpos)
{
	//u64 iilid = iil->getIILID()-5001;
	int num_docs = sgNumDocs[iilpos];

	if (iilpos < sgMaxCheckedIILs)
	{
		aos_assert_r(verifyIIL(num_docs, iil, sgIILValues[iilpos], sgIILDocids[iilpos]), false);
	}
	else
	{
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
AosIILStrTester::doesDocidExist(
		const AosIILStrPtr &iil, 
		const OmnString &value,
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
AosIILStrTester::deleteEntry()
{
	// Pick an existing one. 
	// 1. Pick an IIL
	if (sgNumIILs <= 0) return true;

	int iilpos;
	AosIILStrPtr iil = pickIIL(iilpos, false);
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
	OmnString value;
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
				int len = (rand() % 20) + 5;
				char data[40];
				AosRandomLetterStr(len, data);
				value = data;

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

	rslt = iil->removeDocSafe(value, docid,mRunData);

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
AosIILStrTester::modifyEntry()
{
	//1. Pick iil
	if (sgNumIILs <= 0) return true;
	int iilpos;
	AosIILStrPtr iil = pickIIL(iilpos, false);
	aos_assert_r(iil,false);

	u64 iilid = iil->getIILID()-5001;
	//u64 iilid = iil->getIILID();

	// 2. Determine value uniqueness
	bool value_unique;
	bool docid_unique;
	getUnique(iilid, value_unique, docid_unique);

	// 3. Copy the IIL data 
	// Clean();
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
	OmnString oldvalue = sgValues[idx];
	u64 olddocid = sgDocids[idx];
	
	OmnString newvalue;
	while(1)
	{
		int len = (rand() % 20) + 5;
		char data[40];
		AosRandomLetterStr(len, data);
		newvalue = data;
		
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
			olddocid, value_unique, docid_unique, false,mRunData);
	aos_assert_r(rslt, false);
	
	if (iilpos < sgMaxCheckedIILs)
	{
		// Need to modify the value and docid from the array.
		rslt = modifyValueFromArray(oldvalue, olddocid, 
				newvalue, olddocid, sgIILValues[iilpos],sgIILDocids[iilpos], mNumDocs);
		aos_assert_r(rslt, false);

		iil->copyData(
			sgValues2, 
			sgDocids2, 
			(int)AOSSENG_TORTURER_MAX_DOCS, 
			mNumDocs,
			mRunData);
		rslt = compareValueDocid2(iilid, mNumDocs, iilpos);
		if (!rslt)
		{
			rslt = iil->modifyDocSafe(oldvalue, newvalue,
				olddocid, value_unique, docid_unique, false,mRunData);

			rslt = iil->modifyDocSafe(oldvalue, newvalue,
				olddocid, value_unique, docid_unique, false,mRunData);

			OmnAlarm << "Failed to verify entries " << iilid << enderr;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
	}

	aos_assert_r(verifyIIL(iil, iilpos), false);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	return true;

}
/*
bool 
AosIILStrTester::modifyEntry()
{
	// 1. Pick iil
	if (sgNumIILs <= 0) return true;

	int idx = rand() % sgNumIILs;
	OmnString iilname = sgIILNames[idx];
	AosIILPtr iil = AosIILClient::getSelf()->getIILPublic(iilname);
	aos_assert_r(iil, 0);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, 0);
	AosIILStrPtr striil = (AosIILStr *)iil.getPtr();
	u32 num_docs = sgNumDocs[idx];
	if (num_docs <= 0) return true;
	u64 iilid = iil->getIILID();

	// 2. Determine value and docid uniqueness
	bool value_unique; 
	bool docid_unique;
	getValueDocidUnique(iilid, value_unique, docid_unique);

	// 3. Load the data
	aos_assert_r(copyData(iilname), false);

	// 4. Pick entry
	int idx = rand() % num_docs;
	OmnString oldvalue = sgValues[idx];
	u64 olddocid = sgDocids[idx];

	// 5. Modify the value, it may modify value only, docid only, or both
	OmnString newvalue = oldvalue;
	u64 newdocid = olddocid;
	if (rand() % 100 < 50)
	{
		// Modify value
		int len = (rand() % 20) + 5;
		char data[40];
		AosRandomLetterStr(len, data);
		newvalue = data;
	}

	if (rand() % 100 < 50)
	{
		// Modify the docid
		newdocid = rand();
	}
	
	// 6. Modify the list
	bool rslt = striil->modifyDocSafe(oldvalue, newvalue, 
			olddocid, value_unique, docid_unique, false);

	// 7. Verify
	if (value_unique)
	{
		// Vaule is unique.
		if (oldvalue != newvalue)
		{
			// The value is modified. Check whether the new value is in the
			// list or not. 
			bool found;
			int pos = findValue(newvalue, sgValues, num_docs, found);
			if (found)
			{
				// It modified the list but the new value is in the list. 
				// It should fail the operation
				aos_assert_r(pos >= 0, false);
				aos_assert_r(!rslt, false);
			}
			else
			{
				// The new value is not in the list. It should succeed the operation
				aos_assert_r(pos < 0, false);
				aos_assert_r(rslt, false);
				aos_assert_r(removeEntry(oldvalue, olddocid, 
					sgValues, sgDocids, num_docs), false);
			}
		}
		else
		{
			// The value was not modified. It should do nothing.
			aos_assert_r(rslt, false);
		}
	}
	else
	{
	}

	//change array value
	sgIILValues[sgNumIILs] = newvalue;
	//verify
	bool rs = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, value, docid, isunique,mRunData);
	if(!rs || idx < 0)
	{
		return false;
     }
	else
	{
	bool rs = striil->nextDocidSafe(idx, iilidx, false, eAosOpr_eq, value, docid, isunique,mRunData);
	aos_assert_r(strcmp(value, newvalue)==0, false);
	}

	return true;	
}
*/


bool
AosIILStrTester::verifyObjidIIL()
{
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	iilname << sgSiteid;
	OmnScreen << "Start verifying Objid IIL ..." << endl;

//	u64 iilid = AosIILClient::getSelf()->getIILId(iilname,mRunData);
	aos_assert_r(verifyValues(iilname, true, true), false);
	OmnScreen << "Start verifying Objid IIL ... Finished" << endl;
	return true;
}


bool
AosIILStrTester::getUnique(u64 iilid, bool &value_unique, bool &docid_unique)
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
AosIILStrTester::verifyValues(
		const OmnString &iilname, 
//		const u64 &iilid, 
		const bool value_unique,
		const bool docid_unique)
{
	// It copies all the values and docids into sgValues and sgDocids. 
	// It then sequentially verify each and every entry entry.
	aos_assert_r(copyData(iilname), false);

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
	mStriil = (AosIILStr *)iil.getPtr();

	// 1. Check it in the normal order
	aos_assert_r(verifyNextDocidSafe(false, value_unique), false);

	// 2. Check it in the reversed order
	aos_assert_r(verifyNextDocidSafe(true, value_unique), false);

	aos_assert_r(mStriil->verifyDuplicatedEntries(mRunData), false);

	return true;
}


bool
AosIILStrTester::loadStrIIL(const OmnString &iilname)
{
	if (mStriil)
	{
		AosIILMgr::getSelf()->returnIILPublic(mStriil,mRunData);
	}
	u64 iilid = AosIILClient::getSelf()->getIILId(iilname,mRunData);
	AosIILType iiltype = eAosIILType_Invalid;
//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(iilid,iiltype,false, true,mRunData);

	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILStr *)iil.getPtr();
	mIILName = iilname;
	return true;
}


bool
AosIILStrTester::verifyObjidDuplicatedEntries()
{
	OmnScreen << "Check objid uniqueness ..." << endl;
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);

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
	mStriil = (AosIILStr *)iil.getPtr();
	aos_assert_r(mStriil->verifyDuplicatedEntries(mRunData), false);
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	OmnScreen << "Check objid uniqueness done!" << endl;
	return true;
}


bool
AosIILStrTester::verifyGetObjid()
{
	/*
	OmnScreen << "Verify GetObjid: ..." << endl;
	int tries = rand() % mVerifyGetObjidWeight;

	for (int i=0; i<tries; i++)
	{
		OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
		AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
		aos_assert_r(iil, false);
		aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
		mStriil = (AosIILStr *)iil.getPtr();
		u64 docid;
		OmnString objid = mStriil->getRandomValue(docid);
		aos_assert_r(objid != "", false);
		bool duplicated;
		AosXmlRc errcode; 
		OmnString errmsg;
		AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(sgSiteid, "", 
			eAOSDOCID_ROOT, "", objid, duplicated, errcode, errmsg);
		aos_assert_r(doc, false);
		if (doc->getAttrStr(AOSTAG_OBJID) != objid || 
			doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID) != docid)
		{
			OmnAlarm << "Failed retrieving the doc: " << docid << ":" << objid << enderr;
			OmnScreen << doc->toString() << endl;
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
			return false;
		}
		AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	}
	OmnScreen << "Verify GetObjid: ... Done!" << endl;
*/
	return true;
}


bool
AosIILStrTester::retrieveAllObjids()
{
/*	OmnScreen << "Retrieve all objids: ... " << endl;
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILStr *)iil.getPtr();

	int num_subiils = mStriil->getNumSubiils();
	for (int i=0; i<num_subiils; i++)
	{
		AosIILStrPtr subiil = mStriil->getSubiilPublic(i);
		aos_assert_r(subiil, false);
		u32 numdocs = subiil->getNumDocs();
		for (u32 aa=0; aa<numdocs; aa++)
		{
			OmnString objid = subiil->getValue(aa);
			aos_assert_r(objid != "", false);

			bool duplicated;
			AosXmlRc errcode;
			OmnString errmsg;
			AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(sgSiteid, "", 
				eAOSDOCID_ROOT, "", objid, duplicated, errcode, errmsg);
			aos_assert_r(doc, false);
			aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == objid, false);
		}
	}

	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	OmnScreen << "Retrieve all objids: ... Done" << endl;
*/
	return true;
}


bool
AosIILStrTester::copyData(const OmnString &iilname)
{
	u64 iilid = AosIILClient::getSelf()->getIILId(iilname,mRunData);
	AosIILType iiltype = eAosIILType_Invalid;
//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(iilid,iiltype, false, true, mRunData);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILStr *)iil.getPtr();
	int max_doc_num = AOSSENG_TORTURER_MAX_DOCS;
	aos_assert_r(mStriil->copyData(
							sgValues, 
							sgDocids, 
							max_doc_num, 
							mNumDocs,
							mRunData), false);
	return true;
}


bool
AosIILStrTester::verifyNextDocidSafe(
		const bool reverse, 
		const bool value_unique) 
{
	OmnScreen << "Total Docs: " << mNumDocs << " ... " << endl;
	int group = 0;
	OmnString prev_value;
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
		OmnString value = sgValues[aa];
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

		OmnString vv = mStriil->getValueSafe(idx, iilidx, mRunData);
		aos_assert_r(vv != "", false);
		if (prev_value != "") 
		{
			if (value_unique)
			{
				aos_assert_r(strcmp(prev_value.data(), vv.data()) < 0, false);
			}
			else
			{
				aos_assert_r(strcmp(prev_value.data(), vv.data()) <= 0, false);
			}
		}
		prev_value = vv;
	}

	OmnScreen << "Total Docs: " << mNumDocs << " ... Done!" << endl;
	return true;
}


AosOpr
AosIILStrTester::pickOperator()
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
AosIILStrTester::tortureNextDocidSafe(
		const int tries,
		const bool vunique, 
		const bool dunique, 
		char **values, 
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
		OmnString value;
		u64 docid = 0;
if (sgIi ==18 && i ==7)
OmnMark;

		// 4. Determine whether to look up an existing value
		if (PERCENT(20))
		{
			while(1)
			{
				// Will randomly pick a word
				int len = (rand() % 20) + 5;
				char data[40];
				AosRandomLetterStr(len, data);
				value = data;

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
AosIILStrTester::searchGE(
		const OmnString &value, 
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
AosIILStrTester::searchReverseGE(
		const OmnString &value, 
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
	OmnString prev_value;
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
			//OmnAlarm << "Failed: " << ":" << value << ":" 
			//	<< docid << enderr;
			return false;
		}

		// It found a valid entry. Check whether 
		// 		'value' <= mStriil->getValueSafe(idx, iilidx)
		OmnString crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != "")
		{
			aos_assert_r(strcmp(crtvalue.data(), prev_value.data()) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = strcmp(value.data(), crtvalue.data());
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
			rslt = mStriil->nextDocidSafe(idx, iilidx, true, opr, value, did, isunique,mRunData);
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
					 			opr, value, docid, isunique,mRunData);
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
AosIILStrTester::searchNormalGE(
		const OmnString &value, 
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
	OmnString prev_value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
				opr, value, docid, isunique,mRunData);
		//rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
		//		opr, value, docid, isunique,mRunData);
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

		
//		if (!value_valid)
//		{
//			mStriil ->setIILIdx(0);
//			aos_assert_r(mStriil->getValueSafe(idx, iilidx) != value, false);
//			continue;
//		}

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, it is an error.
		OmnString crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);

		if (prev_value != "")
		{
			aos_assert_r(strcmp(prev_value.data(), crtvalue.data()) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = strcmp(value.data(), crtvalue.data());
		if (rr < 0)
		{
			continue;
		}

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
			
			// Linda,2011/01/14
			if (docid_unique)
			{
				if (docid == expected_docid)
				{
					aos_assert_r(!found, false);
					found = true;
				}
				//aos_assert_r(docid == expected_docid, false);
			}else// docid not unique
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
AosIILStrTester::searchEQ(
		const OmnString &value, 
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
	while (1)
	{
		// OmnScreen << "Search EQ: " << value << ":" << expected_docid << endl;
		// Ready to search the list
		rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
				eAosOpr_eq, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
	
		if (idx < 0)
		{
			// Did not find it. 
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			//aos_assert_r(iilidx == -15, false);
			return false;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);
		//mStriil ->setIILIdx(0);
		aos_assert_r(mStriil->getValueSafe(idx, iilidx, mRunData) == value, false);

		if (value_unique)
		{
			// It should find it. Otherwise, it is an error. 
			found = false;
			u64 did1;
			aos_assert_r(isunique, false);
	
			// Searching next should find none
			rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
					eAosOpr_eq, value, did1, isunique,mRunData);
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
AosIILStrTester::searchLE(
		const OmnString &value, 
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
AosIILStrTester::searchNormalLE(
		const OmnString &value, 
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
	OmnString prev_value;
	bool found = false;
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
			return found;
		}

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, continue searching.
		OmnString crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != "")
		{
			aos_assert_r(strcmp(prev_value.data(), crtvalue.data()) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = strcmp(value.data(), crtvalue.data());
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
int dd1 = idx;
int dd2 = iilidx;
			rslt = mStriil->nextDocidSafe(idx, iilidx, false, opr, value, docid, isunique,mRunData);
			aos_assert_r(rslt, false);
if (idx != -5)
{
rslt = mStriil->nextDocidSafe(dd1, dd2, false, opr, value, docid, isunique,mRunData);
}
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
AosIILStrTester::searchReverseLE(
		const OmnString &value, 
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
	OmnString prev_value;
	bool found = false;
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
			return found;
		}

		aos_assert_r(idx >= 0, false);
		aos_assert_r(iilidx >= 0, false);

		OmnString crtvalue = mStriil->getValueSafe(idx, iilidx, mRunData);
		if (prev_value != "")
		{
			aos_assert_r(strcmp(crtvalue.data(), prev_value.data()) <= 0, false);
		}
		prev_value = crtvalue;

		int rr = strcmp(value.data(), crtvalue.data());
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
				//aos_assert_r(docid == expected_docid, false);
			}else// docid not unique
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
/*
bool
AosIILStrTester::searchGT(
		const OmnString &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid)
{
	if (reverse)
	{
		aos_assert_r(searchReverseGT(value, 
			value_unique, docid_unique, expected_docid), false);
		return true;
	}
	
	aos_assert_r(searchNormalGT(value, 
			value_unique, docid_unique, expected_docid), false);
	return true;
}


bool
AosIILStrTester::searchReverseGT(
		const OmnString &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
{
	// This function assumes that it searches in the reverse order
	// for entries that are > 'value'. Since it is searching in the
	// reversed order, it should continue searching until it finds
	// no more.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	OmnString prev_value;
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
				eAosOpr_ge, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			return true;
		}

		aos_assert_r(idx > 0, false);
		aos_assert_r(iilidx > 0, false);
		OmnString crt_value = mStriil->getValueSafe(idx, iilidx);
		if (prev_value != "")
		{
			if (value_unique)
			{
				aos_assert_r(strcmp(crt_value.data(), prev_value.data()) < 0, false);
			}
			else
			{
				aos_assert_r(strcmp(crt_value.data(), prev_value.data()) <= 0, false);
			}
		}
		prev_value = crt_value;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILStrTester::searchNormalGT(
		const OmnString &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
{
	// It searches in the normal order for entries that are > value.
	// It should not find the entry.
	int idx = -10;
	int iilidx = -10;
	u64 docid;
	bool isunique;
	bool rslt;
	rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
			eAosOpr_ge, value, docid, isunique,mRunData);
	aos_assert_r(rslt, false);

	if (docid == AOS_INVDID)
	{
		aos_assert_r(idx == -5, false);
		aos_assert_r(iilidx == -5, false);
		return true;
	}

	OmnString prev_value = mStriil->getValueSafe(idx, iilidx);

	while (1)
	{
		if (docid == expected_docid)
		{
			// Found the entry. 
			if (docid_unique)
			{
				// We will keep on searching for a number of times. 
				// It should not find an entry whose docid is 'expected_docid'
				int num = rand() % 10;
				for (int i=0; i<num; i++)
				{
				 	rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
					 			eAosOpr_ge, value, docid, isunique,mRunData);
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
			}
			return true;
		}

		// Keep on searching
	 	rslt = mStriil->nextDocidSafe(idx, iilidx, false, 
				eAosOpr_ge, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		if (docid == AOS_INVDID)
		{
			// It failed the searching
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			OmnAlarm << "Failed: " << value << ":" << docid << enderr;
			return false;
		}

		OmnString crt_value = mStriil->getValueSafe(idx, iilidx);
		if (value_unique)
		{
			aos_assert_r(strcmp(crt_value.data(), prev_value.data()) > 0, false);
		}
		else
		{
			aos_assert_r(strcmp(crt_value.data(), prev_value.data()) >= 0, false);
		}
		prev_value = crt_value;
	}

	OmnShouldNeverComeHere;
	return false;
}
*/

/*
bool
AosIILStrTester::searchLE(
		const OmnString &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid)
{
	OmnNotImplementedYet;
	return false;
}
*/


bool
AosIILStrTester::verifyAllIILS(
		const u64 &start_iilidx,
		const u64 &end_iilidx)
{
	OmnScreen << "Verifying all iils ..." << endl;
	AosIILType iiltype;
	int group = 0;
	for (u64 i=start_iilidx; i<=end_iilidx; i++)
	{
		if (++group == 100)
		{
			cout << ":" << i << flush;
			group = 0;
		}

		//AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(i, iiltype);
		AosIILPtr iil;
		if (iil)
		{
			switch (iiltype)
			{
			case eAosIILType_Str:
				 aos_assert_r(verifyStrIIL((AosIILStr*)iil.getPtr()), false);
				 break;

			default:
				 break;
			}
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		}

	}

	return true;
}


bool
AosIILStrTester::verifyStrIIL(const AosIILStrPtr &iil)
{
	// This function verifies that all the values are sorted. 
	// In addition, for the same values, docids are sorted. 
	int max_doc_num = AOSSENG_TORTURER_MAX_DOCS;
	aos_assert_r(iil->copyData(
						sgValues, 
						sgDocids, 
						max_doc_num, 
						mNumDocs,
						mRunData), false);
	
	// Values should be sorted. For the same values, the docids are sorted.
	if (mNumDocs <= 1) return true;

	for (int aa=1; aa<mNumDocs; aa++)
	{
		int rslt = strcmp(sgValues[aa-1], sgValues[aa]);
		if (rslt > 0)
		{
			OmnAlarm << "Value out of order: " << aa << enderr;
			for (int bb=0; bb<aa+10; bb++)
			{
				cout << sgDocids[bb] << ":" << sgValues[bb] << endl;
			}
			return false;
		}

		if (rslt < 0) continue;
		
		for (int kk=aa+1; kk<mNumDocs; kk++)
		{
			int rslt2 = strcmp(sgValues[kk-1], sgValues[kk]);
			aos_assert_r(rslt2 <= 0, false);
			if (rslt2 < 0)
			{
				aa = kk;
				break;
			}

			if (rslt2 == 0)
			{
				if (sgDocids[kk-1] > sgDocids[kk])
				{
					OmnAlarm << "Out of order found: " << iil->getIILID()
						<< ":" << kk << ":" << mNumDocs << enderr;

					cout << "\n";
					for (int bb=0; bb<kk+10; bb++)
					{
						cout << sgDocids[bb] << ":" << sgValues[bb] << endl;
					}
					return false;
				}
			}
		}
	}
	return true;
}


bool
AosIILStrTester::addValueToArray(
		const OmnString &value, 
		const u64 &docid,
		char**	values,
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
		strcpy(values[i],values[i-1]);
		docids[i] = docids[i-1];
	}
	
	strcpy(values[idx],value);
	docids[idx] = docid;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool
AosIILStrTester::removeValueFromArray(
		const OmnString &value, 
		const u64 &docid,
		char**	values,
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
		strcpy(values[i],values[i+1]);
		docids[i] = docids[i+1];
	}
	
	strcpy(values[num_docs -1],"");
	docids[num_docs -1] = 0;
	// 'idx' is the idx that the value should be inserted in front of it.
	return true;
}

bool 
AosIILStrTester::modifyValueFromArray(
		const OmnString &oldvalue, 
		const u64 &olddocid,
		const OmnString &newvalue, 
		const u64 &newdocid,
		char**	values,
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


//bool
//AosIILStrTester::removeEntry(
//		const OmnString &value, 
//		const u64 &docid,
//		char **values,
//		u64 *docids,
//		u64 &num_docs)
//{
//	bool found;
//	int idx = findValue(value, values, num_docs, found);
//	aos_assert_r(idx >= 0, false);
//	aos_assert_r(found, false);
//
//	for (int i=idx; i<num_docs; i++)
//	{
//		strcpy(values[i], values[i+1]);
//		docids[i] = docids[i+1];
//	}
//	num_docs--;
//	return true;
//}
 

/*
bool
AosIILStrTester::removeEntry(
		char **values,
		u64 *docids,
		const int idx,
		const int num_docs,
		const u64 &iilid)
{
	aos_assert_r(idx >= 0, false);

	for (int i=idx; i<num_docs; i++)
	{
		strcpy(values[i], values[i+1]);
		docids[i] = docids[i+1];
	}
	//Linda,
	sgNumDocs[iilid]--;
	return true;
}


bool
AosIILStrTester::insertDoc(
		const u64 &iilid, 
		const OmnString &value, 
		const u64 &docid, 
		const u32 idx)
{
	// This function inserts the pair [value, docid] at position 'idx'. 
	// If 'idx' > num_docs, it is to append to the end of the list. 
	u32 num_docs = sgNumDocs[iilid];  //Linda,
	if (num_docs >= eMaxIILLen)
	{
		for (u32 k=eMaxIILLen-1; k>idx; k--)
		{
			strcpy(sgValues[k], sgValues[k-1]);
			sgDocids[k] = sgDocids[k-1];
		}
	}
	else
	{
		for (u32 k=num_docs; k>idx; k--)
		{
			strcpy(sgValues[k], sgValues[k-1]);
			sgDocids[k] = sgDocids[k-1];
		}
	}
	
	strcpy(sgValues[idx], value.data());
	sgDocids[idx] = docid;
	return true;
}
*/


bool
AosIILStrTester::verifyIIL(
		const u32 num_docs,
		const AosIILStrPtr &iil, 
		char **values, 
		const u64 *docids)
{
	//Skip verify IIL
	//if(sgIi >= sgVerifyIILstartpos)
	//{
		aos_assert_r(verifyEntries(num_docs, iil, values, docids), false);
	//}

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
AosIILStrTester::verifyEntries(
		const u32 num_docs,
		const AosIILStrPtr &iil, 
		char **values, 
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
	OmnString value;
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

			if (!(strcmp(values[i], value) == 0))
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
		value = "";
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
		
			if (!(strcmp(values[i], value) == 0))
			{
				OmnAlarm << "Failed to verify entries " << iilidx <<":" << sgIi << ":" 
					<< value <<":" << docid << ":" << idx << ":" << num_docs << ":" 
					<< i << ":" << values[i] << ":" << docids[i]<< ":" <<sgOpr << enderr;
				return false;
			}
			aos_assert_r(strcmp(values[i], value) == 0, false);
	
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
AosIILStrTester::compareValueDocid(const int iilid, const int num_docs, const int iilpos)
{
	// This function assumes that 'iilid' is an in-memory IIL. Its
	// values are stored in sgIILValues[iilid], its docids are stored
	// in sgIILDocids[iilid]. The caller has loaded the iil's value
	// to 'sgValues' and 'sgDocids'. 
	aos_assert_r(num_docs < sgMaxIILLen && num_docs >= 0, false);
	aos_assert_r(iilpos <= sgMaxCheckedIILs, false);
	for (int i = 0; i < num_docs;i++)
	{
		if (!(strcmp(sgValues[i], sgIILValues[iilpos][i]) == 0))
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
AosIILStrTester::compareValueDocid2(const int iilid, const int num_docs, const int iilpos)
{
	// This function assumes that 'iilid' is an in-memory IIL. Its
	// values are stored in sgIILValues[iilid], its docids are stored
	// in sgIILDocids[iilid]. The caller has loaded the iil's value
	// to 'sgValues' and 'sgDocids'. 
	aos_assert_r(num_docs < sgMaxIILLen && num_docs >= 0, false);
	aos_assert_r(iilpos <= sgMaxCheckedIILs, false);
	for (int i = 0; i < num_docs;i++)
	{
		if (!(strcmp(sgValues2[i], sgIILValues[iilpos][i]) == 0))
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
AosIILStrTester::toString()
{
	cout << mTotal 
		<< " Add=" << mAddOpr
		<< " Del=" << mDelOpr
		<< " Mod=" << mModOpr
		<< " IILs=" << sgNumIILs 
		<< " Max IIL Length: " << sgLongestIIL << endl;
}

