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
// 12/05/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MgmtServer_Device_h
#define AOS_MgmtServer_Device_h

#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/IpAddr.h"
#include <vector>
using namespace std;


class AosDevice : public OmnRCObject
{
	OmnDefineRCObject;
	
private:
	int					mDeviceId;
	vector<OmnIpAddr>	mAddrs;
	vector<int>			mPorts;

public:
	AosDevice(const int device_id);
	~AosDevice();
};

#endif

