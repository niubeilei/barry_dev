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
// 02/11/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/FrontEndSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Rundata/Rundata.h"
#include "SEModules/Site.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"

static int sgFrontEndPort = 6472;

OmnSingletonImpl(AosFrontEndSvrSingleton,
                 AosFrontEndSvr,
                 AosFrontEndSvrSelf,
                "AosFrontEndSvr");


AosFrontEndSvr::AosFrontEndSvr()
:
mServerId(-1)
{
}


AosFrontEndSvr::~AosFrontEndSvr()
{
}


bool      	
AosFrontEndSvr::start()
{
	return true;
}


bool        
AosFrontEndSvr::stop()
{
	return true;
}


bool
AosFrontEndSvr::config(const AosXmlTagPtr &def)
{
	// If it is a frontend, there should be a config tag like:
	// 	<config ...>
	// 		<frontendserver server_id="xxx">
	// 			<server server_id="xxx" ip="xxx" port="xxx"/>
	// 			<server server_id="xxx" ip="xxx" port="xxx"/>
	// 			...
	// 		</frontendserver>
	if (OmnApp::isFrontEndServer())
	{
		aos_assert_r(def, false);
		AosXmlTagPtr tag = def->getFirstChild("frontendserver");
		if (!tag)
		{
			OmnAlarm << "Configuration incorrect: no front end server information!" << enderr;
			exit(-1);
		}
	
		int server_id = tag->getAttrInt("server_id", -1);
		if (server_id < 0)
		{
			OmnAlarm << "Missing Front End Server ID: " << server_id << enderr;
			exit(-1);
		}
		mServerId = server_id;
		OmnApp::setServerId(mServerId);

		AosXmlTagPtr server = tag->getFirstChild();
		while (server)
		{
			server_id = server->getAttrInt("server_id", -1);
			if (server_id < 0)
			{
				OmnAlarm << "Missing Front End Server ID: " << server_id << enderr;
				exit(-1);
			}

			OmnString ip = server->getAttrStr("ip", "");
			if (ip == "")
			{
				OmnAlarm << "Invalid IP address!" << enderr;
				exit(-1);
			}

			int port = server->getAttrInt("port", sgFrontEndPort);
			if (port <= 0)
			{
				port = sgFrontEndPort;
			}

			ServerInfo server_info(server_id, ip, port);
			if (mServerInfo.size() <= (u32)server_id)
			{
				mServerInfo.resize(server_id+1);
			}
			mServerInfo[server_id] = server_info;

			server = tag->getNextChild();
		}

		// Make sure all front ends are specified
		for (u32 i=0; i<mServerInfo.size(); i++)
		{
			if (mServerInfo[i].ip == "")
			{
				OmnAlarm << "Missing front end server info: " << i << enderr;
				exit(-1);
			}
		}
	}

	return true;
}


bool
AosFrontEndSvr::checkSsid(const OmnString &ssid)
{
	OmnNotImplementedYet;
	return false;
}

