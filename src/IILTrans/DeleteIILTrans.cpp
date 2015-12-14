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
// 2014/05/15 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/DeleteIILTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransDeleteIIL::AosIILTransDeleteIIL(const bool flag)
:
AosIILTrans(AosTransType::eDeleteIIL, flag AosMemoryCheckerArgs),
mTrueDelete(false)
{
}


AosIILTransDeleteIIL::AosIILTransDeleteIIL(
		const u64 &iilid,
		const bool true_delete,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eDeleteIIL, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mTrueDelete(true_delete)
{
}


AosIILTransDeleteIIL::AosIILTransDeleteIIL(
		const OmnString &iilname,
		const bool true_delete,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eDeleteIIL, iilname, 
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mTrueDelete(true_delete)
{
}


AosTransPtr
AosIILTransDeleteIIL::clone()
{
	return OmnNew AosIILTransDeleteIIL(false);
}


bool
AosIILTransDeleteIIL::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU8(mTrueDelete);
	return true;
}


bool
AosIILTransDeleteIIL::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTrueDelete = buff->getU8(0);
	return true;
}


bool
AosIILTransDeleteIIL::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = iilobj->deleteIILSafe(mTrueDelete, rdata);

	OmnScreen << "delete iil finished, iilname:" << mIILName
		<< ", true_delete:" << mTrueDelete
		<< ", success:" << rslt << endl;

	return rslt;
}


AosIILType 
AosIILTransDeleteIIL::getIILType() const
{
	return eAosIILType_Invalid;
}


int
AosIILTransDeleteIIL::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize();
}

