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
#include "IILTrans/HitRemoveDocByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

#if 0
// Ketty This Trans TransClient not used.
AosIILTransHitRemoveDocById::AosIILTransHitRemoveDocById(const bool flag)
:
AosIILTrans(AosIILFuncType::eHitRemoveDocById, flag AosMemoryCheckerArgs),
mDocid(0)
{
}


AosIILTransHitRemoveDocById::AosIILTransHitRemoveDocById(
		const u64 &transid,
		const u64 &iilid,
		const u64 &docid,
		const AosRundataPtr &rdata)
:
AosIILTrans(AosIILFuncType::eHitRemoveDocById, transid, 
		iilid, true, false, false AosMemoryCheckerArgs),
mDocid(docid)
{
}


// Ketty 2012/10/24	
/*
bool
AosIILTransHitRemoveDocById::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	bool rslt = AosIILTrans::initTransBaseById(trans_doc, rdata);
	if(!rslt) return rslt;

	mDocid = trans_doc->getAttrU64("docid", 0);
	if (mDocid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << trans_doc->toString() << enderr;
	}
	return true;
}
*/

AosIILTransPtr 
AosIILTransHitRemoveDocById::clone()
{
	return OmnNew AosIILTransHitRemoveDocById(false);
}


bool 
AosIILTransHitRemoveDocById::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);	
	return true;
}


bool 
AosIILTransHitRemoveDocById::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mDocid = buff->getU64(0);
	return true;
}


bool
AosIILTransHitRemoveDocById::proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iil->getIILType() == eAosIILType_Hit, rdata, false);

	return iilobj->removeDocSafe(mDocid, rdata);
}


AosIILType 
AosIILTransHitRemoveDocById::getIILType() const
{
	return eAosIILType_Hit;
}


u64
AosIILTransHitRemoveDocById::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDById(rdata);
}

	
int
AosIILTransHitRemoveDocById::getSerializeSize() const
{
	return AosIILTrans::getSerializeSize()
		+ AosBuff::getU64Size();
}
#endif 
