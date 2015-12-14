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
#include "TaskMgr/TaskDataOutPutDocid.h"
#include "Debug/Debug.h"


AosTaskDataOutPutDocid::AosTaskDataOutPutDocid(const bool flag)
:
AosTaskData(AosTaskDataType::eOutPutDocid, AOSTASKDATA_OUTPUT_DOCID, flag),
mPhysicalId(-1),
mStartDocid(0),
mNum(0)
{
}


AosTaskDataOutPutDocid::AosTaskDataOutPutDocid(
		const int physical_id,
		const OmnString &key,
		const u64 &start_docid,
		const int64_t &num)
:
AosTaskData(AosTaskDataType::eOutPutDocid, AOSTASKDATA_OUTPUT_DOCID, false),
mPhysicalId(physical_id),
mKey(key),
mStartDocid(start_docid),
mNum(num)
{
}


AosTaskDataOutPutDocid::~AosTaskDataOutPutDocid()
{
}


bool
AosTaskDataOutPutDocid::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTaskDataOutPutDocid::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\" " 
		<< AOSTAG_KEY << "=\"" << mKey << "\" "
		<< AOSCONFIG_STARTDOCID << "=\"" << mStartDocid << "\" "
		<< AOSTAG_NUM_DOCS << "=\"" << mNum << "\"/>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataOutPutDocid::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mKey = xml->getAttrStr(AOSTAG_KEY);
	mStartDocid = xml->getAttrU64(AOSCONFIG_STARTDOCID, 0);
	mNum = xml->getAttrInt64(AOSTAG_NUM_DOCS, -1);

	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataOutPutDocid::create() const
{
	OmnTagFuncInfo << endl;
	return OmnNew AosTaskDataOutPutDocid(false);
}


AosTaskDataObjPtr
AosTaskDataOutPutDocid::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}

bool
AosTaskDataOutPutDocid::isTheSameTaskData(
		const AosTaskDataObjPtr &task_data,
		const AosRundataPtr &rdata)
{
	if (mType != task_data->getTaskDataType())
	{
		return false;
	}

	AosTaskDataOutPutDocid* data = (AosTaskDataOutPutDocid*)(task_data.getPtr());
	return (mPhysicalId == data->mPhysicalId &&
			mKey == data->mKey &&
			mStartDocid == data->mStartDocid &&
			mNum == data->mNum);
}


OmnString 
AosTaskDataOutPutDocid::getStrKey(const AosRundataPtr &rdata)
{
	OmnString str;
	str << AosTaskDataType::toStr(mType) << "__"
		<< mPhysicalId << "__"
		<< mKey << "__"
		<< mStartDocid << "__"
		<< mNum;
	return str;
}
