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
#include "SEUtil/DataNullPolicy.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/StrValueInfo.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



bool
AosDataNullPolicy::handleNullData(
		char *record, 
		const int record_len,
		const AosStrValueInfo &handler,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record, false);
	switch (handler.data_null_policy)
	{
	case eTreatAsError:
		 AosSetErrorU(rdata, "data_is_null") << enderr;
		 return false;

	case eReset:
		 memset(record, handler.null_padding, record_len);
		 if (handler.set_trailing_null) record[record_len-1] = 0;
		 return true;

	case eIgnoreSilently:
		 return true;

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_null_type") << enderr;
	return false;
}

