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
#include "IILTrans/StrAddInlineSearchValueTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrAddInlineSearchValue::AosIILTransStrAddInlineSearchValue(const bool flag)
:
AosIILTrans(AosTransType::eStrAddInlineSearchValue, flag AosMemoryCheckerArgs),
mSeqid(0)
{
}


AosIILTransStrAddInlineSearchValue::AosIILTransStrAddInlineSearchValue(
		const OmnString &iilname,
		const OmnString value,
		const u64 seqid,
		const OmnString splitType,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrAddInlineSearchValue, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mSeqid(seqid),
mSplitType(splitType)
{
}


AosTransPtr 
AosIILTransStrAddInlineSearchValue::clone()
{
	return OmnNew AosIILTransStrAddInlineSearchValue(false);
}


bool
AosIILTransStrAddInlineSearchValue::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mValue = buff->getOmnStr("");
	mSplitType = buff->getOmnStr("");
	mSeqid = buff->getU64(0);
	return true;
}


bool
AosIILTransStrAddInlineSearchValue::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setOmnStr(mSplitType);
	buff->setU64(mSeqid);
	return true;
}


bool
AosIILTransStrAddInlineSearchValue::proc(
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
	while(itr != m.end())
	{
		value = *itr;
		iilobj->addDocSafe(value, mSeqid, false, false, rdata);
		itr++;
	}
	return true;
}


AosIILType 
AosIILTransStrAddInlineSearchValue::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrAddInlineSearchValue::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getStrSize(mSplitType.length())
		+ AosBuff::getU64Size();
}

