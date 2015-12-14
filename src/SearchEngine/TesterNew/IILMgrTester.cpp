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
// 1. iilid % 4 == 3: value and docid are not unique
// 2. iilid % 4 == 1: value not unique, docid unique
// 3. iilid % 4 == 2: value and docid unique
//
// sgNumDocs[eMaxIILs]:  keep the number of docs for all IILs
// sgIILNames[eMaxIILs]: keep all the IIL names
//
// Modification History:
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/TesterNew/IILMgrTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILStr.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"

const int sgMaxDocs = 1000000;
const OmnString sgSiteid = "100";
static char **sgValues;
static u64	sgDocids[sgMaxDocs];

AosIILMgrTester::AosIILMgrTester()
:
mVerifyGetObjidWeight(100),
mIILID(0)
{
	mName = "DocServerTester";
	// Initialize sgValues
	sgValues = new char*[sgMaxDocs];
	for (int i=0; i<sgMaxDocs; i++)
	{
		//sgValues[i] = new char[AosIILStr::eDftMaxStrLen+2];
	}

}


bool AosIILMgrTester::start()
{
	cout << "    Start OmnString Tester ..." << endl;
	basicTest();
	return true;
}


bool AosIILMgrTester::basicTest()
{
	mRunData =  OmnApp::getRundata();

	aos_assert_r(verifyObjidDuplicatedEntries(), false);
	aos_assert_r(verifyObjidIIL(), false);
	aos_assert_r(verifyAllIILS(5000, 600000), false);
	
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	iilname << sgSiteid;
	
	aos_assert_r(loadStrIIL(iilname), false);
	aos_assert_r(tortureNextDocidSafe(1000000, true, true), false);

	int tries = 1000;
	for (int i=0; i<tries; i++)
	{
		int opr = rand() % eOperationMax;
		switch (opr)
		{
		case eVerifyGetObjid:
		 	 aos_assert_r(verifyGetObjid(), false);
		 	 break;

		case eAddEntry:
			 aos_assert_r(tortureAddEntry(), false);
			 break;

		case eDeleteEntry:
			 aos_assert_r(tortureDeleteEntry(), false);
			 break;

		case eModifyEntry:
			 aos_assert_r(tortureModifyEntry(), false);
			 break;
		
		default:
		 	 OmnAlarm << "Unrecognized operation: " << opr << enderr;
		 	 break;
		}
	}
	return true;
}


bool
AosIILMgrTester::verifyObjidIIL()
{
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
	OmnScreen << "Start verifying Objid IIL ..." << endl;
	aos_assert_r(verifyValues(iilname, true, true), false);
	OmnScreen << "Start verifying Objid IIL ... Finished" << endl;
	return true;
}


bool
AosIILMgrTester::verifyValues(
		const OmnString &iilname, 
		const bool value_unique,
		const bool docid_unique)
{
	// It copies all the values and docids into sgValues and sgDocids. 
	// It then sequentially verify each and every entry entry.
	aos_assert_r(copyData(iilname), false);

	AosIILType iiltype = eAosIILType_Invalid;

//	const AosRundataPtr rdata =  OmnApp::getRundata();

//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);
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
AosIILMgrTester::loadStrIIL(const OmnString &iilname)
{
	AosIILType iiltype = eAosIILType_Invalid;
	if (mStriil)
	{
		AosIILMgr::getSelf()->returnIILPublic(mStriil,mRunData);
	}
	
	if(!mIILID)
	{
		mIILID = AosIILMgr::getSelf()->createIILPublic(
						eAosIILType_Str,
						false,
						mRunData);
	}

	AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);
	aos_assert_r(iil, false);

	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
//	mStriil = (AosIILStr *)iil.getPtr();
//	mIILName = iilname;
	return true;
}


