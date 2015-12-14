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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 06/24/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/GsmMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "ShortMsgUtil/GsmModem.h"
#include "ShortMsgUtil/Ptrs.h"


AosGsmMgr::AosGsmMgr(const AosXmlTagPtr &config)
{
	aos_assert(config);
	aos_assert(init(config));
}


AosGsmMgr::~AosGsmMgr()
{
}


bool
AosGsmMgr::init(const AosXmlTagPtr &config)
{
	int port = 0;
	AosXmlTagPtr gsm_tag = config->getFirstChild();
	while(gsm_tag)
	{
		if (port == 0)
		{
			mDftGsmModem = OmnNew AosGsmModem(gsm_tag);
			if (!mDftGsmModem->openModem(port))
			{
				OmnAlarm << "Failed to init default GSM Modem!" << enderr;
				return false;
			}
		}
		else
		{
			OmnString pKey = gsm_tag->getAttrStr("key");
			if (pKey == "") 
			{
				OmnAlarm << "key is empty!" << enderr;
				return false;
			}
			AosGsmModemPtr gsm = OmnNew AosGsmModem(gsm_tag);
			if (gsm->openModem(port))
			{
				mGsmModemMap.insert(AosGsmModemPair(pKey, gsm));
			}
		}
		port ++;
		gsm_tag = config->getNextChild();
	}
	return true;
}


void 
AosGsmMgr::close()
{
	if (mDftGsmModem) mDftGsmModem->closeModem();
	AosGsmModemMapItr itr = mGsmModemMap.begin();
	for (; itr != mGsmModemMap.end(); itr++)
	{
		itr->second->closeModem();
	}
}

AosGsmModemPtr
AosGsmMgr::getGsmModem(const OmnString &key)
{
	// This function map the key-gsm, and return 
	// the gsm. key may be multi-part, format like this:
	// "1,2,3"
	aos_assert_r(key != "", 0);
	AosGsmModemMapItr itr;
	for (itr=mGsmModemMap.begin(); itr!=mGsmModemMap.end(); itr++)
	{
		if (itr->first.indexOf(key, 0) != -1)
		{
			if (itr->second)
			{
				return itr->second;
			}
			else
			{
				return mDftGsmModem;
			}
		}
	}
	return mDftGsmModem;
}
