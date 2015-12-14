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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorturer/TesterReadObj.h"

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

# if 0
AosReadObjTester::AosReadObjTester(const OmnString &weight_tagname):
AosSengTester(weight_tagname)
{
	AosSengTesterPtr thisptr(this, false);
	registerTester(eReadObjTester, thisptr);
}


AosReadObjTester::~AosReadObjTester()
{
}


bool 
AosReadObjTester::test()
{
	// This function randomly reads a doc from 
	// the server. 
	int tid = mThread->getThreadId();
	OmnString siteid = mThread->getSiteid();
	/*
	u64 max_docid = mThread->getCrtMaxDocid();
	if (max_docid == AosSengTestThrd::eMaxDocs+1) return true;
//	while (1)
//	{
		if (max_docid ==0) local_docid =0;
		else   local_docid = (rand() % max_docid) +1;
OmnScreen<<" local docid:"<< local_docid <<endl;
		server_docid = mThread->getServerDocid(local_docid);
//	}
//	*/
	const AosStDoc *doc = mThread->pickDoc();
	u64 local_docid = doc->locdid;
	u64 sdocid = doc->svrdid;

	AosTestFileMgrPtr ff = mgr->getFileMgr();
	//Retrieve the newly created doc from local
	AosXmlTagPtr local_doc;
	ff->readDoc(tid,local_docid, local_doc);

	// Retrieve the newly created doc from server
	OmnString ssid = mThread->getCrtSsid();
	AosXmlTagPtr server_doc =
		 AosSengAdmin::getSelf()->retrieveDocByDocid(siteid, ssid, sdocid);
	aos_assert_r(server_doc, false);

	//比较
	bool rslt = server_doc->isDocSame(local_doc);
	aos_assert_r(rslt, false);

	return true;
}
/*

bool 
AosReadObjTester::ReadObj(
		const AosSengTestThrdPtr &thread,
		const AosSengTesterMgrPtr &mgr,
		const OmnString &ldocid)
{
	int tid = thread->getThreadId();
	OmnString server_docid;
	AosTestFileMgrPtr ff = mgr->getFileMgr();
	OmnString siteid = thread ->getSite();
	//Retrieve the newly created doc from local
	u64 local_docid = atoll(ldocid);
	AosXmlTagPtr local_doc;
	ff->readDoc(tid, local_docid, local_doc);

	server_docid = local_doc->getAttrStr(AOSTAG_DOCID);

	struct AccessRecord ar = thread->getAccessRecord(local_docid);
	u64 ar_docid = ar.docid; 
	//bool canRead = checkCreate();
	u64 sdocid = atoll(server_docid);
	// Retrieve the newly created doc from server
	AosXmlTagPtr server_doc =
		 AosSengAdmin::getSelf()->retrieveDocByDocid(siteid, sdocid);
	aos_assert_r(server_doc, false);

	//比较
	bool rslt = server_doc->isDocSame(local_doc);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosCreateCtnrTester::checkRead(
		AosXmlTagPtr &ar,
		OmnString &ssid,
		OmnString &userid,
		AosXmlTagPtr arobj,
		const AosSengTestThrdPtr &thread,
		const AosSengTesterMgrPtr &mgr)
{
	//1. whether checkLogin
	if (ssid =="") return false;

	//2.Retrieve the user account record
	OmnString opr = "Read";
	OmnString accesses = ar.getAttrStr(AOSTAG_READ_ACSTYPE);
	if (accesses == "")
	{
		return false;
	}

	OmnStrParser parser(accesses, ",");
	OmnString word;
	while ((word = parser.nextWord()) != "")
	{
		 switch (word)
		 {
		 	case "private":
				//It checks whether the requester owns the requested.
				if (!arobj) return false;
				//判断此条arcess是否是doc的accessRecord;
				u64 docid = ar->getAttrU64(AOSTAG_OWNER_DOCID, AOS_INVDID);
				if (docid == AOS_INVDID) return false;
				if (docid == userid) return true;
				//Check Document creator is the same with Requester
				u64 createtor = arobj->getAttrU64(AOSTAG_CREATOR, AOS_INVDID);
				if (createtor == AOS_INVDID) return false;
				if (createtor == userid) return true;
				break;
			case "group":
				break;
			case "role":
				break;
			case "lcpublic":
				if (userid != AOS_INVDID) return true;
				break;
			case "public":
				return true;
				break;
			default:
				return false;
		 }
	}
	
}
*/
#endif
