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
#include "UserMgmt/UserDomainMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "UserMgmt/UserDomain.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(AosUserDomainMgrSingleton,
				 AosUserDomainMgr,
				 AosUserDomainMgrSelf,
				 "AosUserDomainMgr");



AosUserDomainMgr::AosUserDomainMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosUserDomainMgr::~AosUserDomainMgr()
{
}


bool
AosUserDomainMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosUserDomainMgr::start()
{
	return true;
}

bool
AosUserDomainMgr::stop()
{
	return true;
}


AosUserDomainObjPtr
AosUserDomainMgr::getUserDomain1(
		const OmnString &ctnr_objid, 
		const AosUserAcctObjPtr &user_acct, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	aos_assert_rr(user_acct, rdata, 0);
	try
	{
		return OmnNew AosUserDomain(ctnr_objid, user_acct, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateUserDomain);
		OmnAlarm << rdata->getErrmsg() << ". Container: " << ctnr_objid << enderr;
		return 0;
	}


	/*
	mLock->lock();
	map<OmnString, AosUserDomainObjPtr>::iterator itr = mUserDomains.find(ctnr_objid);
	if (itr == mUserDomains.end())
	{
		// Not found. Create it.
		try
		{
			AosUserDomainObjPtr domain = OmnNew AosUserDomain(ctnr_objid, user_acct, rdata);
			mUserDomains[ctnr_objid] = domain;
			mLock->unlock();
			if (domain->isGood())
			{
				return domain;
			}
			return 0;
		}

		catch (...)
		{
			OmnAlarm << "Failed creating user domain!" << enderr;
			return 0;
		}
	}

	AosUserDomainObjPtr domain = itr->second;
	mLock->unlock();
	if (domain->isGood()) return domain;
	return 0;
	*/
}


bool 
AosUserDomainMgr::isFriendUserDomain(
		const AosUserDomainObjPtr &domain1, 
		const AosUserDomainObjPtr &domain2,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(domain1 && domain2, rdata, false);
	return domain1->isFriendDomain(domain2);
}

