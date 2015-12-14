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
#include "DocTrans/CreateDocTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosCreateDocTrans::AosCreateDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eCreateDoc, regflag)
{
}


AosCreateDocTrans::AosCreateDocTrans(
		const u64 docid,
		const AosXmlTagPtr &new_doc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eCreateDoc, 
		docid, need_save, need_resp, snap_id),
mIsXml(true),
mDocid(docid),
mNewDoc(new_doc),
mDocBuff(0),
mDocLen(0)
{
}


AosCreateDocTrans::AosCreateDocTrans(
		const u64 docid,
		const char *new_doc,
		const int data_len,   
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eCreateDoc,
		docid, need_save, need_resp, snap_id),
mIsXml(false),
mDocid(docid),
mNewDoc(0),
mDocBuff(0),
mDocLen(data_len)
{
	mDocBuff = OmnNew AosBuff(mDocLen AosMemoryCheckerArgs);
	mDocBuff->setBuff(new_doc, mDocLen);
}


AosCreateDocTrans::~AosCreateDocTrans()
{
}


bool
AosCreateDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mIsXml = buff->getU8(0);
	mDocid = buff->getU64(0);
	if (mIsXml)
	{
		u32 data_len = buff->getU32(0);
		AosBuffPtr doc_buff = buff->getBuff(data_len, true AosMemoryCheckerArgs);

		AosXmlParser parser;
		mNewDoc = parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);

		aos_assert_r(mDocid && mNewDoc, false);
	}
	else
	{
		mDocLen = buff->getU32(0);
		mDocBuff = buff->getBuff(mDocLen, true AosMemoryCheckerArgs);
		aos_assert_r(mDocid && mDocLen, false);
	}
	setDistId(mDocid);
	return true;
}


bool
AosCreateDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU8(mIsXml);
	buff->setU64(mDocid);
	if (mIsXml)
	{
		u32 data_len = mNewDoc->getDataLength();
		buff->setU32(data_len);
		buff->setBuff((char *)mNewDoc->getData(), data_len);
	}
	else
	{
		aos_assert_r(mDocBuff && mDocLen > 0, false);
		buff->setU32(mDocLen);
		buff->setBuff(mDocBuff);
	}
	return true;
}


AosTransPtr
AosCreateDocTrans::clone()
{
	return OmnNew AosCreateDocTrans(false);
}


bool
AosCreateDocTrans::proc()
{
	// This function is the server side implementations of AosDocSvr::createDoc(...)
	// It assumes:
	// 	<request ...>
	// 		<doc>
	// 			<the_doc .../>
	// 		</doc>
	// 	</request>
	
	// Ketty 2012/11/30
	setFinishLater();

	if (!mIsXml)
	{
		aos_assert_r(mDocBuff && mDocLen > 0, false);
		bool rslt = AosDocSvrObj::getDocSvr()->createDoc(mRdata, mDocid, mDocBuff->data(), mDocLen, AosTransId::Invalid, mSnapshotId);
		aos_assert_r(rslt, false);
		return true;
	}

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
			//mNewDoc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
		}
	}

	rslt = AosDocSvrObj::getDocSvr()->createDoc(mRdata, mNewDoc, getTransId(), mSnapshotId);
	aos_assert_r(rslt, false);

	return true;
}


