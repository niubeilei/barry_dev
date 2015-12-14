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
// 2014/05/15 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/CounterRangeTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "CounterUtil/CounterUtil.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosIILTransCounterRange::AosIILTransCounterRange(const bool flag)
:
AosIILTrans(AosTransType::eCounterRange, flag AosMemoryCheckerArgs),
mQueryContext(0),
mQueryRslt(0),
mQueryBitmap(0),
mCounterQuery(0)
{
}


AosIILTransCounterRange::AosIILTransCounterRange(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eCounterRange, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mCounterQuery(counter_query)
{
}


AosIILTransCounterRange::AosIILTransCounterRange(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eCounterRange, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context),
mQueryRslt(query_rslt),
mQueryBitmap(query_bitmap),
mCounterQuery(counter_query)
{
}


AosTransPtr
AosIILTransCounterRange::clone()
{
	return OmnNew AosIILTransCounterRange(false);
}


bool
AosIILTransCounterRange::serializeTo(const AosBuffPtr &buff)
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

	if(mCounterQuery)
	{
		buff->setU8(true);
		rslt = mCounterQuery->serializeToXml(xml);
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
AosIILTransCounterRange::serializeFrom(const AosBuffPtr &buff)
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
		xml = parser.parse(rs_buff->data(), len, "" AosMemoryCheckerArgs);
		aos_assert_r(xml, false);
		mCounterQuery = OmnNew AosCounterQuery();
		rslt = mCounterQuery->serializeFromXml(xml);
		aos_assert_r(rslt, false);
	}

	exist = buff->getU8(0);
	if (exist)
	{
		len = buff->getU32(0);
		aos_assert_r(len, false);
		rs_buff = buff->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(rs_buff, false);
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
AosIILTransCounterRange::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = true;
	// Linda, 2013/02/22
	if (!mCounterQuery)
	{
		u32 num_conds = mQueryContext->getNumMultiCond();
		if (num_conds <= 0)
		{
			rslt = AosIILMgrObj::getIILMgr()->querySafeByIIL(
				iilobj, mQueryRslt, mQueryBitmap, mQueryContext, rdata);
		}
		else
		{
			int64_t blocksize = mQueryContext->getBlockSize();
			for (u32 idx = 0; idx < num_conds; idx++)
			{
				rslt = mQueryContext->retrieveMultiCond(idx);
				if (!rslt) break;
				mQueryContext->setBlockSize(blocksize / num_conds);

				rslt = AosIILMgrObj::getIILMgr()->querySafeByIIL(
					iilobj, mQueryRslt, mQueryBitmap, mQueryContext, rdata);
				mQueryContext->updateMultiCond(idx);
				if (!rslt) break;
			}
		}
	}
	else
	{
		while(!mQueryContext->finished())
		{
			rslt = AosIILMgrObj::getIILMgr()->querySafeByIIL(
				iilobj, mQueryRslt, mQueryBitmap, mQueryContext, rdata);
			if (!rslt) break;
			if (!mCounterQuery) break;
			if (mQueryRslt->getNumDocs() == 0) break;
		
			mCounterQuery->aggregateMultiple(mQueryRslt, rdata);

			// Linda, 2012/12/17 temporary
			//if (mCounterQuery->getMapSize()>= AosCounterUtil::eMaxPerCounterEntry) break;
			bool with_value = mQueryRslt->isWithValues(); 
			mQueryRslt->clear();
			mQueryRslt ->setWithValues(with_value);
		}
	}

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, mRdata);
	aos_assert_r(rslt && xml, false);
	resp_buff->setU32(xml->getDataLength());
	resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());

	if(mQueryRslt)
	{
		if (mCounterQuery) mCounterQuery->copyData(mQueryRslt, rdata);
		rslt = mQueryRslt->serializeToXml(xml);
		aos_assert_r(rslt && xml, false);
		resp_buff->setU32(xml->getDataLength());
		resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());
	}

	if (mCounterQuery)
	{
		rslt = mCounterQuery->serializeToXml(xml);
		aos_assert_r(rslt && xml, false);
		resp_buff->setU32(xml->getDataLength());
		resp_buff->setBuff((char*)xml->getData(), xml->getDataLength());
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


AosIILType 
AosIILTransCounterRange::getIILType() const
{
	return eAosIILType_Invalid;
}


int
AosIILTransCounterRange::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}

