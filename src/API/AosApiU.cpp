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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/11/2015	Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiU.h"

bool AosU64BatchAdd(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->U64BatchAdd(iilname, entry_len, buff,
			executor_id, snap_id, task_docid, rdata);
}

