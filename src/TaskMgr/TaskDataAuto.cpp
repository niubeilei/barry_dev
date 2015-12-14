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
#include "TaskMgr/TaskDataAuto.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/ReadFile.h"


AosTaskDataAuto::AosTaskDataAuto(const bool flag)
:
AosTaskData(AosTaskDataType::eAuto, AOSTASKDATA_AUTO, flag),
mPhysicalId(-1)
{
}


AosTaskDataAuto::AosTaskDataAuto(
		const OmnString &file_name,
		const int physical_id)
:
AosTaskData(AosTaskDataType::eAuto, AOSTASKDATA_AUTO, false),
mPhysicalId(physical_id),
mFileName(file_name)
{
}


AosTaskDataAuto::~AosTaskDataAuto()
{
}


bool
AosTaskDataAuto::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	bool rslt = AosTaskData::config(def, rdata);
	aos_assert_r(rslt, false);

	mPhysicalId = def->getAttrInt(AOSTAG_PHYSICALID, -1);
	if (mPhysicalId < 0)
	{
		AosSetErrorU(rdata, "invalid_physical_id:") << def->toString();
		return false;
	}

	mFileName = def->getNodeText();
	if (mFileName == "")
	{
		AosSetErrorU(rdata, "missing_file_name:") << def->toString();
		return false;
	}

	return true;
}


bool
AosTaskDataAuto::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_PHYSICALID << "=\"" << mPhysicalId << "\">"
		<< "<![CDATA[" << mFileName << "]]></" << AOSTAG_TASKDATA_TAGNAME << ">";	

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataAuto::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPhysicalId = xml->getAttrInt(AOSTAG_PHYSICALID, -1);
	mFileName = xml->getNodeText();
	aos_assert_r(mFileName != "", false);
	
	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataAuto::create() const
{
	return OmnNew AosTaskDataAuto(false);
}


AosTaskDataObjPtr
AosTaskDataAuto::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataAuto * data = OmnNew AosTaskDataAuto(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}


AosDataCacherObjPtr 
AosTaskDataAuto::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	//AosDataScannerObjPtr scanner = AosDataScannerObj::createFileScannerStatic(
	//		mPhysicalId, mFileName, record->getRecordLen(), rdata);
	//AosDataBlobObjPtr blob = AosDataBlobObj::createDataBlobStatic(
	//		record, rdata);
	//return AosDataCacherObj::createDataCacher(scanner, blob, rdata);	
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskDataAuto::isDataExist(const AosRundataPtr &rdata)
{
	return true;
}

#endif
