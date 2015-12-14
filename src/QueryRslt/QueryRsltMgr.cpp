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
// Modification History:
// 03/05/2011 Created by Shawn li
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/QueryRsltMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "QueryRslt/QueryRslt.h"
#include "Util/RCObjImp.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosQueryRsltMgrSingleton,
                 AosQueryRsltMgr,
                 AosQueryRsltMgrSelf,
                "QueryRsltMgr");

// mMaxRsltNum(eMaxRsltNum),
AosQueryRsltMgr::AosQueryRsltMgr()
{
}


AosQueryRsltMgr::~AosQueryRsltMgr()
{
}


bool		
AosQueryRsltMgr::config(const AosXmlTagPtr &def)
{
	// Chen Ding, 11/28/2012
	AosQueryRslt::initQueryRslt();

    return true;
}

