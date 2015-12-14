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
// 2015/4/01 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "DataletProcs/GetDataletProc.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "JimoAPI/JimoDocFunc.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGetDataletProc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGetDataletProc(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosGetDataletProc::AosGetDataletProc(const int version)
:
AosDataletProc(version)
{
}


AosJimoPtr
AosGetDataletProc::cloneJimo() const
{
	return OmnNew AosGetDataletProc(mJimoVersion);
}


bool 
AosGetDataletProc::isValid() const
{
	return true;
}


bool 
AosGetDataletProc::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u32 func_id =jimo_call.getFuncID();
	switch (func_id)
	{
	case JimoDocFunc::eGetDataletByObjid:
	case JimoDocFunc::eGetDataletByDocid:
		return getDocByObjid(rdata, jimo_call);

	case JimoDocFunc::eIsDataletDeleted:
		return isDataletDeleted(rdata, jimo_call);

	case JimoDocFunc::eReadUnLockDatalet:
		return readUnLockDatalet(rdata, jimo_call);

	case JimoDocFunc::eReadLockDatalet:
		return readLockDatalet(rdata, jimo_call);

	case JimoDocFunc::eCheckDataletLock:
		return checkDocLock(rdata, jimo_call);

	case JimoDocFunc::eProcDataletLock:
		return procDocLock(rdata, jimo_call);

	case JimoDocFunc::eBatchGetDatalets:
		return batchGetDocs(rdata, jimo_call);

	case JimoDocFunc::eGetCSVDatalet:
		return getCSVDoc(rdata, jimo_call);

	case JimoDocFunc::eGetFixedDatalet:
		return getFixedDoc(rdata, jimo_call);

	default:
		 break;
	}

	return false;

}


bool
AosGetDataletProc::getDocByObjid(AosRundata *rdata, AosJimoCall &jimo_call)
{
	AosXmlTagPtr doc;
	OmnString objid = jimo_call.getOmnStr(rdata, AosFN::eObjid, "");
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	bool needbinarydata = jimo_call.getBool(rdata, AosFN::eFlag, false);
	u64 did = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	jimo_call.reset();

	u64 docid;
	
	if (objid != "")
	{
		docid = AosIILMgrObj::getIILMgr()->getDocidByObjid(objid, rdata);
		if (docid != 0)
		{
			int v1 = AosGetCubeId(docid);
			int v2 = AosGetCubeId(objid);
			if (v1 == v2)
			{
				doc = getDoc(rdata, docid, snap_id, needbinarydata);
			}
		}
	}
	else
	{
		doc = getDoc(rdata, did, snap_id, needbinarydata);
	}

	AosBuffPtr resp;
	if (!doc)
	{
		resp = OmnNew AosBuff(20 AosMemoryCheckerArgs);
		resp->setU8(false);
		resp->setU64(docid);
	}
	else
	{
		u32 doc_len = doc->getDataLength();
		resp = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp->setU8(true);
		resp->setU32(doc_len);
		resp->setBuff((char *)doc->getData(), doc_len);
	}

	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	
	return true;
}


AosXmlTagPtr
AosGetDataletProc::getDoc(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const u64 snap_id,
		const bool &needbinarydata)
{
	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getDoc(docid, snap_id, rdata);	
	if (!doc) return 0;

	if(doc->getAttrU64(AOSTAG_DOCID, 0) != docid) return 0;
	
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		if(doc) return 0;
	}

	if (needbinarydata)
	{
		//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", 0);
		//if (doc->getAttrU64(AOSTAG_BINARY_SOURCELEN, 0) == 0) return doc;

		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
		AosBuffPtr buff;
		bool rslt = AosDocSvrObj::getDocSvr()->retrieveBinaryDoc(doc, buff, rdata, snap_id);
		if (!rslt) 	return doc;

		aos_assert_r(buff->dataLen() > 0, doc);
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
		doc->setTextBinary(nodename, buff);
	}
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	return doc;
}


