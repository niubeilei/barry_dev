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
#include "IILTrans/HitAddDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "IILTrans/IILTransTester.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransHitAddDocByName::AosIILTransHitAddDocByName(const bool flag)
:
AosIILTrans(AosTransType::eHitAddDocByName, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransHitAddDocByName::AosIILTransHitAddDocByName(
		const OmnString &iilname,
		const bool isPersis,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eHitAddDocByName, iilname, 
		isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocid(docid)
{
}


// Ketty 2012/10/24	
/*
bool
AosIILTransHitAddDocByName::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	mTransId = trans->getTransId();
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	if(!rslt) return rslt;
	
	mDocid = trans_doc->getAttrU64("docid", 0);
	if (mDocid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << trans_doc->toString() << enderr;
		return false;
	}
	return true;
}
*/

AosTransPtr 
AosIILTransHitAddDocByName::clone()
{
	return OmnNew AosIILTransHitAddDocByName(false);
}


bool 
AosIILTransHitAddDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);	
	return true;
}


bool 
AosIILTransHitAddDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);

	aos_assert_r(mDocid, false);	
	aos_assert_r(mIILName != "", false);	
	
	return true;
}

	
bool
AosIILTransHitAddDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{ 
	aos_assert_rr(iilobj, rdata, false);

	aos_assert_rr(iilobj->getIILType() == eAosIILType_Hit, rdata, false);

	return iilobj->addDocSafe(mDocid, rdata);
}


AosIILType 
AosIILTransHitAddDocByName::getIILType() const
{
	return eAosIILType_Hit;
}


/*
u64
AosIILTransHitAddDocByName::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/
	
int
AosIILTransHitAddDocByName::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getStrSize(mIILName.length())
		+ AosBuff::getU64Size();
}

