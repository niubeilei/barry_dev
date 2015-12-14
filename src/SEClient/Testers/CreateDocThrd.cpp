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
// a period of time. When it wakes up, it creates a new doc. After
// that, it sleeps again, until it creates a given number of docs. 
//   
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEClient/Testers/CreateDocThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SEClient.h"
#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "SEClient/Testers/TestXmlDoc.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosCreateDocThrd::AosCreateDocThrd(
		const AosSearchEngTesterPtr &torturer,
		const int total, 
		const int weight, 
		const OmnIpAddr &raddr, 
		const int rport)
:
mTorturer(torturer),
mTotal(total),
mWeight(weight),
mNumDocs(0),
mIsCreating(false)
{
	mConn = OmnNew AosSEClient(raddr, rport);
	if (mWeight <= 0) mWeight = eDftWeight;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "CreateDocThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosCreateDocThrd::~AosCreateDocThrd()
{
}


bool
AosCreateDocThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosCreateDocThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mIsCreating = false;
    while (state == OmnThrdStatus::eActive)
    {
		int wtime = rand() % mWeight;
		if (wtime < eMinWaitMsec) wtime = eMinWaitMsec;
		OmnWaitSelf->wait(wtime);
		if (mNumDocs < mTotal) 
		{
			if (!mTorturer->isUpdateStopped()) 
			{
				mIsCreating = true;
				// createDoc(-3);
				createDoc(-10);
				// createDoc(0);
				// createDoc(1);
				// createDoc(2);
				// createDoc(3);
				// return true;
				mIsCreating = false;
			}
		}
	}
	return true;
}


bool
AosCreateDocThrd::createDoc(const int dd)
{
	// This function adds a new doc into the system. The doc
	// is randomly generated. 

	// 1. Generate the doc
	int depth = dd;
	if (depth == -10) depth = rand() % eDepth;
	AosTestXmlDocPtr doc = OmnNew AosTestXmlDoc("", depth);
	aos_assert_r(doc, false);
	mNumDocs++;
	OmnScreen << "Create doc: " << mNumDocs << endl;

	// Need to construct a request, which is in the form:
	//	<request>
    //		<item name='username'>chending</item>
    //		<item name='appname'>testapp</item>
	//		<xmlobj>
	//			the xml generate above
	//		</xmlobj>
	//	</request>

	// Convert the generated XML into a string
	OmnString docstr;
	doc->composeData(docstr);

	// Add the header to the generated xml
	OmnString hh = "<request><item name='operation'>serverCmd</item>";
	mAppname = "app";
	mAppname << mNumDocs;
	mCtnr = "container_";
	mCtnr << mNumDocs;
	mUser = "User_";
	mUser << mNumDocs;
	hh << "<item name='appname'>"
		<< mAppname << "</item><item name='" << AOSTAG_USERNAME << "'>"
		<< mUser << "</item><item name='" << AOSTAG_SITEID << "'>"
		<< doc->getSiteid() << "</item><item name='" << AOSTAG_HPCONTAINER << "'>"
		<< mCtnr << "</item><item name='" << AOSTAG_OBJID << "'>"
		<< doc->getObjid() << "</item>"
		<< "<cmd opr='createcobj'/>"
		<< "<xmlobj>"
		<< docstr << "</xmlobj></request>";

	OmnString errmsg;
	OmnString resp;
	// OmnScreen << "To create: " << hh << endl;
	aos_assert_r(mConn->procRequest(doc->getSiteid(), 
			mAppname, mUser, hh, resp, errmsg), false);
	// OmnScreen << "Response: " << resp << endl;
	aos_assert_r(resp != "", false);

	// Check the response. Response should be in the form:
	// 	<response error="false" 
	// 		code="200"
	// 		docid="xxx"/>
	AosXmlDocPtr header;
	AosXmlParser parser;
	AosXmlTagPtr resproot = parser.parse(resp, "");
	AosXmlTagPtr child = resproot->getFirstChild();
	aos_assert_r(child, false);
	bool exist;
	OmnString docid = child->xpathQuery("Contents/docid", exist, "");
	if (docid == "")
	{
		OmnAlarm << "Failed to retrieve the docid!" << enderr;
		OmnScreen << "Request: " << hh << endl;
		OmnScreen << "Response: " << resp << endl;
		return false;
	}
	aos_assert_r(child->xpathQuery("status/code", exist, "") == "200", false);
	doc->setDocid(docid);
	// doc->setSiteid(mSiteid);
	docstr = "";
	doc->composeData(docstr);
	mTorturer->docCreated(doc->getSiteid(), docid, doc->getObjid(), docstr);

	aos_assert_r(doc->saveWords(), false);
	return true;
}


bool
AosCreateDocThrd::signal(const int threadLogicId)
{
	return true;	
}


bool    
AosCreateDocThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

