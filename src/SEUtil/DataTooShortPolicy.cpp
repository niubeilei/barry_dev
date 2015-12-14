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
// 07/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/DataTooShortPolicy.h"

#include "SEUtil/StrValueInfo.h"
#include "Rundata/Rundata.h"


bool
AosDataTooShortPolicy::handleDataTooShort(
		char *&record, 
		int &record_len,
		const char *data,
		const int data_len,
		const AosStrValueInfo &handler,
		const AosRundataPtr &rdata)
{
	aos_assert_r(data_len < record_len, false);

	switch (handler.data_too_short_policy)
	{
	case eTreatAsError:
		 AosSetErrorU(rdata, "data_too_short") << enderr;
		 return false;

	case eResetRecord:
		 memset(record, handler.too_short_padding, record_len);
		 return true;

	case eIgnoreSilently:
		 record = 0;
		 return true;

	case eAlignLeft:
		 memset(record, handler.too_short_padding, record_len-data_len);
		 memcpy(record+record_len-data_len, data, data_len);
		 return true;

	case eAlignRight:
		 memcpy(record, data, data_len);
		 memset(record+data_len, handler.too_short_padding, record_len-data_len);
		 return true;

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_policy") << ": " << handler.data_too_long_policy << enderr;
	return false;
}

