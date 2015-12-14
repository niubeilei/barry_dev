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
#include "DocTrans/CreateDocSafeTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"

AosCreateDocSafeTrans::AosCreateDocSafeTrans(const bool regflag)
:
AosDocTrans(AosTransType::eCreateSafeDoc, regflag)
{
}


AosCreateDocSafeTrans::AosCreateDocSafeTrans(
		const u64 docid,
		const AosXmlTagPtr &new_doc,
		const bool save_doc_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eCreateSafeDoc, docid, need_save, need_resp, snap_id),
mDocid(docid),
mSaveDocFlag(save_doc_flag),
mNewDoc(new_doc)
{
}


AosCreateDocSafeTrans::~AosCreateDocSafeTrans()
{
}


bool
AosCreateDocSafeTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mSaveDocFlag = buff->getU8(0);
	u32 data_len = buff->getU32(0);
	AosBuffPtr doc_buff  = buff->getBuff(data_len, false AosMemoryCheckerArgs);
	
	AosXmlParser parser;
	mNewDoc = parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);
	
	aos_assert_r(mDocid && mNewDoc, false);
	setDistId(mDocid);
	return true;
}


bool
AosCreateDocSafeTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	u32 data_len = mNewDoc->getDataLength();
	buff->setU64(mDocid);
	buff->setU8(mSaveDocFlag);
	buff->setU32(data_len);
	buff->setBuff((char *)mNewDoc->getData(), data_len);
	return true;
}


AosTransPtr
AosCreateDocSafeTrans::clone()
{
	return OmnNew AosCreateDocSafeTrans(false);
}


bool
AosCreateDocSafeTrans::proc()
{
	// This function is the server side implementations of AosDocSvr::createDocSafe(...)
	
	// Ketty 2012/11/30
	setFinishLater();

	u32 siteid = mNewDoc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != mRdata->getSiteid())
	{
	   	OmnAlarm << "Siteid mismatch: " << siteid << ":" << mRdata->getSiteid() << enderr;
	    mRdata->setSiteid(siteid);
	}
	
	// Linda 2012/08/20 save Binary Doc
	bool rslt = false;
	if (mNewDoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		OmnString nodename = mNewDoc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
		AosBuffPtr buff = mNewDoc->getNodeTextBinaryCopy(nodename AosMemoryCheckerArgs);
		aos_assert_r(buff && buff->dataLen() >0, false);
		if (nodename == "")
		{
			mNewDoc->removeNodeTexts();
		}
		else
		{
			mNewDoc->removeNode(nodename, false, false);
		}
		OmnString signature;
		rslt = AosDocSvrObj::getDocSvr()->saveBinaryDoc(mNewDoc, buff, signature, mRdata, getTransId(), mSnapshotId);
		aos_assert_r(rslt, false);
		aos_assert_r(signature != "", false);
		//mNewDoc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
	}

	rslt = AosDocSvrObj::getDocSvr()->createDocSafe(mRdata, mNewDoc, mSaveDocFlag, getTransId(), mSnapshotId);
	aos_assert_r(rslt, false);

	return rslt;
}