bool
AosGetDataletProc::isDataletDeleted(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function is the server side implementations of AosXmlDoc::isDocDeleted(...)

	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	jimo_call.reset();
	bool result = false;
	AosBuffPtr resp;
	bool rslt = AosDocSvrObj::getDocSvr()->isDocDeleted(docid, result, rdata);
	if(!rslt)
	{
		resp = OmnNew AosBuff(20 AosMemoryCheckerArgs);
		resp->setU8(false);
		resp->setU64(docid);
	}

	resp= OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	resp->setU8(result);
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool
AosGetDataletProc::readLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 userid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	int waittimer = jimo_call.getInt(rdata, AosFN::eValue1, 0);
	int holdtimer = jimo_call.getInt(rdata, AosFN::eValue2, 0);
	jimo_call.reset();

	// AosTransPtr thisptr(this, false);
	int status;
	bool rslt = AosDocSvrObj::getDocSvr()->readLockDoc(docid, userid, waittimer, holdtimer, 0, status, rdata);	

	AosBuffPtr resp;
	if(!rslt)
	{
		// 1. There are errors in reading the doc. 
		// 2. It failed locking the doc.
		switch (status)
		{
		case 0:
			 // Need to return an error;
			 resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp->setU8(false);
			 resp->setOmnStr(rdata->getErrmsg());
OmnScreen << "@@@@@@ readLockDocTrans error : " << userid << " errmsg : " << rdata->getErrmsg() << endl;
			 break;

		case 1:
			 // It means it failed obtaining the lock. It needs
			 // to wait until the lock becomes available.
			 // When the lock becomes available, it will call:
OmnScreen << "@@@@@@ readLockDocTrans wait : " << userid << endl;
			 return true;

		default:
			 resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			 resp->setU8(false);
			 OmnString errmsg = __FILE__;
			 errmsg << ":" << __LINE__ 
				 << " Internal Error: " << status;
			 resp->setOmnStr(errmsg);
OmnScreen << "@@@@@@ readLockDocTrans invalid : " << userid << " errmsg : " << errmsg << endl;
			 break;
		}
	}
	else
	{
		resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp->setU8(true);
OmnScreen << "@@@@@@ readLockDocTrans success : " << userid << endl;
	}
	
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


bool 
AosGetDataletProc::readUnLockDatalet(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 userid =  jimo_call.getU64(rdata, AosFN::eUserid, 0);
	jimo_call.reset();

	bool rslt = AosDocSvrObj::getDocSvr()->readUnLockDoc(docid, userid, rdata);

	AosBuffPtr resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	resp->setOmnStr(rdata->getErrmsg());
	
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool 
AosGetDataletProc::checkDocLock(AosRundata *rdata, AosJimoCall &jimo_call)
{
	jimo_call.reset();
	// This function is the server side implementations of AosCheckDocLock::check(...)

	bool result = false; //AosDocSvr::getSelf()->procCheckLock(mRdata, mDocid, mDType);

	//OmnString contents = "<Contents><record ";
	//contents << "result" << "=\"" << (result?"true":"false")<< "\" /></Contents>";
	//rdata->setContents(contents);
	//rdata->setOk();

	AosBuffPtr resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp->setU8(result);

	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool 
AosGetDataletProc::procDocLock(AosRundata *rdata, AosJimoCall &jimo_call)
{
	// This function is the server side implementations of AosDocLock::check(...)
	// AosTransPtr thisptr(this, false);
	bool rslt = false; //AosDocSvr::getSelf()->procDocLock(mRdata, thisptr, mDocid, mLockType, mLockTimer, mLockid);
	aos_assert_r(rslt, false);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool 
AosGetDataletProc::batchGetDocs(AosRundata *rdata, AosJimoCall &jimo_call)
{
	AosBuffPtr len_buff = jimo_call.getBuff(rdata, AosFN::eBuff, 0);
	u32 num_docs = jimo_call.getU32(rdata, AosFN::eValue1, 0);
	u32 blocksize = jimo_call.getU32(rdata, AosFN::eValue2, 0);
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	AosBuffPtr docid_buff = jimo_call.getBuff(rdata, AosFN::eValue, 0);
	jimo_call.reset();

	map<u64, int> size_len_map;
	while(len_buff && len_buff->getCrtIdx() < len_buff->dataLen())
	{
		u64 sizeid = len_buff->getU64(0);
		int len = len_buff->getInt(0);
		size_len_map.insert(make_pair(sizeid, len)); 
	}

	u64 end_docid = 0;

	AosBuffPtr buff = OmnNew AosBuff(blocksize AosMemoryCheckerArgs);
	bool rslt = AosDocSvrObj::getDocSvr()->getDocs(rdata, buff,
			num_docs, docid_buff, size_len_map, end_docid, snap_id);
	aos_assert_r(rslt, false);

	AosBuffPtr resp;
	if(!rslt)
	{
		resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp->setU8(false);
	}
	else
	{
		u32 doc_len = buff->dataLen();
		u32 size = doc_len + sizeof(u8) + sizeof (u64) + sizeof(u32);
		resp = OmnNew AosBuff(doc_len + size AosMemoryCheckerArgs);
		resp->setU8(true);
		resp->setU64(end_docid);
		resp->setU32(doc_len);
		resp->setBuff((char *)buff->data(), doc_len);
	}
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}



bool 
AosGetDataletProc::getCSVDoc(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	int record_len = jimo_call.getInt(rdata, AosFN::eLength, 0);
	jimo_call.reset();

	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getCSVDoc(docid, record_len, snap_id, rdata);	
	if(!doc)
	{
		jimo_call.arg(AosFN::eErrmsg, "getCSVDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}

	if(doc->getAttrU64(AOSTAG_DOCID, 0) != docid)
	{
		jimo_call.arg(AosFN::eErrmsg, "getCSVDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}

	AosBuffPtr resp;
	if(!doc)
	{
		resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp->setU8(false);
	}
	else
	{
		u32 doc_len = doc->getDataLength();
		resp = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp->setU8(true);
		resp->setU32(doc_len);
		resp->setBuff((char *)doc->getData(), doc_len);
	}
	
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);

	return true;
}


bool 
AosGetDataletProc::getFixedDoc(AosRundata *rdata, AosJimoCall &jimo_call)
{
	u64 docid = jimo_call.getU64(rdata, AosFN::eDocid, 0);
	u64 snap_id = jimo_call.getU64(rdata, AosFN::eSnapID, 0);
	int record_len = jimo_call.getInt(rdata, AosFN::eLength, 0);
	jimo_call.reset();

	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getFixedDoc(docid, record_len, snap_id, rdata);	
	if(!doc)
	{
		jimo_call.arg(AosFN::eErrmsg, "getFixedDoc_failed");
		jimo_call.setLogicalFail();
		jimo_call.sendResp(rdata);
		return true;
	}

	AosBuffPtr resp;

	if(!doc)
	{
		resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp->setU8(false);
	}
	else
	{
		if(doc->getAttrU64(AOSTAG_DOCID, 0) != docid)
		{
			resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
			resp->setU8(false);
		}
		else
		{
			u32 doc_len = doc->getDataLength();
			resp = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
			resp->setU8(true);
			resp->setU32(doc_len);
			resp->setBuff((char *)doc->getData(), doc_len);
		}
	}
	
	jimo_call.arg(AosFN::eBuff, resp);
	jimo_call.setSuccess();
	jimo_call.sendResp(rdata);
	return true;
}


