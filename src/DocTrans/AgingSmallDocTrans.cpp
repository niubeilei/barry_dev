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
// 11/04/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/AgingSmallDocTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"
//#include "StreamFS/SmallDocStore.h"
#include "SEInterfaces/SmallDocStoreObj.h"


AosAgingSmallDocTrans::AosAgingSmallDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eAgingSmallDoc, regflag)
{
}


AosAgingSmallDocTrans::AosAgingSmallDocTrans(
		const u64 docid,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eAgingSmallDoc, docid, need_save, need_resp, 0),
mDocid(docid)
{
}


AosAgingSmallDocTrans::~AosAgingSmallDocTrans()
{
}


bool
AosAgingSmallDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	aos_assert_r(mDocid, false);
	setDistId(mDocid);

	return true;
}


bool
AosAgingSmallDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


AosTransPtr
AosAgingSmallDocTrans::clone()
{
	return OmnNew AosAgingSmallDocTrans(false);
}


bool
AosAgingSmallDocTrans::proc()
{
	AosTransId trans_id = getTransId();
	bool rslt = AosSmallDocStoreObj::getSmallDocStoreObj()->ageOutDocs(mRdata, mDocid, trans_id);
	if (!rslt)
	{
		mRdata->setError() << "Failed to remove binary doc!" << mDocid;
		OmnAlarm << mRdata->getErrmsg() << enderr;
	}

	if(isNeedResp())
	{
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(rslt);
		sendResp(resp_buff);
	}
	return true;
}
