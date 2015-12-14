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
// 04/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrGetSplitValueTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosIILTransStrGetSplitValue::AosIILTransStrGetSplitValue(const bool flag)
:
AosIILTrans(AosTransType::eStrGetSplitValue, flag AosMemoryCheckerArgs),
mNumBlocks(0)
{
}


AosIILTransStrGetSplitValue::AosIILTransStrGetSplitValue(
		const OmnString &iilname,
		const int num_blocks,
		const AosQueryRsltObjPtr &query_rslt,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrGetSplitValue, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mNumBlocks(num_blocks),
mQuery_rslt(query_rslt)
{
}
	

//Ketty 2012/10/24
/*
bool
AosIILTransStrGetSplitValue::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	mTransId = trans->getTransId();	
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	if(!rslt) return rslt;

	AosXmlTagPtr qryrslt = trans_doc->getFirstChild("query_rslt");
	if(qryrslt)
	{
		mQuery_rslt = AosQueryRsltObj::getQueryRsltStatic();
		mQuery_rslt->serializeFromXml(qryrslt);
	}

	mNumBlocks = trans_doc->getAttrInt("num_blocks", 0);

	aos_assert_r(mQuery_rslt , false);
	
	return true;
}
*/

AosTransPtr 
AosIILTransStrGetSplitValue::clone()
{
	return OmnNew AosIILTransStrGetSplitValue(false);
}


bool
AosIILTransStrGetSplitValue::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mNumBlocks = buff->getInt(-1);	
	u32 rs_len = buff->getU32(0);
	if(rs_len)
	{
		AosBuffPtr rs_buff = buff->getBuff(rs_len, false AosMemoryCheckerArgs);
		mQuery_rslt = AosQueryRsltObj::getQueryRsltStatic();
	
		AosXmlParser parser;
		AosXmlTagPtr xml_rs = parser.parse(rs_buff->data(), 
				rs_len, "" AosMemoryCheckerArgs);
		mQuery_rslt->serializeFromXml(xml_rs);
	}
	aos_assert_r(mQuery_rslt , false);
	return true;
}


bool
AosIILTransStrGetSplitValue::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mNumBlocks);
	if(mQuery_rslt)
	{
		AosXmlTagPtr xml;	
		mQuery_rslt->serializeToXml(xml);
		if(xml)
		{
			buff->setU32(xml->getDataLength());
			buff->setBuff((char *)xml->getData(), xml->getDataLength());
		}
	}
	return true;
}


bool
AosIILTransStrGetSplitValue::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{	
	aos_assert_rr(iilobj, rdata, false);

	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	bool rslt = false;
	OmnNotImplementedYet;
	//striil->getSplitValueSafe(mNumBlocks, mQuery_rslt, false,rdata);
	
	// Ketty 2013/03/18
	/*
	OmnString str;
	str << "<rsp rslt =\"" << rslt << "\" >";

	if(mQuery_rslt)
	{
		AosXmlTagPtr xml;
		rslt = mQuery_rslt->serializeToXml(xml);
		aos_assert_r(rslt, false);
		str << xml->toString();
	}
	str << "</rsp>";
	
	rdata->setContents(str);
	*/
	return rslt;
}


/*
u64 
AosIILTransStrGetSplitValue::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/

AosIILType 
AosIILTransStrGetSplitValue::getIILType() const
{
	return eAosIILType_Str;
}

	
int
AosIILTransStrGetSplitValue::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}

