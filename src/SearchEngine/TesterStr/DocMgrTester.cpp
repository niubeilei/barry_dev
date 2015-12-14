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
#include "SearchEngine/TesterStr/DocMgrTester.h"

#include "SearchEngine/DocMgr.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

const OmnString sgSiteid = "100";
const u64		mMinDocid = 5000;
const u64		mMaxDocid = 120000;


AosDocMgrTester::AosDocMgrTester()
{
	mName = "DocMgrTester";
	mDocHead = new docLink;
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
	int idx = 0;
	for (int i=0; i<tries; i++)
	{
		if (idx == 100)
		{
			cout << " *****Errors***** " << mErrors << endl;
			idx = 0;
		}
		else
		{
			idx++;
		}
		Operation opr = pickOperation();
		switch (opr)
		{
		case eGetDocByDocid:
			 aos_assert_r(getDocByDocId(), false);
			 break;

		case eGetDocByObjid:
			 aos_assert_r(getDocByObjId(), false);
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
	// This function randomly pick a docid. It then calls the doc manager
	// to retrieve the doc. 
	u64 docid = (rand() % (mMaxDocid - mMinDocid)) + mMinDocid;
	AosXmlTagPtr doc = AosDocMgr::getSelf()->getDocByDocId(docid);
	if (!doc) return true;
	OmnString objid = doc->getAttrStr("zky_objid","");	

//	cout << "DocMgrTester.cpp:98----->objid:  " << objid << " docid: " << docid << endl;

	aos_assert_r(verifyAndAddDoc(docid, doc, objid), false);
	aos_assert_r(verifyLastDoc(), false);
	return true;
}


bool
AosDocMgrTester::getDocByObjId()
{
	// This function randomly pick a docid. It then calls the doc manager
	// to retrieve the doc. 
	OmnString objid = pickObjid();
	AosXmlTagPtr doc = AosDocMgr::getSelf()->getDocByObjId(sgSiteid, objid);
	if (!doc) return true;

	u64 docid = doc->getAttrInt("zky_docid",-1);	

	aos_assert_r(verifyAndAddDoc(docid, doc, objid), false);
	aos_assert_r(verifyLastDoc(), false);
	return true;
}

bool
AosDocMgrTester::verifyAndAddDoc(u64 docid, const AosXmlTagPtr &doc, OmnString objid)
{
	// This function checks whether the doc is in the array. 
	// If yes, 'doc' and the one in the array should be the
	// same pointer. Otherwise, it is an error. If it is 
	// correct, and if the doc is in the array, it is moved
	// to the front of the array.
	//
	// It will add the objid into the hash table and the 
	// objid array.
	if(docid < 0 || objid == "")
	{
		OmnAlarm << "--------------------------no docid or objid----------------" << enderr;
		mErrors++;
		return false;
	}

	docLink* node;

	map<u64, docLink*>::iterator iter;
	iter = mDocMap.find(docid);
	if(iter != mDocMap.end())
	{
		node = mDocMap[docid];
		if(node->doc != doc)
		{
			mErrors++;
			OmnAlarm << "Duplicated copies are found!" << enderr;
		}
		addNode(node, doc, docid, false);
	}
	else
	{
		node = new docLink;
		node->doc = doc;
		node->docId = docid;
		addNode(node, doc, docid, true);
	}

	if(mNumObjids < eMaxObjids)
		mObjids[mNumObjids++] = objid;

	return true;
}


bool
AosDocMgrTester::verifyLastDoc()
{
	// This function verifies two things: the total number of
	// docs and the last doc. 
	u64 docNum =  AosDocMgr::getSelf()->getDocNum();
	if(docNum != mDocMap.size())
	{
		mErrors++;
		OmnAlarm << "Error: ---------The total number of docs is not the same! " << enderr;
	}
	AosXmlTagPtr lastDoc =  AosDocMgr::getSelf()->getLastDoc();
	if(mDocHead->p->doc != lastDoc)
	{	
		mErrors++;
		OmnAlarm << "Error:------------------lastDoc is not the same! " << enderr;
	}
	return true;
}


bool
AosDocMgrTester::addNode(docLink* node, AosXmlTagPtr doc, u64 docid, bool flag)
{
	if(mNumDocs == 0)
	{ 
		mDocHead->n = node;
		mDocHead->p = node;
		node->p = mDocHead;
		node->n = mDocHead;
	}
	else
	{
		if(mNumDocs != 1)
		{
			if(mDocMap.size() >= MAX_NUM && flag)
			{
				node = mDocHead->p;
				mDocMap.erase(node->docId);
				node->docId = docid;
				node->doc = doc;
				mDocHead->p = mDocHead->p->p;
				mDocHead->p->n = mDocHead;
			}
			node->p = mDocHead;
			node->n = mDocHead->n;
			mDocHead->n->p = node;
			mDocHead->n = node;
		}
	}
	if(flag)
		mDocMap[docid] = node;
	mNumDocs++;
	return true;
}
