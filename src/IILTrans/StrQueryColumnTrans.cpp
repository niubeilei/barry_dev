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
// 05/05/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrQueryColumnTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Util/DataTable.h"
#include "Util/StrStrArray.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrQueryColumn::AosIILTransStrQueryColumn(const bool flag)
:
AosIILTrans(AosTransType::eStrQueryColumn, flag AosMemoryCheckerArgs),
mFieldLen1(-1),
mFieldLen2(-1),
mOpr1(eAosOpr_Invalid),
mOpr2(eAosOpr_Invalid),
mStartIdx(-1),
mPageSize(-1),
mOrderBy(0)
{
}


AosTransPtr 
AosIILTransStrQueryColumn::clone()
{
	return OmnNew AosIILTransStrQueryColumn(false);
}


bool
AosIILTransStrQueryColumn::serializeFrom(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransStrQueryColumn::serializeTo(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransStrQueryColumn::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	bool rslt = false;
	OmnNotImplementedYet;
	return true;
}


int
AosIILTransStrQueryColumn::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransStrQueryColumn::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if(isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}

