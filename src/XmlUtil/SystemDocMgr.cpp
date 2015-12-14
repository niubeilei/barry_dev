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
// 09/05/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/SystemDocMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SystemDoc.h"

OmnSingletonImpl(AosSystemDocMgrSingleton,
				 AosSystemDocMgr,
				 AosSystemDocMgrSelf,
				 "AosSystemDocMgr");



AosSystemDocMgr::AosSystemDocMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosSystemDocMgr::~AosSystemDocMgr()
{
}


bool
AosSystemDocMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSystemDocMgr::start()
{
	return true;
}

bool
AosSystemDocMgr::stop()
{
	return true;
}


AosSystemDocPtr
AosSystemDocMgr::getSystemDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	try
	{
		if (!doc)
		{
			AosSetError(rdata, AosErrmsgId::eMissingUserDoc);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		if (doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_SYSTEM_DOC)
		{
			AosSetError(rdata, AosErrmsgId::eNotSystemDoc)
				<< doc->getAttrStr(AOSTAG_DOCID);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		return OmnNew AosSystemDoc(doc, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateSysdoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
}

