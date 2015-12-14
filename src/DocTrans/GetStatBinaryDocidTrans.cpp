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
#include "DocTrans/GetStatBinaryDocidTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "DataStructs/StatIdMgr.h"
#include "XmlUtil/XmlTag.h"

AosGetStatBinaryDocidTrans::AosGetStatBinaryDocidTrans(const bool regflag)
:
AosDocTrans(AosTransType::eGetStatBinaryDocid, regflag)
{
}


AosGetStatBinaryDocidTrans::AosGetStatBinaryDocidTrans(
		const u64 ctrl_docid,
		const u64 stat_id,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eGetStatBinaryDocid, ctrl_docid, false, need_resp),
mCtrlDocid(ctrl_docid),
mStatId(stat_id)
{
}


AosGetStatBinaryDocidTrans::~AosGetStatBinaryDocidTrans()
{
}


bool
AosGetStatBinaryDocidTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCtrlDocid = buff->getU64(0);
	mStatId = buff->getU64(0);
	setDistId(mCtrlDocid);
	return true;
}


bool
AosGetStatBinaryDocidTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mCtrlDocid);
	buff->setU64(mStatId);
	return true;
}


AosTransPtr
AosGetStatBinaryDocidTrans::clone()
{
	return OmnNew AosGetStatBinaryDocidTrans(false);
}


bool
AosGetStatBinaryDocidTrans::proc()
{
	u64 binary_docid = 0;
	AosXmlTagPtr doc = 0;
	AosBuffPtr buff = retrieveBinaryDoc(doc);
	if (buff)
	{
		aos_assert_r(doc, false);
		int docs_per_distblock = doc->getAttrU64("zky_doc_per_distblock", 0);
		aos_assert_r(docs_per_distblock > 0, false);

		AosStatIdMgrPtr statidmgr = OmnNew AosStatIdMgr(buff, docs_per_distblock);
		aos_assert_r(statidmgr, false);
		binary_docid = statidmgr->getEntryByStatId(mStatId);
	}

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU64(binary_docid);
	sendResp(resp_buff);
	return true;
}


AosBuffPtr
AosGetStatBinaryDocidTrans::retrieveBinaryDoc(AosXmlTagPtr &doc)
{
	AosBuffPtr buff;
	doc = AosDocSvr::getSelf()->getDoc(mCtrlDocid, mRdata);
	aos_assert_r(doc, 0);
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, mRdata);
	if (rslt && buff->dataLen() > 0) return buff;
	aos_assert_r(!rslt, 0);
	return 0;
}
#endif
