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
#include "TaskMgr/TaskDataVirtFile.h"


AosTaskDataVirtFile::AosTaskDataVirtFile(const bool flag)
:
AosTaskData(AosTaskDataType::eVirtualFile, AOSTASKDATA_VIRTUALFILE, flag)
{
}


AosTaskDataVirtFile::AosTaskDataVirtFile(
		const OmnString &virfile_objid,
		const int physical_id)
:
AosTaskData(AosTaskDataType::eVirtualFile, AOSTASKDATA_VIRTUALFILE, false),
mPhysicalId(physical_id),
mVirFileObjid(virfile_objid)
{
}


AosTaskDataVirtFile::~AosTaskDataVirtFile()
{
}


bool
AosTaskDataVirtFile::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	return true;
}


AosTaskDataObjPtr
AosTaskDataVirtFile::create() const
{
	return OmnNew AosTaskDataVirtFile(false);
}


AosTaskDataObjPtr
AosTaskDataVirtFile::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	AosTaskDataVirtFile * data = OmnNew AosTaskDataVirtFile(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}


bool
AosTaskDataVirtFile::isDataExist(const AosRundataPtr &rdata)
{
	return AosVirtualFile::fileExist(mVirFileObjid, rdata);
}

#endif
