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
// This class runs as a separate thread. It randomly sleeps for a 
// a period of time. When it wakes up, it creates a query and query
// the system.
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/DocThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SEClient.h"
#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosDocThrd::AosDocThrd(
		const AosSearchEngTesterPtr &torturer, 
		const OmnIpAddr &raddr, 
		const int rport, 
		const int total,
		const int weight)
:
mTorturer(torturer),
mTotal(total),
mNumDocs(0),
mWeight(weight)
{
	if (mWeight <= 0) mWeight = eDftWeight;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "DocThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosDocThrd::~AosDocThrd()
{
}


bool
AosDocThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosDocThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		int wtime = rand() % mWeight;
		OmnWaitSelf->wait(wtime);
		verifyDocs();
	}
	return true;
}


bool
AosDocThrd::verifyDocs()
{
	/*
	// It randomly reads one doc from the database:
	AosXmlTagPtr doc = mTorturer->readDoc();
	aos_assert_r(doc, false);
	AosXmlTagPtr child = doc->getFirstChild();
	aos_assert_r(child, false);

	OmnString docstr((char *)doc->getData());
	aos_assert_r(docstr != "", false);
	OmnString docid = child->getAttrStr("docid", "");
	aos_assert_r(docid != "", false);

	// 1. For each attribute in the doc, do:
	// 		select docid where attrname = "value"
	//    This query may pick multiple docs, but it should be
	//    able to pick the docid.
	// 2. For each attribute:
	//      select docid where docid = "xxx" and attrname = "xxx"
	// 3. For each attribute and each word in the attribute, 
	//      select docid where attrname ctn-word <word>
	aos_assert_r(verifyAttrs(docid), false);
	aos_assert_r(verifyWords(docid), false);
	aos_assert_r(verifyCtnrs(child), false);
	aos_assert_r(verifyRawDoc(docstr, docid), false);
	*/
	return true;
}


bool
AosDocThrd::signal(const int threadLogicId)
{
	return true;	
}


bool    
AosDocThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDocThrd::verifyWords(const OmnString &docid)
{
	// For each word in the doc, if the word is not ignored,
	// it should be able to use the word to retrieve the doc:
	//      select docid where word and docid = "xxx"
	
	/*
	// Read in all the words
	int start = 0;
	int size = 1000;
	OmnRslt rslt;
	AosSEClient conn(mRemoteAddr, mRemotePort);
	OmnString errmsg;
	OmnString respstr;
	AosXmlParser parser;
	while (1)
	{
		OmnString stmt = "select word from ";
		stmt << mDocwdTname << " where docid='" 
			<< docid << "' limit "
			<< start << ", " << size;
		OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
		OmnDbTablePtr table;
		aos_assert_r(store->query(stmt, table), false);	
		aos_assert_r(table, false);
		start += size;

		if (table->entries() == 0) break;

		table->reset();
		while (table->hasMore())
		{
			OmnDbRecordPtr record = table->next();
			OmnString word = record->getStr(0, "", rslt);

			OmnString query = "select docid where ";
			query << word << " and $docid = '" << docid << "'";

			aos_assert_r(conn.procRequest(query, respstr, errmsg), false);
			AosXmlTagPtr resp = parser.parse(respstr, "");
			aos_assert_r(resp, false);
			OmnString code = resp->getAttrStr("code", "");
			aos_assert_r(code == "200" || code == AOSCODE_WORDIGNORED, false);
		}
	}
	*/

	return true;
}


