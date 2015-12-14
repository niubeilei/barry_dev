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
#include "DocTrans/ModifyObjTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

AosModifyObjTrans::AosModifyObjTrans(const bool regflag)
:
AosDocTrans(AosTransType::eModifyObj, regflag)
{
}


AosModifyObjTrans::AosModifyObjTrans(
		const u64 docid,
		const AosXmlTagPtr &new_doc,
		const AosXmlTagPtr &orig_doc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eModifyObj, docid, need_save, need_resp, snap_id),
mDocid(docid),
mNewDoc(new_doc),
mOrigDoc(orig_doc)
{
}


AosModifyObjTrans::~AosModifyObjTrans()
{
}


bool
AosModifyObjTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	u32 data_len = buff->getU32(0);
	AosBuffPtr doc_buff = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	
	AosXmlParser parser;
	mNewDoc= parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);
	
	aos_assert_r(mDocid && mNewDoc, false);
	setDistId(mDocid);
	return true;
}


bool
AosModifyObjTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	u32 data_len = mNewDoc->getDataLength();
	buff->setU64(mDocid);
	buff->setU32(data_len);
	buff->setBuff((char *)mNewDoc->getData(), data_len);
	return true;
}


AosTransPtr
AosModifyObjTrans::clone()
{
	return OmnNew AosModifyObjTrans(false);
}


bool
AosModifyObjTrans::proc()
{
	// This function is the server side implementations of AosDocSvr::modifyObj(...)

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
		//aos_assert_r(buff && buff->dataLen() >0, false);
		if (buff && buff->dataLen() > 0)
		{
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
		}
	}

	rslt = AosDocSvrObj::getDocSvr()->modifyObj(mRdata, mNewDoc, mDocid, getTransId(), mSnapshotId);
	aos_assert_r(rslt, false);
	return true;
}


