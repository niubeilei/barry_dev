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
// 2013/09/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransServer/TransFileReq.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"

	
AosTransFileReq::AosTransFileReq(const Type tp, const int read_id)
:
mType(tp),
mReadId(read_id),
mFinish(false),
mSem(OmnNew OmnSem(0))
{
}



OmnString
AosTransFileReq::toString()
{
	OmnString str;
	str << "type:" << (mType == eClean ? "clean" : "readUnfinish")
		<< "; read_id:" << mReadId;
	return str;
}

