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
#include "DocTrans/BatchGetDocsTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"

AosBatchGetDocsTrans::AosBatchGetDocsTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eBatchGetDocs, regflag)
{
}


AosBatchGetDocsTrans::AosBatchGetDocsTrans(
		const OmnString &scanner_id,
		const int server_id, 
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eBatchGetDocs, server_id, need_save, need_resp),
mScannerId(scanner_id)
{
}


AosBatchGetDocsTrans::~AosBatchGetDocsTrans()
{
}


bool
AosBatchGetDocsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mScannerId = buff->getOmnStr("");
	aos_assert_r(mScannerId != "", false);
	return true;
}


bool
AosBatchGetDocsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mScannerId);
	return true;
}


AosTransPtr
AosBatchGetDocsTrans::clone()
{
	return OmnNew AosBatchGetDocsTrans(false);
}


bool
AosBatchGetDocsTrans::proc()
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)
	AosBuffPtr buff;
	bool rslt = AosDocSvr::getSelf()->batchGetDocs(mScannerId, buff, mRdata);
	aos_assert_r(rslt, false);

	//OmnString contents = "<Contents><record><docs><![BDATA[";
	//contents << resp_buff->dataLen() << ":";
	//contents.append(resp_buff->data(), resp_buff->dataLen());
	//contents << "]]></docs></record></Contents>";
	AosBuffPtr resp_buff;
	if (!buff)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 data_len = buff->dataLen();
		resp_buff = OmnNew AosBuff(data_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);                                       
		resp_buff->setU32(data_len);                                   
		resp_buff->setBuff(buff);          
	}
	
	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}
#endif
