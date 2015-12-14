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
#include "DocTrans/DeleteObjTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/IILMgrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosDeleteObjTrans::AosDeleteObjTrans(const bool regflag)
:
AosDocTrans(AosTransType::eDeleteObj, regflag)
{
}


AosDeleteObjTrans::AosDeleteObjTrans(
		const u64 docid,
		const AosXmlTagPtr &orig_doc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eDeleteObj, docid, need_save, need_resp, snap_id),
mDocid(docid),
mOrigDoc(orig_doc)
{
}


AosDeleteObjTrans::~AosDeleteObjTrans()
{
}


bool
AosDeleteObjTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	u32 data_len = buff->getU32(0);
	AosBuffPtr doc_buff = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	
	AosXmlParser parser;
	mOrigDoc = parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);
	
	aos_assert_r(mDocid && mOrigDoc, false);
	setDistId(mDocid);
	return true;
}


bool
AosDeleteObjTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	u32 data_len = mOrigDoc->getDataLength();
	buff->setU64(mDocid);
	buff->setU32(data_len);
	buff->setBuff((char *)mOrigDoc->getData(), data_len);
	return true;
}


AosTransPtr
AosDeleteObjTrans::clone()
{
	return OmnNew AosDeleteObjTrans(false);
}


bool
AosDeleteObjTrans::proc()
{
	// This function is the server side implementations of
	// 	AosDocSvr::deleteObj(...)
	// Ketty 2012/11/30
	setFinishLater();
	bool rslt = deleteObj();
	if (isNeedResp())
	{
		// Ketty temp.
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(rslt);
		
		sendResp(resp_buff);
	}
	return rslt;
}


bool
AosDeleteObjTrans::deleteObj()
{
	u32 siteid = mOrigDoc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);

	if (siteid != mRdata->getSiteid())
	{
		OmnAlarm << "Siteid mismatch: " << siteid
			<< ":" << mRdata->getSiteid() << enderr;
		mRdata->setSiteid(siteid);
	}

	u64 docid = mOrigDoc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString objid = mOrigDoc->getAttrStr(AOSTAG_OBJID, "");
	if (docid != mDocid || objid == "")
	{
		OmnAlarm << "docid or objid error: " << mDocid
			<< ":" << docid << ":" << objid << enderr;
		return false;
	}
	
	bool rslt = true;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		if (AosGetCubeId(objid) == AosGetCubeId(docid))
		{
			AosIILMgrObjPtr iilmgr = AosIILMgrObj::getIILMgr();
			rslt = iilmgr->unbindObjid(objid, docid, mRdata);
			if (!rslt)
			{
				mRdata->setError() << "failed to unbind objid:"
					<< objid << ":" << docid;
				return false;
			}
		}
	}
	
	// Linda, delete binary doc
	if (mOrigDoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		rslt = AosDocSvrObj::getDocSvr()->deleteBinaryDoc(
			mDocid, mRdata, getTransId(), mSnapshotId);	
		if (!rslt)
		{
			mRdata->setError() << "Failed to remove binary doc!" << mDocid;
			OmnAlarm << mRdata->getErrmsg() << enderr;
			return false;
		}
	}

	OmnScreen << "deleteDoc:" << "docid:" << mDocid
			<< "; transid:" << getTransId().toString() << endl;

	rslt = AosDocSvrObj::getDocSvr()->deleteObj(mRdata, mDocid, getTransId(), mSnapshotId);
	if (!rslt)
	{
		mRdata->setError() << "Failed to remove doc!" << mDocid;
		OmnAlarm << mRdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

