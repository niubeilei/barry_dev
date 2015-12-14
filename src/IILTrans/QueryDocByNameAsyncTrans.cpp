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
#include "IILTrans/QueryDocByNameAsyncTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILMgrObj.h" 
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosIILTransQueryDocByNameAsync::AosIILTransQueryDocByNameAsync(const bool flag)
:
AosIILTrans(AosTransType::eQueryDocByNameAsync, flag AosMemoryCheckerArgs),
mQueryContext(0),
mQueryRslt(0),
mQueryBitmap(0),
mRespCaller(0),
mReqId(0)
{
}


AosIILTransQueryDocByNameAsync::AosIILTransQueryDocByNameAsync(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eQueryDocByNameAsync, iilname,
	false, snapId, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mRespCaller(resp_caller),
mReqId(reqId)
{
}


AosIILTransQueryDocByNameAsync::~AosIILTransQueryDocByNameAsync()
{
}

	
AosTransPtr 
AosIILTransQueryDocByNameAsync::clone()
{
	return OmnNew AosIILTransQueryDocByNameAsync(false);
}


bool
AosIILTransQueryDocByNameAsync::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mQueryContext, false);

	buff->setU64(mReqId);
	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, mRdata);
	aos_assert_r(rslt && xml, false);
	buff->setU32(xml->getDataLength());
	buff->setBuff((char *)xml->getData(), xml->getDataLength());

	if(mQueryRslt)
	{
		buff->setU8(true);
		rslt = mQueryRslt->serializeToXml(xml);
		aos_assert_r(rslt && xml, false);
		buff->setU32(xml->getDataLength());
		buff->setBuff((char *)xml->getData(), xml->getDataLength());
	}
	else                   
	{
		buff->setU8(false);
	}

	if(mQueryBitmap) 
	{
		buff->setU8(true);
		AosBuffPtr bitmap_buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs); 
		mQueryBitmap->reset();
		mQueryBitmap->saveToBuff(bitmap_buff);
		buff->setU32(bitmap_buff->dataLen()); 
		buff->setBuff(bitmap_buff);
	}
	else                   
	{
		buff->setU8(false);
	}

	return true;
}


bool
AosIILTransQueryDocByNameAsync::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mReqId = buff->getU64(0);
	u32 len = buff->getU32(0);
	aos_assert_r(len, false);
	AosBuffPtr rs_buff = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_r(rs_buff, false);
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(rs_buff->data(), len, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	mQueryContext = AosQueryContextObj::createQueryContextStatic();
	aos_assert_r(mQueryContext, false);
	rslt = mQueryContext->serializeFromXml(xml, mRdata);
	aos_assert_r(rslt, false);

	bool exist = buff->getU8(0);
	if (exist)
	{
		len = buff->getU32(0);
		aos_assert_r(len, false);
		rs_buff = buff->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(rs_buff, false);
		xml = parser.parse(rs_buff->data(), len, "" AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		mQueryRslt = AosQueryRsltObj::getQueryRsltStatic();
		aos_assert_r(mQueryRslt, false);
		rslt = mQueryRslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);
	}

	exist = buff->getU8(0);
	if (exist)
	{
		len = buff->getU32(0);
		aos_assert_r(len, false);
		rs_buff = buff->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(rs_buff, false);
		//mQueryBitmap = AosBitmapObj::getBitmapStatic();
		mQueryBitmap = AosGetBitmap();
		aos_assert_r(mQueryBitmap, false);
		rslt = mQueryBitmap->loadFromBuff(rs_buff);
		aos_assert_r(rslt, false);
	}
	
	aos_assert_r(mQueryRslt || mQueryBitmap, false);
	if(mQueryRslt && mQueryBitmap && mQueryBitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(mQueryBitmap);
		mQueryBitmap = 0;
		return false;
	}
	
	return true;
}


bool
AosIILTransQueryDocByNameAsync::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);

	bool rslt = AosIILMgrObj::getIILMgr()->querySafeByIIL(
		iilobj, mQueryRslt, mQueryBitmap, mQueryContext, rdata);

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(mReqId);

	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, rdata);
	aos_assert_r(rslt && xml, false);
	resp_buff->setU32(xml->getDataLength());
	resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());

	if(mQueryRslt)
	{
		rslt = mQueryRslt->serializeToXml(xml);
		aos_assert_r(rslt && xml, false);
		resp_buff->setU32(xml->getDataLength());
		resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());
	}
	
	if(!mQueryRslt && mQueryBitmap)
	{
		mQueryBitmap->reset();
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		mQueryBitmap->saveToBuff(buff);
		resp_buff->setU32(buff->dataLen());
		resp_buff->setBuff(buff);
	}
	
	if(mQueryBitmap)
	{
		AosBitmapMgrObj::returnBitmapStatic(mQueryBitmap);
		mQueryBitmap = 0;
	}

	return true;
}


bool
AosIILTransQueryDocByNameAsync::respCallBack()
{
	// Ketty 2013/07/20
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();
	
	// will call BinaryDocScanner::callback();
	// will call BinaryDocScanner::binaryDocTransCallback();
	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


int
AosIILTransQueryDocByNameAsync::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransQueryDocByNameAsync::getIILType() const
{
	return eAosIILType_Invalid;
}

