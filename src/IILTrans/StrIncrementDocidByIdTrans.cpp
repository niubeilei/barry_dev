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
#include "IILTrans/StrIncrementDocidByIdTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
//#include "TransUtil/XmlTrans.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrIncrementDocidById::AosIILTransStrIncrementDocidById(const bool flag)
:
AosIILTrans(AosTransType::eStrIncrementDocidById, flag AosMemoryCheckerArgs),
mKey(""),
mIncValue(0),
mInitValue(0),
mAddFlag(false)
{
}


// Ketty 2012/11/12
AosIILTransStrIncrementDocidById::AosIILTransStrIncrementDocidById(
		const u64 iilid,
		const bool isPersis,
		const OmnString &key,
		const u64 incValue,
		const u64 initValue,
		const bool addFlag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrIncrementDocidById, iilid,
		isPersis, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mKey(key),
mIncValue(incValue),
mInitValue(initValue),
mAddFlag(addFlag)
{
}


//Ketty 2012/10/24
/*
bool
AosIILTransStrIncrementDocidById::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans_doc, false);
	mTransId = trans->getTransId();	
	bool rslt = AosIILTrans::initTransBaseById(trans_doc, rdata);
	if(!rslt) return rslt;

	mKey = trans_doc->getAttrStr("key", "");
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

bool
AosIILTransStrIncrementDocidById::serializeFrom(const AosBuffPtr &buff)
{
	// Ketty 2012/11/12

	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mKey = buff->getOmnStr("");
	mIncValue = buff->getU64(0);
	mInitValue = buff->getU64(0);
	mAddFlag = (bool)buff->getU8(0);
	
	if (mKey == "")
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	return true;
}


bool
AosIILTransStrIncrementDocidById::serializeTo(const AosBuffPtr &buff)
{
	// Ketty 2012/11/12
	
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mKey);	
	buff->setU64(mIncValue);
	buff->setU64(mInitValue);
	buff->setU8(mAddFlag);
	return true;
}


bool
AosIILTransStrIncrementDocidById::proc(
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
AosIILTransStrIncrementDocidById::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDById(rdata);
}
*/

AosTransPtr 
AosIILTransStrIncrementDocidById::clone()
{
	return OmnNew AosIILTransStrIncrementDocidById(false);
}


int
AosIILTransStrIncrementDocidById::getSerializeSize() const 
{
	OmnNotImplementedYet;
	return 0;
}


AosIILType 
AosIILTransStrIncrementDocidById::getIILType() const
{
	return eAosIILType_Str;
}

