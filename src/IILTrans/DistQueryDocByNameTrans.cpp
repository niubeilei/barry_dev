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
#if 0
#include "IILTrans/DistQueryDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "DocClient/DocidShufflerMgr.h"


AosIILTransDistQueryDocByName::AosIILTransDistQueryDocByName(const bool flag)
:
AosIILTrans(AosTransType::eDistQueryDocByName, flag AosMemoryCheckerArgs),
mQuery_bitmap(0),
mQuery_context(0)
{
}


AosIILTransDistQueryDocByName::AosIILTransDistQueryDocByName(
		const OmnString &iilname,
		const OmnString &docscanner_id,
		const u64 blocksize,
		const AosQueryContextObjPtr &query_context,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eDistQueryDocByName, iilname, false,
		snap_id, need_save, need_save AosMemoryCheckerArgs),
mQuery_context(query_context),
mDocScannerId(docscanner_id),
mBlockSize(blocksize)
{
}


bool
AosIILTransDistQueryDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocScannerId = buff->getOmnStr("");
	mBlockSize = buff->getU64(0);
	aos_assert_r(mDocScannerId != "" && mBlockSize!=0, false);

	u32 con_len = buff->getU32(0);
	if(con_len)
	{
		AosBuffPtr con_buff = buff->getBuff(con_len, false AosMemoryCheckerArgs);	
		mQuery_context = AosQueryContextObj::createQueryContextStatic();	
		
		AosXmlParser parser;
		AosXmlTagPtr xml_context = parser.parse(con_buff->data(), 
				con_len, "" AosMemoryCheckerArgs);
		mQuery_context->serializeFromXml(xml_context, mRdata);
	}
	
	mQuery_bitmap = AosBitmapMgrObj::getBitmapStatic();
	return true;	
}


bool
AosIILTransDistQueryDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mDocScannerId);
	buff->setU64(mBlockSize);
	
		
	AosXmlTagPtr xml_qr;
	mQuery_context->serializeToXml(xml_qr, mRdata);
	if(xml_qr)
	{
		buff->setU32(xml_qr->getDataLength());
		buff->setBuff((char *)xml_qr->getData(), xml_qr->getDataLength());
	}
	return true;
}


AosTransPtr 
AosIILTransDistQueryDocByName::clone()
{
	return OmnNew AosIILTransDistQueryDocByName(false);
}



bool
AosIILTransDistQueryDocByName::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);
	
	bool rslt = false;
	rslt = AosDocidShufflerMgr::getSelf()->sendStart(mDocScannerId, mBlockSize, rdata);
	aos_assert_r(rslt, false);
	int total_msgs = 0;
	while(!mQuery_context->finished())
	{
		mQuery_bitmap->reset();
		rslt = AosIILMgrObj::getIILMgr()->querySafeByIIL(iilobj, 0, mQuery_bitmap, mQuery_context, rdata);
		aos_assert_r(rslt, false);
		AosDocidShufflerMgr::getSelf()->shuffle(mDocScannerId, mQuery_bitmap, 1, rdata);
		total_msgs++;
	}
	AosDocidShufflerMgr::getSelf()->sendFinished(mDocScannerId, rdata);
	return true;
}


/*
u64 
AosIILTransDistQueryDocByName::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/



int
AosIILTransDistQueryDocByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransDistQueryDocByName::getIILType() const
{
	return eAosIILType_Invalid;
}

#endif
