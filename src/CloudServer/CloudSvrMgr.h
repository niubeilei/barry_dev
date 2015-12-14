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
#ifndef Aos_CloudServer_CloudSvrMgr_h
#define Aos_CloudServer_CloudSvrMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosCloudSvrMgr : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxVirtuals = 65535
	};

private:
	int			mNumVirtuals;

public:
	AosCloudSvrMgr();
	~AosCloudSvrMgr();

	bool config(const AosXmlTagPtr &conf);
	bool start();
	bool stop();

	virtual int getNumCloudServers() const;
};
#endif

