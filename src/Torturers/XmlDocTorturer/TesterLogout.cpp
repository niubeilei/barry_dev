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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Torturers/XmlDocTorturer/TesterLogout.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SEModules/SeRundata.h" 
#include "SmartDoc/SmartDoc.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include <stdlib.h>

AosLogoutTester::AosLogoutTester(const bool regflag)
:
AosSengTester(AosSengTester_Logout, AosTesterId::eLogout, regflag),
mUseValidSsidWeight(40),
mDoNotUseSsidWeight(30),
mUseSsidWeight(50)
{
}


AosLogoutTester::AosLogoutTester()
:
AosSengTester(AosSengTester_Logout, "logout", AosTesterId::eLogout),
mUseValidSsidWeight(40),
mDoNotUseSsidWeight(30),
mUseSsidWeight(50)
{
}


AosLogoutTester::~AosLogoutTester()
{
}


bool 
AosLogoutTester::test()
{
	/*
	// To logout, one must have already been logged in.
	AosRundataPtr rdata = mThread->getRundata();
	//rdata->setLogging(true);
	int tries = rand() % eLogoutTryWeight + 1;
	while (tries-- > 0)
	{
		//rdata->resetForReuse(0);
		//rdata->setLogging(true);
		OmnString ssid = mThread->getCrtSsid();
		OmnString ssid_to_use;
		bool valid_ssid = false;
		int vv = rand() % 100;
		if (ssid != "")
		{
			if (vv < mUseValidSsidWeight)
			{
				// Use valid SSID
				valid_ssid = true;
				ssid_to_use = ssid;
			}
			else if (vv < mDoNotUseSsidWeight)
			{
				// Do not use SSID
				ssid_to_use = "";
				valid_ssid = false;
			}
			else
			{
				// Randomly pick an SSID
				ssid_to_use = OmnRandom::letterStr(eMinSsidLen, eMaxSsidLen);
				if (ssid == ssid_to_use)
				{
					// The randomly picked SSID is a good one
					valid_ssid = true;
				}
			}
		}
		else
		{
			// Did not get an SSID
			if (vv < mUseSsidWeight)
			{
				ssid_to_use = OmnRandom::letterStr(eMinSsidLen, eMaxSsidLen);
			}
		}
	
		bool rslt = AosSengAdmin::getSelf()->logout(
						mThread->getSiteid(), 
						ssid_to_use, 
						mThread->getUrlDocid(),
						rdata
						);
		if (valid_ssid)
		{
			aos_assert_r(rslt, false);
			mThread->resetSession();
			return true;
		}
		else
		{
			aos_assert_r(!rslt, false);
		}
	}

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

