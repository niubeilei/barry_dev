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
#include "IILTrans/HitAddDocByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


// Ketty 2013/03/18
// The TransClient not use this trans ??
#if 0
AosIILTransHitAddDocById::AosIILTransHitAddDocById(const bool flag)
:
AosIILTrans(AosIILFuncType::eHitAddDocById, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransHitAddDocById::AosIILTransHitAddDocById(
		const u64 &transid,
		const u64 &iilid,
		const bool isPersis,
		const u64 &docid,
		const AosRundataPtr &rdata)
:
AosIILTrans(AosIILFuncType::eHitAddDocById, transid,
		iilid, true, isPersis, false AosMemoryCheckerArgs),
mDocid(docid)
{
}


// Ketty 2012/10/24	
/*
bool
AosIILTransHitAddDocById::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	mTransId = trans->getTransId();
	bool rslt = AosIILTrans::initTransBaseById(trans_doc, rdata);
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


AosIILTransPtr 
AosIILTransHitAddDocById::clone()
{
	return OmnNew AosIILTransHitAddDocById(false);
}


bool 
AosIILTransHitAddDocById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);	
	return true;
}


bool 
AosIILTransHitAddDocById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	aos_assert_r(mDocid, false);	
	
	return true;
}

	
bool
AosIILTransHitAddDocById::proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);

	aos_assert_rr(iilobj->getIILType() == eAosIILType_Hit, rdata, false);

	return iilobj->addDocSafe(mDocid, rdata);
}


u64
AosIILTransHitAddDocById::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDById(rdata);
}


int
AosIILTransHitAddDocById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize() + AosBuff::getU64Size();
}


AosIILType
AosIILTransHitAddDocById::getIILType() const 
{
	return eAosIILType_Hit;
}

#endif
