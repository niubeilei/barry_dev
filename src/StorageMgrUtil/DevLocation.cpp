////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 07/22/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgrUtil/DevLocation.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/XmlTag.h"


AosDevLocation1::AosDevLocation1(const AosXmlTagPtr &config)
{
	// config format:
	// <partition devname="/dev/sda1" userdir="/home/ketty/AOS/Data"
	//      mountdir="/" totalsize="..."  />
	// 		<policy ... />
	// </partition>
	// if not config the userdir, use mountpoint.
	if (!config)
	{
		OmnThrowException("Missing configuration!");
		return;
	}

	//Jozhi 2015-04-20 no one use it 
	//mDevName = config->getAttrStr("devname", "dev/sda1");
	//if (mDevName == "")
	//{
	//	OmnThrowException("DevName is empty");
	//	return;
	//}

	// Chen Ding, 02/19/2012
	// Make sure 'mUserDir' is configured as an absolute path.
	//mUserDir = config->getAttrStr("userdir", "");
	OmnString user_dir = config->getAttrStr("userdir", "");
	mUserDir = OmnApp::createSysDir(user_dir);
	if(mUserDir != "" )
	{
		u32 len = mUserDir.length();
		if(mUserDir.data()[len-1] != '/')	mUserDir << "/";
	}
	/*
	if (mUserDir == "" || mUserDir.data()[0] != '/')
	{
		OmnAlarm << "UserDir is empty or not an absolute path: " << mUserDir << enderr;
		OmnThrowException("UserDir is empty or not an absolute path");
		return;
	}
	*/
}


AosDevLocation1::~AosDevLocation1()
{
}


