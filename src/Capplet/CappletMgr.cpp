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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Capplet/CappletMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"


OmnSingletonImpl(AosCappletMgrSingleton,
                 AosCappletMgr,
                 AosCappletMgrSelf,
                "AosCappletMgr");


AosCappletMgr::AosCappletMgr()
{
}


AosCappletMgr::~AosCappletMgr()
{
}


bool
AosCappletMgr::start()
{
	return true;
}


bool
AosCappletMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosCappletMgr::stop()
{
    return true;
}


bool 
AosCappletMgr::registerCapplet(
		const AosRundataPtr &rdata, 
		const AosCappletPtr &capplet)
{
	aos_assert_rr(capplet, rdata, false);
	AosCappletId::E id = capplet->getId();
	aos_assert_rr(AosCappletId::isValid(id), rdata, false);

	if (id == AosCappletId::eNamedCapplet)
	{
		OmnString name = capplet->getName();
		aos_assert_rr(name != "", rdata, false);

		mLock->lock();
		mapitr_t itr = mCappletMap.find(name);
		if (itr != mCappletMap.end())
		{
			mLock->unlock();
			AosSetErrorU(rdata, "capplet_already_registered") << ": " << name <<enderr;
			return false;
		}
		mCappletMap[name] = capplet;
		mLock->unlock();
		return true;
	}

	mLock->lock();
	aos_assert_rl(!mCapplets[id], mLock, false);
	mCapplets[id] = capplet;
	mLock->unlock();
	return true;
}


bool 
AosCappletMgr::runCapplet(
		const AosRundataPtr &rdata, 
		AosParms &parms, 
		const AosCappletId::E capplet_id, 
		const AosDataletPtr &datalet)
{
	AosCappletPtr capplet = getCapplet(rdata, capplet_id);
	aos_assert_rr(capplet, rdata, false);
	return capplet->run(rdata, parms, datalet);
}


bool 
AosCappletMgr::runCapplet(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &parms,
		const AosCappletId::E capplet_id, 
		const AosDataletPtr &data)
{
	AosParms &pp = AosParmsXml(parms);
	return runCapplet(rdata, pp, capplet_id, datalet);
}
#endif
