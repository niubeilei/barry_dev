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
#include "IILTrans/GetSplitValueByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosIILTransGetSplitValueByName::AosIILTransGetSplitValueByName(const bool flag)
:
AosIILTrans(AosTransType::eGetSplitValueByName, flag AosMemoryCheckerArgs),
mSize(-1),
mQueryContext(0)
{
}


AosIILTransGetSplitValueByName::AosIILTransGetSplitValueByName(
		const OmnString &iilname,
		const int size,
		const AosQueryContextObjPtr &context,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eGetSplitValueByName, iilname, false,
		snap_id, need_save, need_resp AosMemoryCheckerArgs),
mSize(size),
mQueryContext(context)
{
}


AosTransPtr 
AosIILTransGetSplitValueByName::clone()
{
	return OmnNew AosIILTransGetSplitValueByName(false);
}


bool
AosIILTransGetSplitValueByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mSize);
		
	AosXmlTagPtr xml;
	rslt = mQueryContext->serializeToXml(xml, mRdata);
	aos_assert_r(rslt && xml, false);
	buff->setU32(xml->getDataLength());
	buff->setBuff((char *)xml->getData(), xml->getDataLength());

	return true;
}


bool
AosIILTransGetSplitValueByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mSize = buff->getInt(-1);
	aos_assert_r(mSize > 0, false);

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
AosIILTransGetSplitValueByName::proc(
		const AosIILObjPtr &iilobj, 
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);

	vector<AosQueryContextObjPtr> contexts;
	bool rslt = iilobj->getSplitValueSafe(mQueryContext, mSize, contexts, rdata);
	
	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setI64(mQueryContext->getTotalDocInRslt());

	AosXmlTagPtr xml;
	int size = contexts.size();
	if (size == 0)
	{
		resp_buff->setInt(1);	

		rslt = mQueryContext->serializeToXml(xml, rdata);
		aos_assert_r(rslt && xml, false);
		resp_buff->setU32(xml->getDataLength());
		resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());
	}
	else
	{
		resp_buff->setInt(size);	

		for(int i=0; i<size; i++)
		{
			aos_assert_r(contexts[i], false);
			rslt = contexts[i]->serializeToXml(xml, rdata);
			aos_assert_r(rslt && xml, false);
			resp_buff->setU32(xml->getDataLength());
			resp_buff->setBuff((char *)xml->getData(), xml->getDataLength());
		}
	}

	return true;
}


int
AosIILTransGetSplitValueByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransGetSplitValueByName::getIILType() const
{
	return eAosIILType_Invalid;
}

