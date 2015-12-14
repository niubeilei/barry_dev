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
// 2013/04/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapTransObj_h
#define Aos_SEInterfaces_BitmapTransObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/TransType.h"
#include "TransUtil/TaskTrans.h"

class AosBitmapTransObj : public AosTaskTrans
{
public:
	AosBitmapTransObj(
			const AosTransType::E type,
			const bool reg_flag);
	AosBitmapTransObj(
			const AosTransType::E type,
			const int svr_id,
			const bool need_save,
			const bool need_resp);

	virtual bool proc() = 0;
	virtual bool procMsg(const AosRundataPtr &rdata) = 0;
};
#endif

