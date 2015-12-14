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
// 2013/02/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapTransObj.h"

//#include "BitmapEngine/Ptrs.h"
#include "SEInterfaces/BitmapEngineObj.h"


AosBitmapTransObj::AosBitmapTransObj(
			const AosTransType::E type,
			const bool reg_flag)
:
AosTaskTrans(type,reg_flag)
{
}

AosBitmapTransObj::AosBitmapTransObj(
		const AosTransType::E type,
		const int svr_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(type,svr_id,need_save,need_resp)
{
}