bool
AosDocThrd::verifyAttrs(const OmnString &docid)
{
	// For each attribute in the doc,
	// it should be able to:
	//      select docid where attrname = "value"
	
	/*
	int start = 0;
	int size = 1000;
	OmnRslt rslt;
	AosSEClient conn(mRemoteAddr, mRemotePort);
	OmnString errmsg;
	OmnString respstr;
	AosXmlParser parser;
	while (1)
	{
		OmnString stmt = "select aname, avalue from ";
		stmt << mAttrsTname << " where docid='" 
			<< docid << "' limit "
			<< start << ", " << size;
		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		OmnDbTablePtr table;
		aos_assert_r(store->query(stmt, table), false);	
		aos_assert_r(table, false);
		start += size;

		if (table->entries() == 0) break;

		table->reset();
		while (table->hasMore())
		{
			OmnDbRecordPtr record = table->next();
			OmnString aname  = record->getStr(0, "", rslt);
			OmnString avalue = record->getStr(1, "", rslt);

			OmnString query = "select docid where ";
			query << aname << " = '" << avalue << "'";

			aos_assert_r(conn.procRequest(query, respstr, errmsg), false);
			AosXmlTagPtr resp = parser.parse(respstr, "");
			aos_assert_r(resp, false);
			AosXmlTagPtr child = resp->getFirstChild();
			aos_assert_r(child, false);

			OmnString code = child->getAttrStr("code", "");
			aos_assert_r(code == "200", false);

			OmnString total = child->getAttrStr("total", "");
			OmnString num = child->getAttrStr("num", "");
			aos_assert_r(total == "1", false);
			aos_assert_r(num == "1", false);

			AosXmlTagPtr rcd = child->getFirstChild();
			aos_assert_r(rcd, false);
			OmnString did = rcd->getAttrStr("docid", "");
			aos_assert_r(did == docid, false);
		}
	}
	*/

	return true;
}


bool
AosDocThrd::verifyRawDoc(
		const OmnString &doc1, 
		const OmnString &docid)
{
	// It retrieves the doc from the server and then compares 
	// the returned doc with 'doc1'. They should be the same.
	
	/*
	OmnString query = "select * where docid = '";
	query << docid << "'";

	OmnString errmsg;
	OmnString respstr;
	aos_assert_r(mConn->procRequest(query, respstr, errmsg), false);
	aos_assert_r(respstr != "", false);
	AosXmlParser parser;
	AosXmlTagPtr doc2 = parser.parse(respstr, "");
	aos_assert_r(doc2, false);

	// The response should be:
	// 		<Contents ...>
	// 			<xmlobj>
	// 				<the object itself>
	// 			</xmlobj>
	// 		</Contents>
	AosXmlTagPtr child = doc2->getFirstChild();
	aos_assert_r(child, false);
	child = child->getFirstChild("xmlobj");
	aos_assert_r(child, false);
	child = child->getFirstChild();
	aos_assert_r(child, false);

	AosXmlTagPtr root = parser.parse(doc1, "");
	aos_assert_r(root, false);
	AosXmlTagPtr child2 = root->getFirstChild();
	aos_assert_r(child2, false);
	aos_assert_r(child->isDocSame(child2), false);
	*/
	return true;
}


bool
AosDocThrd::verifyCtnrs(const AosXmlTagPtr &doc)
{
	// For each container to which the doc belongs, it does:
	//      select docid from <container> where $docid = "xxx"
	
	/*
	OmnString docid = doc->getAttrStr("docid", "");
	aos_assert_r(docid != "", false);
	OmnString ctnrs = doc->getAttrStr(AOSTAG_CONTAINER, "");
	aos_assert_r(ctnrs != "", false);

	OmnStrParser strparser(ctnrs);
	OmnString ctnr;
	OmnConnBuffPtr conn;
	OmnString errmsg;
	OmnString respstr;
	AosXmlParser parser;
	while ((ctnr = strparser.nextWord("", ", ", true)) != "")
	{
		OmnString query = "select docid from ";
		query << ctnr << " where $docid = '" << docid << "'";
		aos_assert_r(mConn->procRequest(query, respstr, errmsg), false);
		aos_assert_r(respstr != "", false);
		AosXmlTagPtr root = parser.parse(respstr, "");
		aos_assert_r(root, false);
		AosXmlTagPtr child = root->getFirstChild();
		aos_assert_r(child, false);

		OmnString code = child->getAttrStr("code", "");
		aos_assert_r(code == "200", false);

		OmnString total = child->getAttrStr("total", "");
		OmnString num = child->getAttrStr("num", "");
		aos_assert_r(total == "1", false);
		aos_assert_r(num == "1", false);

		AosXmlTagPtr rcd = child->getFirstChild();
		aos_assert_r(rcd, false);
		OmnString did = rcd->getAttrStr("docid", "");
		aos_assert_r(did == docid, false);
	}
	*/

	return true;
}

