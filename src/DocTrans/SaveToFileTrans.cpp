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
#include "DocTrans/SaveToFileTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

AosSaveToFileTrans::AosSaveToFileTrans(const bool regflag)
:
AosDocTrans(AosTransType::eSaveToFile, regflag)
{
}


AosSaveToFileTrans::AosSaveToFileTrans(
		const u64 docid,
		const AosXmlTagPtr new_doc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eSaveToFile, docid, need_save, need_resp, snap_id),
mDocid(docid),
mNewDoc(new_doc)
{
}


AosSaveToFileTrans::~AosSaveToFileTrans()
{
}


bool
AosSaveToFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	u32 data_len = buff->getU32(0);
	AosBuffPtr doc_buff = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	
	AosXmlParser parser;
	mNewDoc= parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);
	
	aos_assert_r(mDocid && mNewDoc, false);
	aos_assert_r(mNewDoc->getAttrU64(AOSTAG_DOCID, 0) == mDocid, false);
	setDistId(mDocid);
	return true;
}


bool
AosSaveToFileTrans::serializeTo(const AosBuffPtr &buff)
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
AosSaveToFileTrans::clone()
{
	return OmnNew AosSaveToFileTrans(false);
}


bool
AosSaveToFileTrans::proc()
{
	// Ketty 2012/11/30
	setFinishLater();

//OmnScreen << "ketttttttttty:: saveDoc:"
//		<< " docid:" << docid
//		<< "; transid: " << trans->getTransId()
//		<< endl;

	bool rslt = AosDocSvrObj::getDocSvr()->saveToFile(mDocid, mNewDoc, mRdata, getTransId(), mSnapshotId);
	if (!rslt)
	{
		mRdata->setError() << "Failed saving to file: " << mRdata->getErrmsg();
		OmnAlarm << mRdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


