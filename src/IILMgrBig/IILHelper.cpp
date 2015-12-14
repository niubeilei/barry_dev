////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 09/27/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IILHelper.h"

#include "IdGen/IdGenMgr.h"
#include "IILMgrBig/IILMgr.h"
#include "IILClient/IIClient.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/SeError.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"

OmnSingletonImpl(AosIILHelperSingleton,
                 AosIILHelper,
                 AosIILHelperSelf,
                "AosIILHelper");



AosIILHelper::AosIILHelper()
{
}


AosIILHelper::~AosIILHelper()
{
}


bool      	
AosIILHelper::start()
{
	return true;
}


bool        
AosIILHelper::stop()
{
	return true;
}


bool
AosIILHelper::config(const AosXmlTagPtr &theconfig) 
{
	// OmnThreadedObjPtr thisPtr(this, false);
	// mThread = OmnNew OmnThread(thisPtr, "Thrd", 0, true, true, __FILE__, __LINE__);
	// mThread->start();
	return true;
}

 
bool    
AosIILHelper::signal(const int threadLogicId)
{
	return true;
}


bool    
AosIILHelper::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosIILHelper::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(100);
	}

	return true;
}


/*
bool	
AosIILHelper::addWhoVisited(
		const OmnString &siteid,
		const AosXmlTagPtr &doc, 
		const OmnString &cid, 
		const AosRundataPtr &rdata)
{
	// The system keeps a special IIL that records who visited which 
	// doc and when. The IIL is identified by:
	// 	AOSZTG_WHOVISITED + siteid + ":" + visited_doc_docid 
	// Its string value is cid and the docid is used to store the seconds.
	aos_assert_r(cid != "", false);
	OmnString docid = doc->getAttrStr(AOSTAG_DOCID);
	aos_assert_r(docid != "", false);

	OmnString iilname = AOSZTG_WHOVISITED;
	iilname << siteid << ":" << doc->getAttrStr(AOSTAG_DOCID);
	u64 secs;
	bool isunique;
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(cid!="", false);
	AosIILClient::getSelf()->getDocid(iilname, cid, secs, isunique, rdata);
	if (secs != 0)
	{
		// It exists in the list. Modify it.
		AosIILClient::getSelf()->removeValueDoc(iilname, cid, secs, rdata);
	}
	secs = OmnGetSecond();
	AosIILClient::getSelf()->addValueDoc(iilname, cid, secs, false, true, rdata);
	return true;
}
*/

