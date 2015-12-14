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
// A device map maintains a list of devices. This class is responsible for 
// managing the statuses of all the devices in the device map.
//
// Modification History:
// 12/15/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "MgmtServer/DeviceMap.h"

#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"



AosDeviceMap::AosDeviceMap()
{
}


AosDeviceMap::~AosDeviceMap()
{
}


bool
AosDeviceMap::config(const AosXmlTagPtr &conf)
{
	// The confiruation should be:
	// 	<config ...>
	// 		<devices>
	// 			<device .../>
	// 			<device .../>
	// 			...
	// 		</devices>
	// 		...
	// 	</config>
	aos_assert_r(conf, false);
	AosXmltagPtr devices = conf->getFirstChild("devices");
	if (!devices)
	{
		OmnAlarm << "Missing devices: " << conf->toString() << enderr;
		exit(0);
	}

	AosXmlTagPtr dev = devices->getFirstChild();
	mMaxDeviceId = -1;
	while (dev)
	{
		try
		{
			AosDevicePtr device = OmnNew AosDevice(dev);
			int device_id = device->getDeviceId();
			if (device_id <0 || device_id >= eMaxDevices)
			{
				AosSetError(rdata, AOSLT_INVALID_DEVICE_ID);
				OmnAlarm << rdata->getErrmsg() << ". " << device_id << enderr;
				exit(0);
			}
			if (mDevices[device_id])
			{
				AosSetError(rdata, AOSLT_INVALID_DEVICE_ID);
				OmnAlarm << rdata->getErrmsg() 
					<< "Two devices with the same ID: " << device_id << enderr;
				exit(0);
			}

			mDevices[device_id] = device;
			if (device_id > mMaxDeviceId) mMaxDeviceId = device_id;
		}
		dev= devices->getNextChild();
	}
	return true;
}


