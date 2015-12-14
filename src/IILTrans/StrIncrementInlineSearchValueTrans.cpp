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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrIncrementInlineSearchValueTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrIncrementInlineSearchValue::AosIILTransStrIncrementInlineSearchValue(const bool flag)
:
AosIILTrans(AosTransType::eStrIncrementInlineSearchValue, flag AosMemoryCheckerArgs),
mSeqid(0),
mIncValue(0)
{
}


AosIILTransStrIncrementInlineSearchValue::AosIILTransStrIncrementInlineSearchValue(
		const OmnString &iilname,
		const OmnString &value,
		const u64 seqid,
		const u64 inc_value,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrIncrementInlineSearchValue, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mSeqid(seqid),
mIncValue(inc_value)
{
}	


AosTransPtr 
AosIILTransStrIncrementInlineSearchValue::clone()
{
	return OmnNew AosIILTransStrIncrementInlineSearchValue(false);
}


bool
AosIILTransStrIncrementInlineSearchValue::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mValue = buff->getOmnStr("");
	mSeqid = buff->getU64(0);
	mIncValue = buff->getU64(0);
	return true;
}


bool
AosIILTransStrIncrementInlineSearchValue::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setU64(mSeqid);
	buff->setU64(mIncValue);
	return true;
}


bool
AosIILTransStrIncrementInlineSearchValue::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	int64_t idx = -10;
	int64_t iilidx = -10;
	u64 seqid = 0;
	bool isunique;
	bool rslt = false;
	u64 count = 0;
	//AosIILStr *striil = (AosIILStr*)iil;
	while(1)
	{
		AosIILIdx the_idx;
		rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx, false, eAosOpr_eq, mValue, seqid, isunique, rdata); 
		if(!rslt || idx == -5 || iilidx == -5)
		{
			break;
		}
		if((seqid & 0xffffffff) == mSeqid)
		{
			count = seqid >> 32;
			iilobj->removeDocSafe(mValue, seqid, rdata);
			break;
		}
	}
	
	count += mIncValue;
	u64 did = (count << 32) + mSeqid;
	return iilobj->addDocSafe(mValue, did, false, false, rdata);
}


AosIILType 
AosIILTransStrIncrementInlineSearchValue::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrIncrementInlineSearchValue::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getU64Size();
}

