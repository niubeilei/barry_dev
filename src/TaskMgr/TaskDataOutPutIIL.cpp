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
#include "TaskMgr/TaskDataOutPutIIL.h"
#include "Debug/Debug.h"


AosTaskDataOutPutIIL::AosTaskDataOutPutIIL(const bool flag)
:
AosTaskData(AosTaskDataType::eOutPutIIL, AOSTASKDATA_OUTPUT_IIL, flag),
mPhysicalId(-1),
mFileId(0),
mLevel(0)
{
}


AosTaskDataOutPutIIL::AosTaskDataOutPutIIL(
		const int physical_id,
		const OmnString &datacol_id,
		const u64 &file_id,
		const int level,
		const int block_idx,
		const bool last_merge)
:
AosTaskData(AosTaskDataType::eOutPutIIL, AOSTASKDATA_OUTPUT_IIL, false),
mPhysicalId(physical_id),
mDataColId(datacol_id),
mFileId(file_id),
mLevel(level),
mBlockIdx(block_idx),
mLastMerge(last_merge)
{
}


AosTaskDataOutPutIIL::~AosTaskDataOutPutIIL()
{
}


bool
AosTaskDataOutPutIIL::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskDataOutPutIIL::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_DATACOLLECTOR_ID << "=\"" << mDataColId << "\" "
		<< AOSTAG_STORAGE_FILEID << "=\"" << mFileId << "\" "
		<< AOSTAG_LEVEL << "=\"" << mLevel << "\" "
		<< AOSTAG_BLOCK_IDX << "=\"" << mBlockIdx << "\" "
		<< AOSTAG_LAST_MERGE << "=\"" << mLastMerge << "\"/>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataOutPutIIL::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mDataColId = xml->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	mFileId = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	mLevel = xml->getAttrInt(AOSTAG_LEVEL, 0);
	mBlockIdx = xml->getAttrInt(AOSTAG_BLOCK_IDX, 0);
	mLastMerge = xml->getAttrBool(AOSTAG_LAST_MERGE, false);

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataOutPutIIL::create() const
{
	OmnTagFuncInfo << endl;
	return OmnNew AosTaskDataOutPutIIL(false);
}

AosTaskDataObjPtr
AosTaskDataOutPutIIL::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}

bool
AosTaskDataOutPutIIL::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataOutPutIIL* data = (AosTaskDataOutPutIIL*)(task_data.getPtr());
	return (mPhysicalId == data->mPhysicalId &&
			mDataColId == data->mDataColId &&
			mFileId == data->mFileId);
}


OmnString 
AosTaskDataOutPutIIL::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mPhysicalId << "__"
		<< mDataColId << "__"
		<< mFileId;
	return str;
}
