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
#include "IILTrans/StrRemoveInlineSearchValueTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrRemoveInlineSearchValue::AosIILTransStrRemoveInlineSearchValue(const bool flag)
:
AosIILTrans(AosTransType::eStrRemoveInlineSearchValue, flag AosMemoryCheckerArgs),
mSeqid(0)
{
}


AosIILTransStrRemoveInlineSearchValue::AosIILTransStrRemoveInlineSearchValue(
		const OmnString &iilname,
		const OmnString &value,
		const u64 &seqid,
		const OmnString &splitType,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrRemoveInlineSearchValue, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mSeqid(seqid),
mSplitType(splitType)	
{
}


AosTransPtr 
AosIILTransStrRemoveInlineSearchValue::clone()
{
	return OmnNew AosIILTransStrRemoveInlineSearchValue(false);
}


bool
AosIILTransStrRemoveInlineSearchValue::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");;
	mSeqid = buff->getU64(0);
	mSplitType = buff->getOmnStr("");
	return true;
}


bool
AosIILTransStrRemoveInlineSearchValue::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setU64(mSeqid);
	buff->setOmnStr(mSplitType);
	return true;
}


bool
AosIILTransStrRemoveInlineSearchValue::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	set<OmnString> m;
	set<OmnString>::iterator itr;
	bool rslt = AosInlineSearchSplit(mValue, mSplitType, m);
	aos_assert_r(rslt, false);

	OmnString value;
	itr = m.begin();
	
	int64_t idx = -10;
	int64_t iilidx = -10;
	u64 seqid = 0;
	bool isunique;
	
	while(itr != m.end())
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
			if((seqid & 0xffffffff) == mSeqid)
			{
				iilobj->removeDocSafe(value, seqid, rdata);
				break;
			}
		}
		itr++;
	}
	return true;
}


AosIILType 
AosIILTransStrRemoveInlineSearchValue::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrRemoveInlineSearchValue::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

