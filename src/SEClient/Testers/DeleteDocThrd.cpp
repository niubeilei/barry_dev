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
#include "SEClient/Testers/DeleteDocThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "SEClient/SEClient.h"
#include "SEClient/Testers/SearchEngTester.h"
#include "SEClient/Testers/CreateDocThrd.h"
#include "SEClient/Testers/DeleteDocThrd.h"
#include "SEClient/Testers/ModifyDocThrd.h"
#include "SEClient/Testers/QueryThrd.h"
#include "SEClient/Testers/DocThrd.h"
#include "SEClient/Testers/SearchEngTester.h"
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


AosDeleteDocThrd::AosDeleteDocThrd(
		const AosSearchEngTesterPtr &torturer,
		const int total, 
		const int weight, 
		const OmnIpAddr &raddr, 
		const int rport, 
		const OmnString &attrtname)
:
mTorturer(torturer),
mTotal(total),
mWeight(weight),
mNumDocs(0),
mAttrTname(attrtname)
{
	mConn = OmnNew AosSEClient(raddr, rport);
	if (mWeight <= 0) mWeight = eDftWeight;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "DeleteDocThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosDeleteDocThrd::~AosDeleteDocThrd()
{
}


bool
AosDeleteDocThrd::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	return true;
}


bool	
AosDeleteDocThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	mIsCreating = false;
	int nn;
    while (state == OmnThrdStatus::eActive)
    {
		// int wtime = rand() % mWeight;
		// OmnWaitSelf->wait(wtime);
		nn = mNumDocs;
		if (mNumDocs < mTotal) 
		{
			if (!mTorturer->isUpdateStopped()) 
			{
				mIsCreating = true;
				deleteDoc();
				mIsCreating = false;
			}
		}

		if (nn == mNumDocs)
		{
			// No more to delete
			OmnSleep(1000);
		}
	}
	return true;
}


bool
AosDeleteDocThrd::deleteDoc()
{
	// This function deletes a doc from the system. The doc
	// to be deleted is randomly selected. 

	// 1. Generate the doc
	OmnString siteid;
	OmnString docid;
	OmnString objid;
	mTorturer->getDocidToDelete(siteid, docid, objid);
	if (docid == "") return true;
	mNumDocs++;
	OmnScreen << "To delete: " << mNumDocs << endl;

	// Need to construct a request, which is in the form:
	//	<request>
    //		<item name='operation'>deleteObj</item>
    //		<item name='username'>xxx</item>
    //		<item name='appname'>xxx</item>
    //		<item name='siteid'>xxx</item>
    //		<item name='container'>xxx</item>
    //		<item name='docid'>xxx</item>
	//	</request>

	OmnString appname = "testapp";
	appname << mNumDocs;

	OmnString user = "chending";
	user << mNumDocs;

	OmnString ctnr = "ctnr";
	ctnr << mNumDocs;

	// Add the header to the generated xml
	OmnString hh = "<request><item name='operation'>delObject</item><item name='";
	hh << AOSTAG_USERNAME << "'>"
		<< user << "</item><item name='" << AOSTAG_APPNAME << "'>"
		<< appname << "</item><item name='" << AOSTAG_SITEID << "'>"
		<< siteid << "</item><item name='" << AOSTAG_HPCONTAINER << "'>"
		<< ctnr << "</item><item name='" << AOSTAG_DOCID << "'>"
		<< docid << "</item><item name='" << AOSTAG_OBJID << "'>" 
		<< objid << "</item></request>";

	OmnString errmsg;
	OmnString resp;
	OmnScreen << "To send request: " << hh << endl;
	mConn->procRequest(siteid, appname, user, hh, resp, errmsg);
	aos_assert_r(resp != "", false);
	OmnScreen << "Got response: " << resp<< endl;

	// Check the response. Response should be in the form:
	// 	<response error="false" 
	// 		code="200"
	// 		docid="xxx"/>
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "");
	AosXmlTagPtr child = root->getFirstChild();
	aos_assert_r(child, false);
	child = child->getFirstChild("status");
	aos_assert_r(child, false);
	aos_assert_r(child->getAttrStr("code") == "200", false);
	mTorturer->docDeleted(siteid, docid);

	// Delete all the attributes from database
	OmnString stmt = "delete from ";
	stmt << mAttrTname << " where siteid='" << siteid 
		<< "' and docid='" << docid << "'";
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnRslt rslt;
	aos_assert_r(store->runSQL(stmt), false);
	return true;
}


bool
AosDeleteDocThrd::signal(const int threadLogicId)
{
	return true;	
}


bool    
AosDeleteDocThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

