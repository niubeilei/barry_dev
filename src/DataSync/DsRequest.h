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
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataSync_DsRequest_h
#define AOS_DataSync_DsRequest_h

#include "DataSync/DsTypes.h"
#include "DataSync/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosDsRequest : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	u64					mRequestId;
	u64					mTime;
	AosDsDataType::E	mDataType;
	AosXmlTagPtr		mData;
	bool				mRespReceived[eAosDsMaxConns];
	int					mNumSyncs;

public:
	AosDsRequest(
			const int num_syncs,
			const AosDsDataType::E data_type, 
			const AosXmlTagPtr &data);
	~AosDsRequest();

	u64		getRequestId() const {return mRequestId;}
};
#endif
#endif
