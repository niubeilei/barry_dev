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
#if 0
#include "TaskMgr/TaskDataOutPutDoc.h"
#include "Debug/Debug.h"


AosTaskDataOutPutDoc::AosTaskDataOutPutDoc(const bool flag)
:
AosTaskData(AosTaskDataType::eOutPutDoc, AOSTASKDATA_OUTPUT_DOC, flag),
mPhysicalId(-1),
mServerId(-1),
mSizeid(0),
mRecordLen(-1),
mFileId(0),
mIsFixed(true)
{
}


AosTaskDataOutPutDoc::AosTaskDataOutPutDoc(
		const int physical_id,
		const int server_id,
		const OmnString &key,
		const u64 &sizeid,
		const int record_len,
		const u64 &file_id,
		const bool is_fixed)
:
AosTaskData(AosTaskDataType::eOutPutDoc, AOSTASKDATA_OUTPUT_DOC, false),
mPhysicalId(physical_id),
mServerId(server_id),
mDataColId(key),
mSizeid(sizeid),
mRecordLen(record_len),
mFileId(file_id),
mIsFixed(is_fixed)
{
}


AosTaskDataOutPutDoc::~AosTaskDataOutPutDoc()
{
}


bool
AosTaskDataOutPutDoc::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskDataOutPutDoc::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" "
		<< AOSTAG_SERVERID << "=\"" << mServerId << "\" " 
		<< AOSTAG_DATACOLLECTOR_ID << "=\"" << mDataColId << "\" "
		<< AOSTAG_SIZEID << "=\"" << mSizeid << "\" "
		<< AOSTAG_RECORD_LEN << "=\"" << mRecordLen << "\" "
		<< AOSTAG_STORAGE_FILEID << "=\"" << mFileId << "\" "
		<< AOSTAG_IS_FIXED << "=\"" << mIsFixed << "\"/>"; 

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataOutPutDoc::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mServerId = xml->getAttrInt(AOSTAG_SERVERID, -1);
	mDataColId = xml->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	mSizeid = xml->getAttrU64(AOSTAG_SIZEID, 0);
	mRecordLen = xml->getAttrInt(AOSTAG_RECORD_LEN, -1);
	mFileId = xml->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	mIsFixed = xml->getAttrBool(AOSTAG_IS_FIXED, true);
		
	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataOutPutDoc::create() const
{
	OmnTagFuncInfo << endl;
	return OmnNew AosTaskDataOutPutDoc(false);
}


AosTaskDataObjPtr
AosTaskDataOutPutDoc::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}

bool
AosTaskDataOutPutDoc::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataOutPutDoc* data = (AosTaskDataOutPutDoc*)(task_data.getPtr());

	return (mPhysicalId == data->mPhysicalId &&
			mServerId == data->mServerId &&
			mDataColId == data->mDataColId &&
			mSizeid == data->mSizeid &&
			mRecordLen == data->mRecordLen &&
			mFileId == data->mFileId && 
			mIsFixed == data->mIsFixed);
}


OmnString
AosTaskDataOutPutDoc::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mPhysicalId << "__"
		<< mServerId << "__"
		<< mDataColId << "__"
		<< mSizeid << "__"
		<< mRecordLen << "__"
		<< mFileId << "__" 
		<< mIsFixed;
	return str;
}

#endif
