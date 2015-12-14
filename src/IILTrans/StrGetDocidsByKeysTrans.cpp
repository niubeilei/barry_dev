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
#include "IILTrans/StrGetDocidsByKeysTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILTrans/IILTransInclude.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrGetDocidsByKeys::AosIILTransStrGetDocidsByKeys(const bool flag)
:
AosIILTrans(AosTransType::eStrGetDocidsByKeys, flag AosMemoryCheckerArgs),
mKeyNum(0),
mKeys(0),
mNeedDftValue(false),
mDftValue(0)
{
}


AosIILTransStrGetDocidsByKeys::AosIILTransStrGetDocidsByKeys(
		const u64 &iilid,
		const u32 key_num,
		const AosBuffPtr &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrGetDocidsByKeys, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKeyNum(key_num),
mKeys(keys),
mNeedDftValue(need_dft_value),
mDftValue(dft_value)
{
}


AosIILTransStrGetDocidsByKeys::AosIILTransStrGetDocidsByKeys(
		const OmnString &iilname,
		const u32 key_num,
		const AosBuffPtr &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrGetDocidsByKeys, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKeyNum(key_num),
mKeys(keys),
mNeedDftValue(need_dft_value),
mDftValue(dft_value)
{
}


AosTransPtr 
AosIILTransStrGetDocidsByKeys::clone()
{
	return OmnNew AosIILTransStrGetDocidsByKeys(false);
}


bool
AosIILTransStrGetDocidsByKeys::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mKeyNum);
	buff->setU8(mNeedDftValue);
	buff->setU64(mDftValue);

	buff->setU32(mKeys->dataLen());
	buff->setBuff(mKeys);
	return true;
}


bool
AosIILTransStrGetDocidsByKeys::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mKeyNum = buff->getInt(-1);
	mNeedDftValue = buff->getU8(0);
	mDftValue = buff->getU64(0);

	u32 key_len = buff->getU32(0);
	mKeys = buff->getBuff(key_len, true AosMemoryCheckerArgs);

	aos_assert_r(mKeyNum > 0 && mKeys, false);
	return true;
}


bool
AosIILTransStrGetDocidsByKeys::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	aos_assert_r(iilobj->getIILType() == eAosIILType_Str, false);

	AosBuffPtr rsp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	int64_t idx, iilidx;
	OmnString key;
	u64 docid = 0;
	bool isunique = false;
	bool rslt;
	for(int i=0; i<mKeyNum; i++)
	{
		key = mKeys->getOmnStr("");
		docid = mDftValue;
		isunique = false;
		idx = -10;
		iilidx = -10;
		if(key != "")
		{
			AosIILIdx the_idx;
			u64 tmp_docid = 0;
			rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx, false, eAosOpr_eq, key, tmp_docid, isunique, rdata); 
			if (rslt && tmp_docid != 0 && isunique)
			{
				docid = tmp_docid;
			}
			else if (mNeedDftValue)
			{
				docid = mDftValue;
			}
			else
			{
				return false;
			}
		}
		rsp_buff->setU64(docid);
	}

	u32 docid_len = rsp_buff->dataLen();
	resp_buff = OmnNew AosBuff(docid_len + 10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU32(docid_len);
	resp_buff->setBuff(rsp_buff);

	return rslt;
}


AosIILType 
AosIILTransStrGetDocidsByKeys::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrGetDocidsByKeys::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

