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
#include "IILTrans/U64SetValueDocUniqueByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransU64SetValueDocUniqueById::AosIILTransU64SetValueDocUniqueById(const bool flag)
:
AosIILTrans(AosTransType::eU64SetValueDocUniqueById, flag AosMemoryCheckerArgs),
mValue(0),
mDocid(0),
mMustSame(true)
{
}


AosIILTransU64SetValueDocUniqueById::AosIILTransU64SetValueDocUniqueById(
		const u64 iilid,
		const bool isPersis,
		const u64 value,
		const u64 docid,
		const bool must_same,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eU64SetValueDocUniqueById, iilid,
		isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mMustSame(must_same)
{
}


AosTransPtr 
AosIILTransU64SetValueDocUniqueById::clone()
{
	return OmnNew AosIILTransU64SetValueDocUniqueById(false);
}


bool
AosIILTransU64SetValueDocUniqueById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mValue = buff->getU64(0);
	mDocid = buff->getU64(0);
	char a = buff->getChar('t');
	mMustSame = (a == 't');
	return true;
}


bool
AosIILTransU64SetValueDocUniqueById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mValue);
	buff->setU64(mDocid);
	char a = mMustSame ? 't' : 'f';
	buff->setChar(a);
	return true;
}


bool
AosIILTransU64SetValueDocUniqueById::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_U64, rdata, false);

	return iilobj->setValueDocUniqueSafe(mValue, mDocid, mMustSame, rdata);
}


AosIILType 
AosIILTransU64SetValueDocUniqueById::getIILType() const
{
	return eAosIILType_U64;
}


int
AosIILTransU64SetValueDocUniqueById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getU64Size()
		+ AosBuff::getU64Size();
		+ AosBuff::getCharSize();
}

