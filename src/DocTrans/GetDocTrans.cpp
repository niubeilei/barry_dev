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
#include "DocTrans/GetDocTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "XmlUtil/XmlTag.h"

AosGetDocTrans::AosGetDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eGetDoc, regflag)
{
}


AosGetDocTrans::AosGetDocTrans(
		const u64 &docid,
		const bool need_binarydata,
		const u64 &snap_id)
:
AosDocTrans(AosTransType::eGetDoc, docid, false, true, snap_id),
mDocid(docid),
mObjid(""),
mNeedBinaryData(need_binarydata)
{
}


AosGetDocTrans::AosGetDocTrans(
		const OmnString &objid,
		const bool need_binarydata,
		const u64 &snap_id)
:
AosDocTrans(AosTransType::eGetDoc, AosGetHashKey(objid), false, true, snap_id),
mDocid(0),
mObjid(objid),
mNeedBinaryData(need_binarydata)
{
}


AosGetDocTrans::~AosGetDocTrans()
{
}


bool
AosGetDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mObjid = buff->getOmnStr("");
	mNeedBinaryData = buff->getU8(0);
	aos_assert_r(mDocid != 0 || mObjid != "", false);
	
	if (mDocid)
	{
		setDistId(mDocid);
	}
	else
	{
		setDistId(AosGetHashKey(mObjid));
	}
	return true;
}


bool
AosGetDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setOmnStr(mObjid);
	buff->setU8(mNeedBinaryData);
	return true;
}


AosTransPtr
AosGetDocTrans::clone()
{
	return OmnNew AosGetDocTrans(false);
}


bool
AosGetDocTrans::proc()
{
	AosXmlTagPtr doc;
	if (mObjid != "")
	{
		mDocid = AosIILMgrObj::getIILMgr()->getDocidByObjid(mObjid, mRdata);
		if (mDocid != 0)
		{
			int v1 = AosGetCubeId(mDocid);
			int v2 = AosGetCubeId(mObjid);
			if (v1 == v2)
			{
				doc = getDoc();
			}
		}
	}
	else
	{
		doc = getDoc();
	}

	AosBuffPtr resp_buff;
	if (!doc)
	{
		resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
		resp_buff->setU64(mDocid);
	}
	else
	{
		u32 doc_len = doc->getDataLength();
		resp_buff = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff((char *)doc->getData(), doc_len);
	}
	
	sendResp(resp_buff);
	return true;
}


AosXmlTagPtr
AosGetDocTrans::getDoc()
{
	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getDoc(mDocid, mSnapshotId, mRdata);	
	if (!doc) return 0;

	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == mDocid, 0);
	
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, 0);
	}

	if (mNeedBinaryData)
	{
		//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", 0);
		//if (doc->getAttrU64(AOSTAG_BINARY_SOURCELEN, 0) == 0) return doc;

		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
		AosBuffPtr buff;
		bool rslt = AosDocSvrObj::getDocSvr()->retrieveBinaryDoc(doc, buff, mRdata, mSnapshotId);
		if (!rslt) 	return doc;

		aos_assert_r(buff->dataLen() > 0, doc);
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
		doc->setTextBinary(nodename, buff);
	}
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	return doc;
}


bool
AosGetDocTrans::procGetResp()
{
	return proc();
}

