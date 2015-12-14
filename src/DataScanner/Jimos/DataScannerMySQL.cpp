////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	
// Modification History:
// 2013/11/21: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataFile/DataFileMySQL.h"

#include "Alarm/Alarm.h"


AosDataFileMySQL::AosDataFileMySQL()
{
}


AosDataFileMySQL::AosDataFileMySQL(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	if (!def)
	{
		OmnThrowException("missing_data_file_def");
		return;
	}

	mFileDoc = def->clone();
}


AosDataFileMySQL::AosDataFileMySQL(
		const AosRundataPtr &rdata, 
		const OmnString &objid)
{
	if (!init(rdata, objid))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}

}


bool
AosDataFileMySQL::init(
		const AosRundataPtr &rdata,
		const OmnString &objid)
{
	mFileDoc = AosGetDocByObjid(rdata, objid);
	aos_assert_rr(mFileDoc, rdata, false);
	return true;
}


AosDataFileMySQL::~AosDataFileMySQL()
{
}


int64_t
AosDataFileMySQL::getTotalSize()
{
	aos_assert_rr(mFileDoc, rdata, -1);
	return mFileDoc->getAttrInt64("file_size", -1);
}

#endif
