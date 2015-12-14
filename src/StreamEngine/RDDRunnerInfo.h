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
// 12/09/2015 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_RDDRunnerInfo_h
#define AOS_StreamEngine_RDDRunnerInfo_h

#include "TaskUtil/TaskStatus.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


class AosRDDRunnerInfo : virtual public OmnRCObject
{
	OmnDefineRCObject;

	OmnString				mServiceId;
	u64						mRDDId;
	AosTaskStatus::E		mStatus;
	int						mRunServerId;
	u64						mStartStampTime;
	OmnString				mSnapShots;
	OmnString				mErrmsg;
	
public:
	AosRDDRunnerInfo();

	AosRDDRunnerInfo(
			const OmnString &service_id,
			const int svr_id);

	AosRDDRunnerInfo(
		const OmnString &service_id,
		const u64 &rdd_id,
		const int svr_id,
		const AosTaskStatus::E status,
		const u64 &timestamp,
		const OmnString &errmsg);

	~AosRDDRunnerInfo();

	bool serializeFrom(const AosBuffPtr &buff);

	bool serializeTo(const AosBuffPtr &buff);

	OmnString toString();

	u64			getRDDId() const {return mRDDId;}
	int			getRunServerId() const {return mRunServerId;}

	AosTaskStatus::E getStatus() const {return mStatus;}

	OmnString	getServiceId() const {return mServiceId;}

	u64 		getStartSTime() const {return mStartStampTime;}

	OmnString			getErrmsg() const {return mErrmsg;}

	void setSnapShots(const AosXmlTagPtr &snapshots);
	OmnString	getSnapShots();
};

#endif

