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
// 06/28/2012 Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Config/ConfigMgr.h"

#include "Config/ConfigHandler.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

static OmnMutex sgLock;
static int sgNumHandlers = 0;
static AosConfigHandlerPtr sgConfigHandlers[1000];
int AosConfigMgr::smBasePort = -1;
AosConfigMgr::map_t AosConfigMgr::smMap;
bool AosConfigMgr::smInited = false;

AosConfigMgr::AosConfigMgr()
{
	smInited = true;
	sgLock.lock();
	for (int i=0; i<sgNumHandlers; i++)
	{
		if (!addHandler(sgConfigHandlers[i]))
		{
			OmnAlarm << "Failed adding handler: " 
				<< sgConfigHandlers[i]->getConfigHandlerName() << enderr;
		}
	}
	sgLock.unlock();
}


bool
AosConfigMgr::registerHandler(const AosConfigHandlerPtr &handler)
{
	bool rslt = true;
	aos_assert_r(handler, false);
	sgLock.lock();
	if (smInited)
	{
		rslt = addHandler(handler);
	}
	else
	{
		sgConfigHandlers[sgNumHandlers++] = handler;
	}
	sgLock.unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosConfigMgr::addHandler(const AosConfigHandlerPtr &handler)
{
	aos_assert_r(handler, false);
	OmnString name = handler->getConfigHandlerName();
	aos_assert_r(name != "", false);
	mapitr_t itr = smMap.find(name);
	if (itr != smMap.end())
	{
		OmnAlarm << "Config handler already exist: " << name << enderr;
		return false;
	}

	smMap[name] = handler;
	cout << __FILE__ << ":" << __LINE__ << " Config Handler added: " << name << endl;
	return true;
}
