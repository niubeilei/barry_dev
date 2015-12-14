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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/TesterNew/DocMgrTester.h"

#include "SearchEngine/DocMgr.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
/*
const OmnString sgSiteid = "100";
const u64		mMinDocid = 5000;
const u64		mMaxDocid = 150000;


AosDocMgrTester::AosDocMgrTester()
{
	mName = "DocMgrTester";
}


bool 
AosDocMgrTester::start()
{
	cout << "Start AosDocMgr Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDocMgrTester::basicTest()
{
	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		if (i % 1000 == 0)
		{
			cout << " " << i << endl;
		}
		Operation opr = pickOperation();
		switch (opr)
		{
		case eGetDocByDocid:
			 aos_assert_r(getDocByDocId(), false);
			 break;

		case eGetDocByDocid1:
			 aos_assert_r(getDocByDocId1(), false);
			 break;

		case eGetDocByObjid:
			 aos_assert_r(getDocByObjId(), false);
			 break;

		case eGetDocByCloudid:
			 aos_assert_r(getDocByCloudid(), false);
			 break;

		case eGetUserAcct:
			 aos_assert_r(getUserAccount(), false);
			 break;

		case eAddCopy1:
		case eAddCopy2:
			 aos_assert_r(addCopy(), false);
			 break;

		case eDeleteDoc:
			 aos_assert_r(deleteDoc(), false);
			 break;

		case eAddDoc1:
		case eAddDoc2:
		case eAddDoc3:
		case eAddDoc4:
			 aos_assert_r(addDoc(), false);
			 break;

		case eGetAccessRcd:
			 aos_assert_r(getAccessRecord(), false);
			 break;

		case eGetParentAR:
			 aos_assert_r(getParentAR(), false);
			 break;

		case eGetAccessRcdBySystem:
			 aos_assert_r(getAccessRcdBySystem(), false);
			 break;

		default:
			 OmnAlarm << "Unrecognized operation: " << opr << enderr;
			 return false;
		}
	}
	cout << endl;
	return true;
}


bool
AosDocMgrTester::getDocByDocId()
{
	// This function tests AosDocMgr::getDoc(const u64 &docid, const OmnString &objid)
	// It randomly picks a docid. It then calls the doc manager
	// to retrieve the doc. 
	u64 docid = (rand() % (mMaxDocid - mMinDocid)) + mMinDocid;

	// Check whether the doc exist. 
	AosXmlTagPtr orig_doc = AosXmlDoc::readFromFile(docid);
	OmnString objid;
	if (orig_doc)
	{
		objid = orig_doc->getAttrStr(AOSTAG_OBJID);
		aos_assert_r(objid != "", false);
	}
	
	AosXmlTagPtr doc;
	if (rand() % 2)
	{
		doc = AosDocMgr::getSelf()->getDoc(docid, "");
	}
	else
	{
		doc = AosDocMgr::getSelf()->getDoc(docid, objid);
	}

	if (!orig_doc)
	{
		aos_assert_r(!doc, 0);
		return true;
	}

	aos_assert_r(doc, false);
	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) == objid, false);
	aos_assert_r(verifyAndAddDoc(docid, doc, objid), false);
	aos_assert_r(verifyDocs(), false);

	AosXmlTagPtr doc2 = AosDocMgr::getSelf()->getDoc(docid, "");
	AosXmlTagPtr doc3 = AosDocMgr::getSelf()->getDoc(docid, objid);
	aos_assert_r(doc2 == doc, false);
	aos_assert_r(doc2 == doc3, false);
	return true;
}


bool
AosDocMgrTester::getDocByDocId1()
{
	// This function tests AosDocMgr::getDocByDocId(const OmnString &docid, 
	// const OmnString &objid)
	// It randomly picks a docid. It then calls the doc manager
	// to retrieve the doc. 
	//
	// Refer to the implementation of getDocByDocId()
	OmnNotImplementedYet;
	return false;
}


bool
AosDocMgrTester::getDocByObjId()
{
	// This function randomly pick a docid. It then calls the doc manager
	// to retrieve the doc. 
	OmnString objid = pickObjid();
	AosXmlTagPtr doc = AosDocMgr::getSelf()->getDoc(sgSiteid, objid);
	if (!doc) return true;

	u64 docid = doc->getAttrInt("zky_docid", -1);	

	aos_assert_r(verifyAndAddDoc(docid, doc, objid), false);
	aos_assert_r(verifyDocs(), false);
	return true;
}


bool
AosDocMgrTester::verifyAndAddDoc(
		const u64 &docid, 
		const AosXmlTagPtr &doc, 
		const OmnString &objid)
{
	// This function checks whether the doc is in the array. 
	// If yes, 'doc' and the one in the array should be the
	// same pointer. Otherwise, it is an error. If it is 
	// correct, and if the doc is in the array, it is moved
	// to the front of the array.
	//
	// It will add the objid into mObjids.
	
	aos_assert_r(docid > 0 && objid != "", false);
	for (u32 i=0; i<mNumDocs; i++)
	{
		if (mDocids[i] == docid)
		{
			// The doc is in the list. Move the doc to the front.
			aos_assert_r(mDocs[i] == doc, false);
			memmove(&mDocs[1], mDocs, i * sizeof(AosXmlTagPtr));
			memmove(&mDocids[1], mDocids, i * sizeof(u64));
			mDocs[0] = doc;
			mDocids[0] = docid;
			return true;
		}
	}
	
	// It is not in the array. Need to add it.
	if (mNumDocs >= AosDocMgr::eMaxNumDocs2Cache)
	{
		// Too many in the cache. Need to remove the last one
		memmove(&mDocs[1], mDocs, (mNumDocs-1) * sizeof(AosXmlTagPtr));
		memmove(&mDocids[1], mDocids, (mNumDocs-1) * sizeof(u64));
		mDocs[0] = doc;
		mDocids[0] = docid;
		return true;
	}

	// Add it.
	memmove(&mDocs[1], mDocs, mNumDocs * sizeof(AosXmlTagPtr));
	memmove(&mDocids[1], mDocids, mNumDocs * sizeof(u64));
	mDocs[0] = doc;
	mDocids[0] = docid;
	mNumDocs++;

	mObjids.push_back(objid);
	return true;
}


bool
AosDocMgrTester::verifyDocs()
{
	// This function verifies two things: the total number of
	// docs and the last doc. 
	u64 docNum =  AosDocMgr::getSelf()->getDocNum();
	aos_assert_r(docNum == mObjids.size(), false);

	AosDocLink* head = AosDocMgr::getSelf()->getDocHead();
	head = head->n;
	for (u32 i=0; i<mObjids.size(); i++)
	{
		aos_assert_r(head, false);
		aos_assert_r(mDocs[i] == head->doc, false);
		aos_assert_r(mDocids[i] == head->docId, false);
		head = head->n;
	}
	return true;
}


bool
AosDocMgrTester::getDocByCloudId()
{
	// This function tortures AosDocMgr::getDocByCLoudId(...)
	// All cloud ids are retrieved and stored in mCloudids. 
	// This function randomly pick a cloud id from the array, 
	// calls the function to retrieve the doc, and then compares
	// the doc. 
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::getUserAccount()
{
	// This function tortures AosDocMgr::getUserAcct(const u64 &userid) and
	// AosDocMgr::getUserAcct(const OmnString &userid)
	//
	// All user accounts are stored in mUserDocids. 
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::addCopy()
{
	// This function tortures AosDocMgr::addCopy(const u64 &docid, 
	// const AosXmlTagPtr &doc, const bool)
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::deleteDoc()
{
	// This function tortures AosDocMgr::deleteDoc(const u64 &docid)
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::addDoc()
{
	// This function tortures AosDocMgr::deleteDoc(
	// 	const AosXmlTagPtr &doc, 
	// 	const u64 &docid, 
	// 	const bool lockflag)
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::getAccessRecord()
{
	// This function tortures AosDocMgr::getAccessRcd(
	// 	const OmnString &siteid, 
	// 	const u64 &docid, 
	// 	const OmnString &ref_objid, 
	// 	const int ttl)
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::getParentAR()
{
	// This function tortures AosDocMgr::getParentAR1(
	// 	const OmnString &siteid, 
	// 	const u64 &docid, 
	// 	const OmnString &objid, 
	// 	const int ttl)
	OmnNotImplementedyet;
	return false;
}


bool
AosDocMgrTester::getAccessRcdBySystem()
{
	// This function tortures AosDocMgr::getAccessRcdBySystem(
	// 	const OmnString &siteid, 
	// 	const AosXmlTagPtr &ref_doc,
	// 	const u64 &docid,
	// 	const OmnString &objid,
	// 	const bool parent_flag,
	// 	int &ttl)
	//
	//
	return false;
}


bool
AosDocMgrTester::getAllCloudids()
{
	idx = -10;
	iilidx = -10;
	OmnString cid;
	u64 docid;
	while (1)
	{
		aos_assert_r(AosIILClient::getSelf()->pickNextDoc(
				AOSZTG_CLOUDID, idx, iilidx, false, cid, docid), false);
		if (idx < 0 || iilidx < 0)
		{
			return true;
		}

		aos_assert_r(cid != "", false);
		aos_assert_r(docid, false);
		mCloudids.push_back(cid);
		mUserDocids.push_back(docid);
	}

	return true;
}

*/
