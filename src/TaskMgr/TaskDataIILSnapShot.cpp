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
#include "TaskMgr/TaskDataIILSnapShot.h"


AosTaskDataIILSnapShot::AosTaskDataIILSnapShot(const bool flag)
:
AosTaskData(AosTaskDataType::eIILSnapShot, AOSTASKDATA_IIL_SNAPSHOT, flag),
mVirtualId(0),
mSnapShotId(0),
mTaskDocid(0),
mIsCommit(false)
{
}

AosTaskDataIILSnapShot::AosTaskDataIILSnapShot(
		const u32 virtual_id,
		const u64 snapshot_id,
		const set<OmnString> &iilnames,
		const u64 &task_docid,
		const bool iscommit)
:
AosTaskData(AosTaskDataType::eIILSnapShot, AOSTASKDATA_IIL_SNAPSHOT, false),
mVirtualId(virtual_id),
mSnapShotId(snapshot_id),
mTaskDocid(task_docid),
mIsCommit(iscommit),
mIILNames(iilnames)
{
}

AosTaskDataIILSnapShot::~AosTaskDataIILSnapShot()
{
}


bool
AosTaskDataIILSnapShot::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskDataIILSnapShot::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_TASK_DOCID << "=\"" << mTaskDocid << "\" "
		<< AOSTAG_VIRTUAL_ID << "=\"" << mVirtualId << "\" "
		<< AOSTAG_IS_COMMIT << "=\"" << (mIsCommit?("true"):("false")) << "\" "
		<< AOSTAG_SNAPSHOTID << "=\"" << mSnapShotId << "\">";
	set<OmnString>::iterator itr = mIILNames.begin();
	while(itr != mIILNames.end())
	{
		docstr << "<iil " << AOSTAG_IILNAME << "=\"" << *itr << "\"/>";
		itr++;
	}
	docstr << "</" << AOSTAG_TASKDATA_TAGNAME << ">";
	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataIILSnapShot::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mVirtualId = xml->getAttrU32(AOSTAG_VIRTUAL_ID, 0);
	mSnapShotId = xml->getAttrU32(AOSTAG_SNAPSHOTID, 0);
	mTaskDocid = xml->getAttrU64(AOSTAG_TASK_DOCID, 0);
	mIsCommit = xml->getAttrBool(AOSTAG_IS_COMMIT, false);

	AosXmlTagPtr iil = xml->getFirstChild(true);
	while(iil)
	{
		mIILNames.insert(iil->getAttrStr(AOSTAG_IILNAME));
		iil = xml->getNextChild();
	}

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}

AosTaskDataObjPtr
AosTaskDataIILSnapShot::create() const
{
	return OmnNew AosTaskDataIILSnapShot(false);
}


AosTaskDataObjPtr
AosTaskDataIILSnapShot::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskDataIILSnapShot::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataIILSnapShot* data = (AosTaskDataIILSnapShot*)(task_data.getPtr());
	return ( mVirtualId == data->mVirtualId &&
			mSnapShotId == data->mSnapShotId &&
			mTaskDocid == data->mTaskDocid);
}

OmnString 
AosTaskDataIILSnapShot::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mVirtualId << "__";
	return str;
}

bool	
AosTaskDataIILSnapShot::serializeTo(
					AosBuffPtr &buff,
					const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	bool rslt = AosTaskData::serializeTo(buff, rdata);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	buff->setU32(mVirtualId);
	buff->setU8(mIsCommit);
	buff->setU64(mSnapShotId);

	buff->setU32(mIILNames.size());
	set<OmnString>::iterator itr = mIILNames.begin();
	while(itr != mIILNames.end())
	{
		buff->setOmnStr(*itr);
		itr++;
	}
	return true;
}

bool		
AosTaskDataIILSnapShot::serializeFrom(
					const AosBuffPtr &buff,
					const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	bool rslt = AosTaskData::serializeFrom(buff, rdata);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0);
	mVirtualId = buff->getU32(0);
	mIsCommit = (bool)(buff->getU8(0));
	mSnapShotId = buff->getU64(0);

	u32 size = buff->getU32(0);
	for (u32 i=0; i < size; i++)
	{
		mIILNames.insert(buff->getOmnStr(""));
	}

	return true;
}