bool
AosIILMgrTester::verifyObjidDuplicatedEntries()
{
	OmnScreen << "Check objid uniqueness ..." << endl;
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);

	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);


	mStriil = (AosIILStr *)iil.getPtr();
	aos_assert_r(mStriil->verifyDuplicatedEntries(mRunData), false);
	
	
	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	OmnScreen << "Check objid uniqueness done!" << endl;
	return true;
}


bool
AosIILMgrTester::verifyGetObjid()
{
	int tries = rand() % mVerifyGetObjidWeight;

	for (int i=0; i<tries; i++)
	{
		OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);

		//AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
		AosIILType iiltype = eAosIILType_Invalid;
		AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);

		aos_assert_r(iil, false);
		aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
		mStriil = (AosIILStr *)iil.getPtr();
		u64 docid;
		OmnString objid = mStriil->getRandomValue(docid,mRunData);
		aos_assert_r(objid != "", false);
//		bool duplicated;
//		AosXmlRc errcode; 
		OmnString errmsg;
		AosXmlTagPtr doc;
		//AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(sgSiteid, "", 
		//	eAOSDOCID_ROOT1, "", objid, duplicated, errcode, errmsg);
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
	return true;
}


bool
AosIILMgrTester::retrieveAllObjids()
{
	OmnString iilname = AosIILName::composeObjidListingName(sgSiteid);
//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);

	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);
	mStriil = (AosIILStr *)iil.getPtr();

	int num_subiils = mStriil->getNumSubiils();
	for (int i=0; i<num_subiils; i++)
	{
		AosIILStrPtr subiil = mStriil->getSubiilPublic(i,mRunData);
		aos_assert_r(subiil, false);
		u32 numdocs = subiil->getNumDocs();
		for (u32 aa=0; aa<numdocs; aa++)
		{
			OmnString objid = subiil->getValue(aa);
			aos_assert_r(objid != "", false);

//			bool duplicated;
//			AosXmlRc errcode;
			OmnString errmsg;
			AosXmlTagPtr doc;
			//AosXmlTagPtr doc = AosDocServer::getSelf()->getDoc(sgSiteid, "", 
			//	eAOSDOCID_ROOT1, "", objid, duplicated, errcode, errmsg);
			aos_assert_r(doc, false);
			aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == objid, false);
		}
	}

	AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
	return true;
}


bool
AosIILMgrTester::copyData(const OmnString &iilname)
{
//	AosIILPtr iil = AosIILMgr::getSelf()->getIILPublic(iilname);
	AosIILType iiltype = eAosIILType_Invalid;
	AosIILPtr iil;// = AosIILMgr::getSelf()->loadIILByIDPublic(mIILID,iiltype,mRunData);
	aos_assert_r(iil, false);
	aos_assert_r(iil->getIILType() == eAosIILType_Str, false);

	mStriil = (AosIILStr *)iil.getPtr();
	mNumDocs = sgMaxDocs;
	aos_assert_r(mStriil->copyData(sgValues, sgDocids, sgMaxDocs, mNumDocs,mRunData), false);
	return true;
}


bool
AosIILMgrTester::verifyNextDocidSafe(
		const bool reverse, 
		const bool value_unique) 
{
	OmnScreen << "Total Docs: " << mNumDocs << endl;
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
			bool rslt = mStriil->nextDocidSafe(
							idx, 
							iilidx, 
							reverse, 
							eAosOpr_eq, 
							value, 
							docid, 
							isunique,
							mRunData);
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
				bool rslt = mStriil->nextDocidSafe(
								idx, 
								iilidx, 
								reverse, 
								eAosOpr_eq, 
								value, 
								docid, 
								isunique,
								mRunData);
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

		// It found the entry [sgValues[aa], sgDocids[aa]]
		OmnString vv = mStriil->getValueSafe(idx, iilidx,mRunData);
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

	return true;
}


