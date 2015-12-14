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
#include "IILTrans/StrSetValueDocUniqueByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrSetValueDocUniqueById::AosIILTransStrSetValueDocUniqueById(const bool flag)
:
AosIILTrans(AosTransType::eStrSetValueDocUniqueById, flag AosMemoryCheckerArgs),
mDocid(0),
mMustSame(true)
{
}


AosIILTransStrSetValueDocUniqueById::AosIILTransStrSetValueDocUniqueById(
		const u64 iilid,
		const bool isPersis,
		const OmnString &value,
		const u64 docid,
		const bool must_same,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrSetValueDocUniqueById, 
		iilid, isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mDocid(docid),
mMustSame(must_same)
{
}


AosTransPtr 
AosIILTransStrSetValueDocUniqueById::clone()
{
	return OmnNew AosIILTransStrSetValueDocUniqueById(false);
}


bool
AosIILTransStrSetValueDocUniqueById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mValue = buff->getOmnStr("");
	mDocid = buff->getU64(0);
	char a = buff->getChar('t');
	mMustSame = (a == 't');
	return true;
}


bool
AosIILTransStrSetValueDocUniqueById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setU64(mDocid);
	char a = mMustSame ? 't' : 'f';
	buff->setChar(a);
	return true;
}


bool
AosIILTransStrSetValueDocUniqueById::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	return iilobj->setValueDocUniqueSafe(mValue, mDocid, mMustSame, rdata);
}


AosIILType 
AosIILTransStrSetValueDocUniqueById::getIILType() const
{
	return eAosIILType_Str;
}


int
AosIILTransStrSetValueDocUniqueById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mValue.length())
		+ AosBuff::getU64Size()
		+ AosBuff::getCharSize();
}

