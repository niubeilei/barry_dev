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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngClient_StorageEngClient_h
#define Aos_StorageEngClient_StorageEngClient_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosStorageEngClient : public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosStorageEngClient(const u64 &cubic_id);
	~AosStorageEngClient();

	bool config(const AosXmlTagPtr &conf);
	bool start();
	bool stop();

	virtual u64 getSizeId(const int size, const AosRundataPtr &rdata);
	virtual bool createSizeId(
						const int size, 
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	virtual bool removeSizeId(
						const u64 size, 
						const AosRundataPtr &rdata);

private:
};
#endif

