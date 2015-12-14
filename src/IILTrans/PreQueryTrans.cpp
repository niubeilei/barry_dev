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
#include "IILTrans/PreQueryTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosIILTransPreQuery::AosIILTransPreQuery(const bool flag)
:
AosIILTrans(AosTransType::ePreQuery, flag AosMemoryCheckerArgs),
mQueryContext(0)
{
}


AosIILTransPreQuery::AosIILTransPreQuery(
		const u64 &iilid,
		const AosQueryContextObjPtr &query_context,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::ePreQuery, iilid,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context)
{
}


AosIILTransPreQuery::AosIILTransPreQuery(
		const OmnString &iilname,
		const AosQueryContextObjPtr &query_context,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::ePreQuery, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mQueryContext(query_context)
{
}


AosTransPtr
AosIILTransPreQuery::clone()
{
	return OmnNew AosIILTransPreQuery(false);
}


bool
AosIILTransPreQuery::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mQueryContext, false);

	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, mRdata);
	aos_assert_r(rslt && xml, false);
	buff->setU32(xml->getDataLength());
	buff->setBuff((char *)xml->getData(), xml->getDataLength());

	return true;
}


bool
AosIILTransPreQuery::serializeFrom(const AosBuffPtr &buff)
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

	return true;
}


bool
AosIILTransPreQuery::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = iilobj->preQuerySafe(mQueryContext, rdata);

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, rdata);
	aos_assert_r(rslt && xml, false);
	resp_buff->setU32(xml->getDataLength());
	resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());

	return true;
}


AosIILType 
AosIILTransPreQuery::getIILType() const
{
	return eAosIILType_Invalid;
}


int
AosIILTransPreQuery::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}

