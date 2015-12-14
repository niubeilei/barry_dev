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
#include "StreamEngine/RDDRunnerInfo.h"
#include "XmlUtil/XmlTag.h"

#include "XmlUtil/SeXmlParser.h"


AosRDDRunnerInfo::AosRDDRunnerInfo()
{
}

AosRDDRunnerInfo::AosRDDRunnerInfo(
		const OmnString &service_id,
		const int run_svr_id)
:
mServiceId(service_id),
mRDDId(0),
mStatus(AosTaskStatus::eStop),
mRunServerId(run_svr_id),
mStartStampTime(0),
mSnapShots(""),
mErrmsg("")

{
}

AosRDDRunnerInfo::AosRDDRunnerInfo(
		const OmnString &service_id,
		const u64 &rdd_id,
		const int run_svr_id,
		const AosTaskStatus::E status,
		const u64 &timestamp,
		const OmnString &errmsg)
:
mServiceId(service_id),
mRDDId(rdd_id),
mStatus(status),
mRunServerId(run_svr_id),
mStartStampTime(timestamp),
mSnapShots(""),
mErrmsg(errmsg)
{
}

AosRDDRunnerInfo::~AosRDDRunnerInfo()
{
}

bool
AosRDDRunnerInfo::serializeFrom(const AosBuffPtr &buff)
{
	mServiceId = buff->getOmnStr("");
	mRDDId = buff->getU64(0);
	mStatus = (AosTaskStatus::E)buff->getU8(0);
	mRunServerId = buff->getInt(-1);
	mStartStampTime = buff->getU64(0);
	mSnapShots = buff->getOmnStr("");
	mErrmsg = buff->getOmnStr("");

	return true;
}


bool
AosRDDRunnerInfo::serializeTo(const AosBuffPtr &buff)
{
	buff->setOmnStr(mServiceId);
	buff->setU64(mRDDId);
	buff->setU8(mStatus);
	buff->setInt(mRunServerId);
	buff->setU64(mStartStampTime);
	buff->setOmnStr(mSnapShots);
	buff->setOmnStr(mErrmsg);

	return true;
}


OmnString
AosRDDRunnerInfo::toString()
{
	OmnString str = "Update RDD Status:";
	str << mServiceId << ":" << mRDDId << ":" << AosTaskStatus::toStr(mStatus);
	return str;
}

void
AosRDDRunnerInfo::setSnapShots(const AosXmlTagPtr &snapshots) 
{

	if (snapshots)
	{
		mSnapShots = snapshots->toString(); 
OmnScreen << "========================setSnapShots: " << mSnapShots << endl;
	}
}

OmnString
AosRDDRunnerInfo::getSnapShots() 
{ 
OmnScreen << "=========================getSnapShots: " << mSnapShots << endl;
	return mSnapShots; 
}
