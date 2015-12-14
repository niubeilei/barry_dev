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
#include "DocTrans/AddStatBinaryDocidTrans.h"

#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"
#include "DataStructs/StatIdMgr.h"
#include "DataStructs/DistBlock.h"

AosAddStatBinaryDocidTrans::AosAddStatBinaryDocidTrans(const bool regflag)
:
AosDocTrans(AosTransType::eAddStatBinaryDocid, regflag)
{
}


AosAddStatBinaryDocidTrans::AosAddStatBinaryDocidTrans(
		const u64 ctrl_docid,
		const u64 stat_id,
		const u64 binary_docid,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eAddStatBinaryDocid, ctrl_docid, need_save, need_resp),
mCtrlDocid(ctrl_docid),
mStatId(stat_id),
mBinaryDocid(binary_docid)
{
}


AosAddStatBinaryDocidTrans::~AosAddStatBinaryDocidTrans()
{
}


bool
AosAddStatBinaryDocidTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCtrlDocid = buff->getU64(0);
	aos_assert_r(mCtrlDocid, false);
	mStatId = buff->getU64(0);
	mBinaryDocid = buff->getU64(0);
	setDistId(mCtrlDocid);
	return true;
}


bool
AosAddStatBinaryDocidTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mCtrlDocid);
	buff->setU64(mStatId);
	buff->setU64(mBinaryDocid);
	return true;
}


AosTransPtr
AosAddStatBinaryDocidTrans::clone()
{
	return OmnNew AosAddStatBinaryDocidTrans(false);
}


bool
AosAddStatBinaryDocidTrans::proc()
{
	AosBuffPtr resp_buff;
	AosXmlTagPtr doc = 0;
	AosBuffPtr buff = retrieveBinaryDoc(doc);
	aos_assert_r(buff, false);
	aos_assert_r(doc, false);

	bool rslt = false;
	int docs_per_distblock = doc->getAttrU64("zky_doc_per_distblock", 0);
	aos_assert_r(docs_per_distblock > 0, false);

	AosStatIdMgrPtr statidmgr = OmnNew AosStatIdMgr(buff, docs_per_distblock);
	aos_assert_r(statidmgr, false);

	u64 binary_docid = statidmgr->getEntryByStatId(mStatId);
	if (binary_docid != 0)
	{
		resp_buff = OmnNew AosBuff(16 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU64(binary_docid);
		sendResp(resp_buff);
		return true;
	}
	rslt = statidmgr->addEntryByStatId(mStatId, mBinaryDocid);
	aos_assert_r(rslt, false);

	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, false);

	OmnString signature;
	rslt = AosDocSvr::getSelf()->saveBinaryDoc(doc, buff, signature, mRdata, getTransId());
	aos_assert_r(rslt, false);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(false);
	sendResp(resp_buff);
	return true;
}


AosBuffPtr
AosAddStatBinaryDocidTrans::retrieveBinaryDoc(AosXmlTagPtr &doc)
{
	AosBuffPtr buff;
	doc = AosDocSvr::getSelf()->getDoc(mCtrlDocid, mRdata);
	aos_assert_r(doc, 0);
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, mRdata);
	if (!buff || buff->dataLen() <= 0)
	{
		aos_assert_r(!rslt, 0);
		buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		buff->setInt64(0);
	}
	return buff;
}
#endif
