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
#include "IILTrans/StrIncrementDocidByNameTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DocUtil/DocUtil.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrIncrementDocidByName::AosIILTransStrIncrementDocidByName(const bool flag)
:
AosIILTrans(AosTransType::eStrIncrementDocidByName, flag AosMemoryCheckerArgs),
//mIILName(""),
mKey(""),
mIncValue(0),
mInitValue(0)
{
}


AosIILTransStrIncrementDocidByName::AosIILTransStrIncrementDocidByName(
		const OmnString &iilname,
		const bool isPersis,
		const OmnString &key,
		const u64 inc_value,
		const u64 init_value,
		const bool add_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrIncrementDocidByName, 
		iilname, isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(inc_value),
mInitValue(init_value),
mAddFlag(add_flag)
{
}

//Ketty 2012/10/24
/*
bool
AosIILTransStrIncrementDocidByName::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	mTransId = trans->getTransId();	
	bool rslt = AosIILTrans::initTransBaseByName(trans_doc, mIILName, rdata);
	if(!rslt) return rslt;

	mKey = trans_doc->getAttrStr("key");
	if (mKey == "")
	{
		mKey = trans_doc->getNodeText("key");
	}
	if (mKey == "")
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Doc: " << trans_doc->toString() << enderr;
		return false;
	}
	
	mIncValue = trans_doc->getAttrU64("incValue", 0);
	mInitValue = trans_doc->getAttrU64("initValue", 0);
	mAddFlag = trans_doc->getAttrBool("addflag");
	return true;
}
*/

AosTransPtr 
AosIILTransStrIncrementDocidByName::clone()
{
	return OmnNew AosIILTransStrIncrementDocidByName(false);
}


bool
AosIILTransStrIncrementDocidByName::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mKey = buff->getOmnStr("");
	mIncValue = buff->getU64(0);
	mInitValue = buff->getU64(0);
	mAddFlag = buff->getU8(0);

	aos_assert_r(mKey != "", false);
	return true;
}


bool
AosIILTransStrIncrementDocidByName::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mKey);
	buff->setU64(mIncValue);
	buff->setU64(mInitValue);
	buff->setU8(mAddFlag);
	return true;
}


bool
AosIILTransStrIncrementDocidByName::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);

	aos_assert_rr(iilobj->getIILType() == eAosIILType_Str, rdata, false);

	u64 value;
	bool rslt = iilobj->incrementDocidSafe(mKey, value, mIncValue, mInitValue, mAddFlag, rdata);

	resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(value);
	//OmnString str;
	//str << "<rsp rslt =\"" << rslt << "\" "
	//	<< "value = \"" << value << "\" />";
	//rdata->setContents(str);
	return rslt;
}


/*
u64 
AosIILTransStrIncrementDocidByName::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/



int
AosIILTransStrIncrementDocidByName::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransStrIncrementDocidByName::getIILType() const
{
	bool isNumAlpha = AosDocUtil::isNumAlpha(mIILName.data(), mIILName.length());
	if(isNumAlpha) return eAosIILType_NumAlpha;
	return eAosIILType_Str;
}