AosOpr
AosIILMgrTester::pickOperator()
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
AosIILMgrTester::tortureNextDocidSafe(
		const int tries,
		const bool value_unique, 
		const bool docid_unique)
{
	// This function randomly tests the given list
	OmnScreen << "Start torturing nextDocid ..." << endl;
	aos_assert_r(copyData(mIILName), false);
	int group = 0;
	for (int i=0; i<tries; i++)
	{
		if (group++ == 10000)
		{
			cout << ":" << i << flush;
			group = 0;
		}

		// 1. Determine the entry 
		int entry = rand() % mNumDocs;

		// 2. Determine the operator
		AosOpr opr = pickOperator();

		// 3. Determine reverse
		bool reverse = rand() % 2;

		// 4. Ready to run
		OmnString value = sgValues[entry];

opr = eAosOpr_eq;

		switch (opr)
		{
		case eAosOpr_ge:
			 aos_assert_r(searchGE(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;

		case eAosOpr_eq:
			 aos_assert_r(searchEQ(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;
				 
		case eAosOpr_gt:
			 aos_assert_r(searchGT(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;

		case eAosOpr_lt:
			 aos_assert_r(searchLT(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;

		case eAosOpr_le:
			 aos_assert_r(searchLE(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;

		case eAosOpr_ne:
			 aos_assert_r(searchLE(value, reverse, value_unique, docid_unique, 
					sgDocids[entry]), false);
			 break;

		default:
			 OmnAlarm << "Invalid operator: " << opr << enderr;
			 break;
		}
	}

	OmnScreen << "Start torturing nextDocid ... Done!" << endl;
	return true;
}


bool
AosIILMgrTester::searchGE(
		const OmnString &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid)
{
	if (reverse)
	{
		aos_assert_r(searchReverseGE(value, 
			value_unique, docid_unique, expected_docid), false);
		return true;
	}
	
	aos_assert_r(searchNormalGE(value, 
			value_unique, docid_unique, expected_docid), false);
	return true;
}


bool
AosIILMgrTester::searchReverseGE(
		const OmnString &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
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
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(
						idx, 
						iilidx, 
						true, 
						eAosOpr_ge, 
						value, 
						docid, 
						isunique,
						mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			OmnAlarm << "Failed: " << ":" << value << ":" 
				<< docid << enderr;
			return false;
		}

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, continue searching.
		if (mStriil->getValueSafe(idx, iilidx,mRunData) != value)
		{
			// Did not find the one we want. Keep on searching
			continue;
		}

		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			// If we continue searching, it should find no more.
			aos_assert_r(docid == expected_docid, false);
			rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
					eAosOpr_ge, value, docid, isunique,mRunData);
			aos_assert_r(rslt, false);
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			aos_assert_r(docid == AOS_INVDID, false);
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
AosIILMgrTester::searchNormalGE(
		const OmnString &value, 
		const bool value_unique,
		const bool docid_unique,
		const u64 &expected_docid)
{
	// This function assumes that it searches in the normal order
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
	while (1)
	{
		// Retrieve the docid
		rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
				eAosOpr_ge, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
		
		// Check whether it finds anything
		if (docid == AOS_INVDID)
		{
			// It failed the testing.
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			OmnAlarm << "Failed: " << ":" << value << ":" 
				<< docid << enderr;
			return false;
		}

		// It found a valid entry. Check whether the found value
		// is 'value'. If not, it is an error.
		aos_assert_r(mStriil->getValueSafe(idx, iilidx,mRunData) == value, false);

		// Found an entry whose value matches 'value'. If 'value_unique'
		// is true, 'docid' must be 'expected_docid'.
		if (value_unique)
		{
			// If we continue searching, it should find no more.
			aos_assert_r(docid == expected_docid, false);
			rslt = mStriil->nextDocidSafe(idx, iilidx, true, 
					eAosOpr_ge, value, docid, isunique,mRunData);
			aos_assert_r(rslt, false);
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			aos_assert_r(docid == AOS_INVDID, false);
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
AosIILMgrTester::searchEQ(
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

	// OmnScreen << "Search EQ: " << value << ":" << expected_docid << endl;
	// Ready to search the list
	rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
			eAosOpr_eq, value, docid, isunique,mRunData);
	aos_assert_r(rslt, false);

	aos_assert_r(idx >= 0, false);
	aos_assert_r(iilidx >= 0, false);
	aos_assert_r(mStriil->getValueSafe(idx, iilidx,mRunData) == value, false);

	if (value_unique)
	{
		// It should find it. Otherwise, it is an error. 
		aos_assert_r(isunique, false);
		aos_assert_r(docid == expected_docid, false);

		// Searching next should find none
		rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
				eAosOpr_eq, value, docid, isunique,mRunData);
		aos_assert_r(docid == AOS_INVDID, false);
		aos_assert_r(idx == -5, false);
		aos_assert_r(iilidx == -5, false);
		// OmnScreen << "Done!" << endl;
		return true;
	}

	u64 prev_docid = 0;
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
				 	rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
					 			eAosOpr_ge, value, docid, isunique,mRunData);
					aos_assert_r(rslt, false);
					aos_assert_r(docid != expected_docid, false);
					if (docid == AOS_INVDID)
					{
						// Finished
						aos_assert_r(idx == -5, false);
						aos_assert_r(iilidx == -5, false);
						// OmnScreen << "Done!" << endl;
						return true;
					}
				}
			}
			// OmnScreen << "Done!" << endl;
			return true;
		}

		if (docid == AOS_INVDID)
		{
			// It failed the searching
			aos_assert_r(idx == -5, false);
			aos_assert_r(iilidx == -5, false);
			OmnAlarm << "Failed: " << value << ":" << docid << enderr;
			return false;
		}

		// aos_assert_r(expected_docid <= docid, false);

		// It found one record but not equal to 'expected_docid'.
		if (reverse)
		{
			if (prev_docid != 0) aos_assert_r(docid <= prev_docid, false);
		}
		else
		{
			if (prev_docid != 0) aos_assert_r(prev_docid <= docid, false);
		}

		prev_docid = docid;
		rslt = mStriil->nextDocidSafe(idx, iilidx, reverse, 
				eAosOpr_eq, value, docid, isunique,mRunData);
		aos_assert_r(rslt, false);
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosIILMgrTester::searchGT(
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
AosIILMgrTester::searchReverseGT(
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
		OmnString crt_value = mStriil->getValueSafe(idx, iilidx,mRunData);
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
AosIILMgrTester::searchNormalGT(
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

	OmnString prev_value = mStriil->getValueSafe(idx, iilidx,mRunData);

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

		OmnString crt_value = mStriil->getValueSafe(idx, iilidx,mRunData);
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


bool
AosIILMgrTester::searchLT(
		const OmnString &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILMgrTester::searchLE(
		const OmnString &value, 
		const bool reverse,
		const bool value_unique, 
		const bool docid_unique,
		const u64 &expected_docid)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILMgrTester::verifyAllIILS(
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
			if (iil->isRootIIL() || iil->isSingleIIL())
			{
				switch (iiltype)
				{
				case eAosIILType_Str:
					 aos_assert_r(verifyStrIIL((AosIILStr*)iil.getPtr()), false);
					 break;

				default:
					 break;
				}
			}
			AosIILMgr::getSelf()->returnIILPublic(iil,mRunData);
		}
	}

	return true;
}


bool
AosIILMgrTester::verifyStrIIL(const AosIILStrPtr &iil)
{
	mNumDocs = sgMaxDocs;
	aos_assert_r(iil->copyData(sgValues, sgDocids, sgMaxDocs, mNumDocs,mRunData), false);
	
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
AosIILMgrTester::tortureAddEntry()
{
	//1. have a iil
	//2. tell which class iil
	//3. add value
	//4. record value
	//5. verify this operation
	//
	return true;
}


bool
AosIILMgrTester::tortureDeleteEntry()
{
	return true;
}


bool
AosIILMgrTester::tortureModifyEntry()
{
	return true;
}


