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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WorkMgr/WorkMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/CondVar.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosWorkMgrSingleton,
                 AosWorkMgr,
                 AosWorkMgrSelf,
                "AosWorkMgr");

AosWorkMgr::AosWorkMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosWorkMgr::~AosWorkMgr()
{
}


bool
AosWorkMgr::start()
{
	return true;
}


bool
AosWorkMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosWorkMgr::stop()
{
    return true;
}


bool 
AosWorkMgr::addContainerMemberTask(
		const OmnString &ctnr_objid,
		const OmnString &aname, 
		const bool value_unique,
		const bool docid_unique, 
		const OmnString &notify_sdoc,
		const AosRundataPtr &rdata)
{
	OmnString docstr = "<task type=\"";
	OmnString vunique = (value_unique)?"true":"false";
	OmnString dunique = (docid_unique)?"true":"false";
	docstr << AOSWORKID_ADD_CONTAINER_MEMBER
		<< "\" " << AOSTAG_CONTAINER_OBJID << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_ANAME << "=\"" << aname
		<< "\" " << AOSTAG_VALUE_UNIQUE << "=\"" << vunique
		<< "\" " << AOSTAG_DOCID_UNIQUE << "=\"" << dunique
		<< "\" " << AOSTAG_NOTIFY_SDOC << "=\"" << nodify_sdoc
		<< "\"/>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr);

	addRequest(doc, rdata);
}


