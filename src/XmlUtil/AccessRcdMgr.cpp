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
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/AccessRcdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/AccessRcd.h"

OmnSingletonImpl(AosAccessRcdMgrSingleton,
				 AosAccessRcdMgr,
				 AosAccessRcdMgrSelf,
				 "AosAccessRcdMgr");



AosAccessRcdMgr::AosAccessRcdMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosAccessRcdMgr::~AosAccessRcdMgr()
{
}


bool
AosAccessRcdMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosAccessRcdMgr::start()
{
	return true;
}

bool
AosAccessRcdMgr::stop()
{
	return true;
}


AosAccessRcdPtr
AosAccessRcdMgr::convertToAccessRecord(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &acd_doc)
{
	try
	{
		if (!acd_doc)
		{
			AosSetError(rdata, AosErrmsgId::eMissingUserDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		if (acd_doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_ACCESS_RCD)
		{
			AosSetError(rdata, "accessrcdmgr_not_access_rcd") << acd_doc << enderr;
			return 0;
		}

		return OmnNew AosAccessRcd(acd_doc, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, "accessrcdmgr_failed_create") << acd_doc << enderr;
		return 0;
	}
}


/*
AosAccessRcdPtr 
AosAccessRcdMgr::getAccessRecord(
	const AosXmlTagPtr &doc, 
	const bool is_parent,
	const AosRundataPtr &rdata);
{
	aos_assert_rr(doc, rdata, 0);
	try
	{
		return getAccessRecord(doc, is_parent, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating Access Record" << doc->toString() << enderr;
		return 0;
	}
}
*/
