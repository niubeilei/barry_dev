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
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataStructs/Jimos/StructProcSum.h"

AosStructProcSum::AosStructProc(const OmnString &version)
:
AosStructProc(AOSSTRUCTPROC_SUM, version)
{
}


AosStructProcSum::~AosStructProc()
{
}


bool 
AosStructProcSum::procRecord(
		const AosRundataPtr &rdata, 
		const char *data, 
		const int data_len, 
		AosDataRecordObj *record)
{
	// This function assumes:
	// 	[docid + xxx + field + xxx]
	// The field type is numerical. It adds its value to 
	// the corresponding field in the doc.
	aos_assert_rr(data, rdata, false);
	aos_assert_rr(mFieldLen > 0, rdata, false);
	aos_assert_rr(mFieldPos >= 0 && mFieldPos + mFieldLen < data_len, rdata, false);
	aos_assert_rr(record, rdata, false);

	AosValuePtr value = AosCreateValue(rdata, &data[mFieldPos], mFieldLen, mDataType);
	aos_assert_rr(value, rdata, false);
	return record->setFieldValue(mFieldName, value);
}	

#endif
