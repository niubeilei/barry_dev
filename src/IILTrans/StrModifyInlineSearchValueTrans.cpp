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
#include "IILTrans/StrModifyInlineSearchValueTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrModifyInlineSearchValue::AosIILTransStrModifyInlineSearchValue(const bool flag)
:
AosIILTrans(AosTransType::eStrModifyInlineSearchValue, flag AosMemoryCheckerArgs),
mOldSeqid(0),
mNewSeqid(0)
{
}


AosIILTransStrModifyInlineSearchValue::AosIILTransStrModifyInlineSearchValue(
		const OmnString &iilname,
		const OmnString &old_value,
		const OmnString &new_value,
		const u64 &old_seqid,
		const u64 &new_seqid,
		const OmnString &splitType,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrModifyInlineSearchValue,
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mOldValue(old_value),
mNewValue(new_value),
mOldSeqid(old_seqid),
mNewSeqid(new_seqid),
mSplitType(splitType)
{
}
	

AosTransPtr 
AosIILTransStrModifyInlineSearchValue::clone()
{
	return OmnNew AosIILTransStrModifyInlineSearchValue(false);
}


bool
AosIILTransStrModifyInlineSearchValue::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mOldValue = buff->getOmnStr("");
	mNewValue = buff->getOmnStr("");
	mOldSeqid = buff->getU64(0);
	mNewSeqid = buff->getU64(0);
	mSplitType = buff->getOmnStr("");
	
	return true;
}


bool
AosIILTransStrModifyInlineSearchValue::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mOldValue);
	buff->setOmnStr(mNewValue);
	buff->setU64(mOldSeqid);
	buff->setU64(mNewSeqid);
	buff->setOmnStr(mSplitType);
	return true;
}


bool
AosIILTransStrModifyInlineSearchValue::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	//AosIILStr *striil = (AosIILStr*)iil;
	set<OmnString> old_set;
	set<OmnString> new_set;
	set<OmnString>::iterator itr;
	bool rslt = AosInlineSearchSplit(mOldValue, mSplitType, old_set);
	aos_assert_r(rslt, false);
	rslt = AosInlineSearchSplit(mNewValue, mSplitType, new_set);
	aos_assert_r(rslt, false);

	OmnString value;
	itr = old_set.begin();
	
	int64_t idx = -10;
	int64_t iilidx = -10;
	u64 seqid = 0;
	bool isunique;
	
	while(itr != old_set.end())
	{
		value = *itr;
		
		idx = -10;
		iilidx = -10;
		seqid = 0;
		while(1)
		{
			AosIILIdx the_idx;
			rslt = iilobj->nextDocidSafe(the_idx, idx, iilidx, false, eAosOpr_eq, value, seqid, isunique, rdata); 
			if(!rslt || idx == -5 || iilidx == -5)
			{
				break;
			}
			if((seqid & 0xffffffff) == mOldSeqid)
			{
				iilobj->removeDocSafe(value, seqid, rdata);
				break;
			}
		}
		itr++;
	}
	
	itr = new_set.begin();
	while(itr != new_set.end())
	{
		value = *itr;
		iilobj->addDocSafe(value, mNewSeqid, false, false, rdata);
		itr++;
	}
	return true;
}


AosIILType 
AosIILTransStrModifyInlineSearchValue::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrModifyInlineSearchValue::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

