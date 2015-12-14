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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CloudServer_CloudServer_h
#define Aos_CloudServer_CloudServer_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosCloudServer : public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCloudServer(const u64 &cubic_id);
	~AosCloudServer();

	bool config(const AosXmlTagPtr &conf);
	bool start();
	bool stop();
};
#endif

