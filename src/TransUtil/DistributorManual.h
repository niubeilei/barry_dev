////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransUtil_DistributorManual_h
#define AOS_TransUtil_DistributorManual_h

#include "TransUtil/TransDistributor.h"
#include "SEInterfaces/ServerInfo.h"
#include "XmlUtil/Ptrs.h"

class AosDistributorManual : public AosTransDistributor	
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxIds = 1000 * 100		// 100,000
	};

	//AosServerInfo::Type	mType;
	u64					mMaxId;
	int				*   mMap;

public:
	AosDistributorManual(
	//		const AosServerInfo::Type type,
			const AosXmlTagPtr &conf);
	~AosDistributorManual();

	// TransDistributor Interface
	virtual int routeReq(const u64 &dist_id);

private:
	bool init(const AosXmlTagPtr &tag);
};
#endif

