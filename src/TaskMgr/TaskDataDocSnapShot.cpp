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
#include "TaskMgr/TaskDataDocSnapShot.h"


AosTaskDataDocSnapShot::AosTaskDataDocSnapShot(const bool flag)
:
AosTaskData(AosTaskDataType::eDocSnapShot, AOSTASKDATA_DOC_SNAPSHOT, flag),
mVirtualId(0),
mSnapShotId(0),
mDocType(AosDocType::eNormalDoc),
mTaskDocid(0),
mIsCommit(false)
{
}


AosTaskDataDocSnapShot::AosTaskDataDocSnapShot(
		const u32 virtual_id,
		const u32 snapshot_id,
		const AosDocType::E doctype,
		const u64 &task_docid,
		const bool iscommit)
:
AosTaskData(AosTaskDataType::eDocSnapShot, AOSTASKDATA_DOC_SNAPSHOT, false),
mVirtualId(virtual_id),
mSnapShotId(snapshot_id),
mDocType(doctype),
mTaskDocid(task_docid),
mIsCommit(iscommit)
{
}


AosTaskDataDocSnapShot::~AosTaskDataDocSnapShot()
{
}


bool
AosTaskDataDocSnapShot::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskDataDocSnapShot::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_TASK_DOCID << "=\"" << mTaskDocid << "\" "
		<< AOSTAG_DOCTYPE << "=\"" << mDocType << "\" "
		<< AOSTAG_VIRTUAL_ID << "=\"" << mVirtualId << "\" "
		<< AOSTAG_IS_COMMIT << "=\"" << (mIsCommit?("true"):("false")) << "\" "
		<< AOSTAG_SNAPSHOTID << "=\"" << mSnapShotId << "\"/>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataDocSnapShot::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mVirtualId = xml->getAttrU32(AOSTAG_VIRTUAL_ID, 0);
	mSnapShotId = xml->getAttrU32(AOSTAG_SNAPSHOTID, 0);
	mDocType = (AosDocType::E)(xml->getAttrU32(AOSTAG_DOCTYPE, 0));
	mTaskDocid = xml->getAttrU64(AOSTAG_TASK_DOCID, 0);
	mIsCommit = xml->getAttrBool(AOSTAG_IS_COMMIT, false);

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}

AosTaskDataObjPtr
AosTaskDataDocSnapShot::create() const
{
	return OmnNew AosTaskDataDocSnapShot(false);
}


AosTaskDataObjPtr
AosTaskDataDocSnapShot::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskDataDocSnapShot::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataDocSnapShot* data = (AosTaskDataDocSnapShot*)(task_data.getPtr());
	return ( mVirtualId == data->mVirtualId &&
			mSnapShotId == data->mSnapShotId &&
			mDocType == data->mDocType &&
			mTaskDocid == data->mTaskDocid);
}

OmnString 
AosTaskDataDocSnapShot::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mVirtualId << "__"
		<< mDocType << "__"
		<< mTaskDocid;
	return str;
}

bool
AosTaskDataDocSnapShot::serializeTo(
					AosBuffPtr &buff,
					const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	bool rslt = AosTaskData::serializeTo(buff, rdata);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	buff->setU32(mDocType);
	buff->setU32(mVirtualId);
	buff->setU8(mIsCommit);
	buff->setU64(mSnapShotId);

	return true;
}

bool
AosTaskDataDocSnapShot::serializeFrom(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	bool rslt = AosTaskData::serializeFrom(buff, rdata);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	mDocType = (AosDocType::E)(buff->getU32(0));
	mVirtualId = buff->getU32(0);
	mIsCommit = (bool)buff->getU8(0);
	mSnapShotId = buff->getU64(0);

	return true;
}


