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
// 10/09/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/IILBatchGetDocidsAsyncTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILBatchGetDocidsAsyncTrans::AosIILBatchGetDocidsAsyncTrans(const bool flag)
:
AosIILTrans(AosTransType::eIILBatchGetDocids, flag AosMemoryCheckerArgs)
{
}


AosIILBatchGetDocidsAsyncTrans::AosIILBatchGetDocidsAsyncTrans(
		const AosBuffPtr &columns,
		const OmnString &iilname,
		const AosAsyncRespCallerPtr &resp_caller,
		const int field_idx,
		const bool isPersis,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eIILBatchGetDocids, 
		iilname, isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mColumns(columns),
mRespCaller(resp_caller),
mFieldIdx(field_idx)
{
}

AosTransPtr 
AosIILBatchGetDocidsAsyncTrans::clone()
{
	return OmnNew AosIILBatchGetDocidsAsyncTrans(false);
}


bool
AosIILBatchGetDocidsAsyncTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	int len = buff->getInt(0);
	mColumns = buff->getBuff(len, true AosMemoryCheckerArgs);
	mFieldIdx= buff->getInt(0);

	return true;
}


bool
AosIILBatchGetDocidsAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	int len = mColumns->dataLen();
	buff->setInt(len);
	buff->setBuff(mColumns);

	buff->setInt(mFieldIdx);
	return true;
}


bool
AosIILBatchGetDocidsAsyncTrans::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	vector<u64> docids;
	mColumns->reset();


	resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setInt(mFieldIdx);

	bool rslt = true;
	while(mColumns->hasMore())
	{
		u64 docid = 0;
		bool isunique = false;
		int64_t idx = -10;
		int64_t iilidx = -10;
		AosIILIdx the_idx;
		OmnString key = mColumns->getOmnStr("");
		//rslt = striil->nextDocidSafe(the_idx, idx, iilidx,
		rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx,
			false, eAosOpr_eq, key, docid, isunique, rdata); 
		OmnString value;
		value << docid;
		resp_buff->setOmnStr(value);
	}

	return rslt;
}


int
AosIILBatchGetDocidsAsyncTrans::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILBatchGetDocidsAsyncTrans::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if(isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}

bool
AosIILBatchGetDocidsAsyncTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();
	
	AosTransPtr thisptr(this, false);
//	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}

