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
#include "TaskMgr/TaskDataDoc.h"


AosTaskDataDoc::AosTaskDataDoc(const bool flag)
:
AosTaskData(AosTaskDataType::eDoc, AOSTASKDATA_DOC, flag),
mPhysicalId(-1)
{
}


AosTaskDataDoc::AosTaskDataDoc(
		const OmnString &scanner_id,
		const int physical_id)
:
AosTaskData(AosTaskDataType::eDoc, AOSTASKDATA_DOC, false),
mPhysicalId(physical_id),
mDocScannerId(scanner_id)
{
}


AosTaskDataDoc::~AosTaskDataDoc()
{
}


bool
AosTaskDataDoc::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	return true;
}


AosTaskDataObjPtr
AosTaskDataDoc::create() const
{
	return OmnNew AosTaskDataDoc(false);
}


AosTaskDataObjPtr
AosTaskDataDoc::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataDoc * data = OmnNew AosTaskDataDoc(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}


AosDataCacherObjPtr 
AosTaskDataDoc::createDataCacher(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	//Linda, 2013/08/09
	//AosDataScannerObjPtr scanner = AosDataScannerObj::createDocScannerStatic(
	//		mDocScannerId, rdata);
	//AosDataBlobObjPtr blob = AosDataBlobObj::createDataBlobStatic("", rdata);
	//return AosDataCacherObj::createDataCacher(scanner, blob, rdata);	
	OmnNotImplementedYet;
	return 0;
}

