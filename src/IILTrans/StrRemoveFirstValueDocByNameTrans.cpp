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
#include "IILTrans/StrRemoveFirstValueDocByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "DocUtil/DocUtil.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrRemoveFirstValueDocByName::AosIILTransStrRemoveFirstValueDocByName(const bool flag)
:
AosIILTrans(AosTransType::eStrRemoveFirstValueDocByName, flag AosMemoryCheckerArgs),
mValue(""),
mReverse(false)
{
}


AosIILTransStrRemoveFirstValueDocByName::AosIILTransStrRemoveFirstValueDocByName(
		const OmnString &iilname,
		const OmnString &value,
		const bool reverse,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrRemoveFirstValueDocByName, 
		iilname, false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mValue(value),
mReverse(reverse)
{
}


//Ketty 2012/10/24
/*
bool
AosIILTransStrRemoveFirstValueDocByName::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(trans_doc, rdata, false);
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	mTransId = trans->getTransId();
	if(!rslt) return rslt;

	mValue = trans_doc->getAttrStr("value");
	if (mValue == "")
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Doc: " << trans_doc->toString() << enderr;
		return false;
	}
	
	mReverse = trans_doc->getAttrBool("reverse", false);
	return true;
}
*/

AosTransPtr 
AosIILTransStrRemoveFirstValueDocByName::clone()
{
	return OmnNew AosIILTransStrRemoveFirstValueDocByName(false);
}


bool
AosIILTransStrRemoveFirstValueDocByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mValue = buff->getOmnStr("");
	mReverse = buff->getU8(0);
	return true;
}


bool
AosIILTransStrRemoveFirstValueDocByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mValue);
	buff->setU8(mReverse);
	return true;
}


bool
AosIILTransStrRemoveFirstValueDocByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);
	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	u64 docid = 0;
	bool rslt = iilobj->removeFirstValueDocSafe(mValue, docid, mReverse, rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(docid);
	//OmnString str;
	//str << "<rsp rslt=\"" << rslt << "\" docid=\"" << docid << "\" /> ";
	//rdata->setContents(str);
	return true;
}


AosIILType 
AosIILTransStrRemoveFirstValueDocByName::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName, mIILName.length());
	if(isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}


/*
u64
AosIILTransStrRemoveFirstValueDocByName::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/


int
AosIILTransStrRemoveFirstValueDocByName::getSerializeSize() const
{
	OmnNotImplementedYet;
	return false;
}
