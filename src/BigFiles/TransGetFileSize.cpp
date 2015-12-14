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
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigFile/TransGetFileSize.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"

AosTransGetFileSize::AosTransGetFileSize(const bool regflag)
:
AosCubicTrans(AosTransType::eGetFileSize, regflag)
{
}


AosTransGetFileSize::AosTransGetFileSize(
		const int phyid,
		const u64 fileid)
:
AosTaskTrans(AosTransType::eGetFileSize, phyid, false, true),
mPhyicalId(phyid),
mFileId(fileid)
{
}


AosTransGetFileSize::~AosTransGetFileSize()
{
}


bool
AosTransGetFileSize::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mPhysicalId = buff->getInt(-1);
	mFileId = buff->getU64(0);
	mFilename = buff->getStr("");
	aos_assert_r(mPhysicalid >= 0, false);
	return true;
}


bool
AosTransGetFileSize::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mPhysicalId);
	buff->setU64(mFileId);
	return true;
}


AosTransPtr
AosTransGetFileSize::clone()
{
	return OmnNew AosTransGetFileSize(false);
}


bool
AosTransGetFileSize::proc()
{
	AosXmlTagPtr doc = getDoc();	

	AosBuffPtr resp_buff;
	if(!doc)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 doc_len = doc->getDataLength();
		resp_buff = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff((char *)doc->getData(), doc_len);
	}
	
	//resp = "<Contents><record>";
	//resp << "<doc>" << doc->toString() << "</doc>"
	//	<< "</record></Contents>";

	// Ketty 2013/07/23
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	sendResp(resp_buff);
	return true;
}


AosXmlTagPtr
AosTransGetFileSize::getDoc()
{
	//OmnScreen << "getDoc:" 
	//		<< "docid:" << mDocid
	//		<< "; transid:" << getTransId() 
	//		<< endl;

	AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(mDocid, mRdata);	
	if(!doc)	return 0;

	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == mDocid, 0);
	
	//Linda 2012/08/21 get binary doc
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, 0);
	}

	if (mNeedBinaryData)
	{
		//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", 0);
		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
		AosBuffPtr buff;
		bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, mRdata);
		if (!rslt) 	return 0;
		aos_assert_r(buff->dataLen() > 0, doc);
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
		doc->setTextBinary(nodename, buff);
	}
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	return doc;
}


