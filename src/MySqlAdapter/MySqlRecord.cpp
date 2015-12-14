////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MySqlAdapter/MySqlRecord.h"
#include "Rundata/Rundata.h"


AosMySqlRecord::AosMySqlRecord()
{
}


AosMySqlRecord::~AosMySqlRecord()
{
}

	
bool 
AosMySqlRecord::encodeHeader(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	// This function creates the Column Definition packets. 
	u8 size = mFields.size();
	aos_assert_r(size == mFields.size(), false);
	buff->setInt(sizeof(size));
	buff->setU8(size);
	for (u32 i=0; i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->encodeHeader(rdata, buff);
		if (!rslt)
		{
			AosSetError(rdata, "mysqlrecord_failed_encode_header") << enderr;
			return false;
		}
	}
	return true;
}


bool 
AosMySqlRecord::encodeRows(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	AosBuffPtr rows = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		if (!mFields[i]->encodeField(rdata, rows))
		{
			AosSetError(rdata, "mysqlrecord_failed_encode_rows") << enderr;
			return false;
		}
	}
	u32 row_len = rows->dataLen();
	buff->setInt(row_len);
	buff->setBuff(rows->data(), row_len);
	return true;
}


bool
AosMySqlRecord::appendField(
		const AosRundataPtr &rdata,
		const AosMySqlFieldPtr &field)
{
	if (field) mFields.push_back(field);
	return true;
}


bool
AosMySqlRecord::writeDelimiter(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	buff->setInt(5);
	buff->setBuff("\376\000\000\000\000", 5);
	return true;
}
