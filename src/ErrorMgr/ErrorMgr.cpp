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
#include "ErrorMgr/ErrorMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "ErrorMgr/Ptrs.h"
#include "ErrorMgr/ErrMsgMgr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosErrorMgrSingleton,
				 AosErrorMgr,
				 AosErrorMgrSelf,
				 "AosErrorMgr");


static OmnString sgErrmgrDir = "Errmsgs";
static OmnString sgMissingErrmsgMgr = "Missing Error Message Manager";

AosErrorMgr::AosErrorMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosErrorMgr::~AosErrorMgr()
{
}


bool
AosErrorMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	OmnString base_dirname = OmnApp::getAppBaseDir();
	mDirname = base_dirname;

	AosXmlTagPtr errmgr_config = config->getFirstChild("errmgr");
	if (!errmgr_config)
	{
		mDirname << "/" << sgErrmgrDir;
		return true;
	}

	OmnString dirname = errmgr_config->getAttrStr("dirname", sgErrmgrDir);
	mDirname << "/" << dirname;

	return true;
}


bool
AosErrorMgr::start()
{
	return true;
}

bool
AosErrorMgr::stop()
{
	return true;
}


OmnString 
AosErrorMgr::getErrmsg(
		const AosLocale::E locale, 
		const AosErrmsgId::E error_id)
{
	AosErrMsgMgrPtr msgmgr = getErrorMsgMgr(locale);
	if (!msgmgr)
	{
		msgmgr = getErrorMsgMgr(AosLocale::getDefaultLocale());
		if (!msgmgr)
		{
			OmnString errmsg = sgMissingErrmsgMgr;
			errmsg << " for locale: " << AosLocale::toStr(locale);
			return errmsg;
		}
	}
	return msgmgr->getErrmsg(error_id);
}


AosErrMsgMgrPtr
AosErrorMgr::getErrorMsgMgr(const AosLocale::E locale)
{
	aos_assert_r(AosLocale::isValid(locale), 0);
	mLock->lock();
	if (!mErrMsgMgr[locale])
	{
		mErrMsgMgr[locale] = OmnNew AosErrMsgMgr(locale);
	}

	AosErrMsgMgrPtr msgmgr = mErrMsgMgr[locale];
	mLock->unlock();
	return msgmgr;
}

