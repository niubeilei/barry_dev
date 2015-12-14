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
// 2013/04/26 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "TaskMgr/TaskDataOutPutSnapShot.h"


AosTaskDataOutPutSnapShot::AosTaskDataOutPutSnapShot(const bool flag)
:
AosTaskData(AosTaskDataType::eOutPutSnapShot, AOSTASKDATA_OUTPUT_SNAPSHOT, flag),
mVirtualId(0),
mSnapShotId(0)
{
}


AosTaskDataOutPutSnapShot::AosTaskDataOutPutSnapShot(
		const u32 virtual_id,
		const u32 snapshot_id)
:
AosTaskData(AosTaskDataType::eOutPutSnapShot, AOSTASKDATA_OUTPUT_SNAPSHOT, false),
mVirtualId(virtual_id),
mSnapShotId(snapshot_id)
{
}


AosTaskDataOutPutSnapShot::~AosTaskDataOutPutSnapShot()
{
}


bool
AosTaskDataOutPutSnapShot::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskDataOutPutSnapShot::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_VIRTUAL_ID << "=\"" << mVirtualId << "\" "
		<< AOSTAG_SNAPSHOTID << "=\"" << mSnapShotId << "\"/>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataOutPutSnapShot::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mVirtualId = xml->getAttrU32(AOSTAG_VIRTUAL_ID, 0);
	mSnapShotId = xml->getAttrU32(AOSTAG_SNAPSHOTID, 0);

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataOutPutSnapShot::create() const
{
	return OmnNew AosTaskDataOutPutSnapShot(false);
}


AosTaskDataObjPtr
AosTaskDataOutPutSnapShot::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskDataOutPutSnapShot::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataOutPutSnapShot* data = (AosTaskDataOutPutSnapShot*)(task_data.getPtr());
	return ( mVirtualId == data->mVirtualId &&
			mSnapShotId == data->mSnapShotId);
}

OmnString 
AosTaskDataOutPutSnapShot::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mVirtualId;
	return str;
}

