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
#include "DocTrans/IsDocDeletedTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"

AosIsDocDeletedTrans::AosIsDocDeletedTrans(const bool regflag)
:
AosDocTrans(AosTransType::eIsDocDeleted, regflag)
{
}


AosIsDocDeletedTrans::AosIsDocDeletedTrans(
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eIsDocDeleted, docid, need_save, need_resp, snap_id),
mDocid(docid)
{
}


AosIsDocDeletedTrans::~AosIsDocDeletedTrans()
{
}


bool
AosIsDocDeletedTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	aos_assert_r(mDocid != 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosIsDocDeletedTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


AosTransPtr
AosIsDocDeletedTrans::clone()
{
	return OmnNew AosIsDocDeletedTrans(false);
}


bool
AosIsDocDeletedTrans::proc()
{
	// This function is the server side implementations of AosXmlDoc::isDocDeleted(...)

	bool result = false;
	bool rslt = AosDocSvrObj::getDocSvr()->isDocDeleted(mDocid, result, mRdata);	
	if(!rslt)
	{
		OmnAlarm << "error" << enderr;
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU8(result);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	
	//OmnString contents = "<Contents><record ";
	//contents << "result" << "=\"" << (result?"true":"false")<< "\" "
	//		    << " /></Contents>";
	return true;
}

