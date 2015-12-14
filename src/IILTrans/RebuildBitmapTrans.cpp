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
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/RebuildBitmapTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILMgrObj.h" 
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosIILTransRebuildBitmap::AosIILTransRebuildBitmap(const bool flag)
:
AosIILTrans(AosTransType::eRebuildBitmap, flag AosMemoryCheckerArgs)
{
}


AosIILTransRebuildBitmap::AosIILTransRebuildBitmap(
		const OmnString &iilname,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eRebuildBitmap, iilname,
	false, snap_id, need_save, true AosMemoryCheckerArgs)
{
}


AosIILTransRebuildBitmap::~AosIILTransRebuildBitmap()
{
}

	
AosTransPtr 
AosIILTransRebuildBitmap::clone()
{
	return OmnNew AosIILTransRebuildBitmap(false);
}


bool
AosIILTransRebuildBitmap::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	//buff->setOmnStr(mIILName);


	return true;
}


bool
AosIILTransRebuildBitmap::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	//mIILName = buff->getOmnStr("");
	
	return true;
}


bool
AosIILTransRebuildBitmap::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);

	bool rslt = AosIILMgrObj::getIILMgr()->rebuildBitmap(
		iilobj, rdata);

	aos_assert_rr(rslt, rdata, false);
	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	return true;
}


int
AosIILTransRebuildBitmap::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransRebuildBitmap::getIILType() const
{
	return eAosIILType_Invalid;
}

