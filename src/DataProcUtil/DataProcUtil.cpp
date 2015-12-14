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
// 2015/01/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcUtil.h"


AosDataRecordObjPtr 
AosDataProcUtil::createIILOutputRecord(AosRundata *rdata)
{
	// This function creates the following data record:
	// 	Field1: key, string
	// 	Field2: docid, u64
	AosDataRecordObjPtr record = AosDataRecordObj::createBuffRecord(rdata);
	aos_assert_rr(record, rdata, 0);
	record->appendField("key", AosDataType::eString);
	record->appendField("docid", AosDataType::eU64);
	return record;
}

