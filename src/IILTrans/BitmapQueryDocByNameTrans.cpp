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
#include "IILTrans/BitmapQueryDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILMgrObj.h" 
#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

extern int sgQueryTorturerFlag;

AosIILTransBitmapQueryDocByName::AosIILTransBitmapQueryDocByName(const bool flag)
:
AosIILTrans(AosTransType::eBitmapQueryDocByName, flag AosMemoryCheckerArgs),
mQueryContext(0),
mQueryRslt(0),
mQueryBitmap(0)
{
}


AosIILTransBitmapQueryDocByName::AosIILTransBitmapQueryDocByName(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eBitmapQueryDocByName,
	iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap)
{
}


AosIILTransBitmapQueryDocByName::~AosIILTransBitmapQueryDocByName()
{
}

	
AosTransPtr 
AosIILTransBitmapQueryDocByName::clone()
{
	return OmnNew AosIILTransBitmapQueryDocByName(false);
}


bool
AosIILTransBitmapQueryDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mQueryContext, false);

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

	if (mQueryBitmap && !mQueryBitmap->isEmpty()) 
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
AosIILTransBitmapQueryDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

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
		// There is query result
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
		// There is query bitmap
		len = buff->getU32(0);
		aos_assert_r(len, false);
		rs_buff = buff->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(rs_buff, false);
		// mQueryBitmap = AosBitmapObj::getBitmapStatic();
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
AosIILTransBitmapQueryDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_r(iilobj, false);

	mQueryBitmap = AosBitmapMgrObj::getBitmapStatic();
	bool rslt = iilobj->bitmapQueryNewSafe(
		mQueryRslt, mQueryBitmap, mQueryContext, rdata);

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, rdata);
	aos_assert_r(rslt && xml, false);
	resp_buff->setU32(xml->getDataLength());
	resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());

	if(mQueryRslt)
	{
		resp_buff->setU8(true);
		rslt = mQueryRslt->serializeToXml(xml);
		aos_assert_r(rslt && xml, false);
		resp_buff->setU32(xml->getDataLength());
		resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());
	}
	else
	{
		resp_buff->setU8(false);
	}
	
	if(mQueryBitmap)
	{
		resp_buff->setU8(true);
		mQueryBitmap->reset();
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		mQueryBitmap->saveToBuff(buff);
		resp_buff->setU32(buff->dataLen());
		resp_buff->setBuff(buff);
	}
	else
	{
		resp_buff->setU8(false);
	}
	
	return true;
}


int
AosIILTransBitmapQueryDocByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransBitmapQueryDocByName::getIILType() const
{
	return eAosIILType_Invalid;
}

