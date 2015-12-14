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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskMgr/TaskDataNorm.h"
#include "Debug/Debug.h"

AosTaskDataNorm::AosTaskDataNorm(const bool flag)
:
AosTaskData(AosTaskDataType::eNorm, AOSTASKDATA_NORM, flag)
{
}


AosTaskDataNorm::~AosTaskDataNorm()
{
}


bool
AosTaskDataNorm::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	bool rslt = AosTaskData::config(def, rdata);
	aos_assert_r(rslt, false);

	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId >= 0, false);

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosTaskDataNorm::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\"/>";

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataNorm::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	
	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataNorm::create() const
{
	return OmnNew AosTaskDataNorm(false);
}


AosTaskDataObjPtr
AosTaskDataNorm::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataNorm * data = OmnNew AosTaskDataNorm(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	OmnTagFuncInfo << endl;
	return data;
}

#endif
