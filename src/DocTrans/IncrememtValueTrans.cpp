////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocTrans/IncrememtValueTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"

AosIncrememtValueTrans::AosIncrememtValueTrans(const bool regflag)
:
AosDocTrans(AosTransType::eIncrememtValue, regflag)
{
}


AosIncrememtValueTrans::AosIncrememtValueTrans(
		const u64 docid,
		const OmnString &aname,
		const u64 init_value,
		const u64 inc_value,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eIncrememtValue, docid, need_save, need_resp, snap_id),
mDocid(docid),
mAname(aname),
mInitValue(init_value),
mIncValue(inc_value)
{
}


AosIncrememtValueTrans::~AosIncrememtValueTrans()
{
}


bool
AosIncrememtValueTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	aos_assert_r(mDocid != 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosIncrememtValueTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


AosTransPtr
AosIncrememtValueTrans::clone()
{
	return OmnNew AosIncrememtValueTrans(false);
}


bool
AosIncrememtValueTrans::proc()
{
	// This function is the server side implementations of AosXmlDoc::isDocDeleted(...)
	u64 newvalue = 0;
	bool rslt = AosDocSvr::getSelf()->incrementValue(mRdata, mDocid, mAname, 
			mInitValue, mIncValue, newvalue, getTransId());	
	if(!rslt)
	{
		OmnAlarm << "error!" << enderr;
	}
	
	//OmnString contents = "<Contents><record ";
	//contents << "rslt" << "=\"" << (rslt?"true":"false")<< "\" "
	//		 << "value" << "=\"" << newvalue << "\" "
	//		 << " /></Contents>";
	//rdata->setContents(contents);
	//rdata->setOk();
	
	AosBuffPtr resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU64(newvalue);
	
	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}
#endif
