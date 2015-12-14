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
// 2014/11/22 Created by Chen Ding
// 2015/03/14 Copied from DocPackage.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoDocEngine.h"

#include "Debug/ErrorMsg.h"
#include "JimoAPI/JimoPackage.h"
#include "JimoAPI/JimoDocFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoAPI/JimoData.h"
#include "JimoCall/JimoCallMgr.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallAsync.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallSyncNorm.h"
#include "JimoCall/JimoCallSyncWrite.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"
#include "Util/StrSplit.h"
#include "Util/String.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"
#include "UserMgmt/UserAcct.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include <vector>

static OmnString sgJimoCallHandler = "AosDocEngineJimoCallHandler";

namespace Jimo
{

bool jimoCreateDoc(
		AosRundata *rdata,
		const OmnString &objid,
		const u64 snap_id,
		const OmnString &doc_type,
		const bool keep_objid,
		u64 &docid,
		const char *doc,
		const int length)
{
	// This function creates a doc of 'doc_type':
	// objid:	If it is not empty, this is the objid the user chose for the doc.
	//          If it is empty, the system will assign an objid for it.
	// snap_id	If this is inside a transaction, snap_id identifies the transaction.
	//          0 means it is not inside a transaction.
	// doc_type	It identifies the doc type. It is mandatory (at least for now).
	// keep_objid	If objid is empty, this param is ignored. Otherwise, if its
	//          vale is true, it will fail the call if the objid is used by someone
	//          else. If it is false, the system will resolve the objid as needed.
	// docid  	If it is not zero, 'objid' shall not be empty and it means
	// 			(objid, docid) should have been reserved.
	// doc and length	These are the body of the doc.
	// jimo_call_ptr	This is the jimo call the system returns. Users may use this
	//          parm to carry more data from the call (reserved for future enhancements).
	/*
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC);
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC);

	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call_ptr;
	if (objid != "")
	{
		u32 distr_id = cluster->getDistrID(rdata, objid);
		jimo_call_ptr = OmnNew AosJimoCallSyncWrite(rdata,
			"AosDocEngineJimoCallHandler", JimoDocFunc::eCreateDoc,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	}
	else
	{
		jimo_call_ptr = OmnNew AosJimoCallAsyncNorm(rdata,
			sgJimoCallHandler, JimoDocFunc::eCreateDoc, cluster);
	}

	AosJimoCall *jimo_call = jimo_call_ptr.getPtrNoLock();
	jimo_call->arg(AosFN::eDocType, "XML");
	jimo_call->arg(AosFN::eMethod, "CreateDoc");
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eKeepObjid, keep_objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, doc, length);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		AosLogError(rdata, true, "failed_creating_doc") << enderr;
		return false;
	}

	docid = jimo_call->getU64(rdata, AosFN::eDocid, 0);
	aos_assert_rg(docid > 0, rdata, false, AosErrMsg("docid_is_null"));
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool jimoGetDocByName(
		AosRundata *rdata,
		AosXmlTagPtr &doc,
		const OmnString &container_objid,
		const OmnString &key_fieldname,
		const OmnString &key_value)
{
	aos_assert_rr(container_objid != "", rdata, false);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, container_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByName,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue1, container_objid);
	jimo_call->arg(AosFN::eValue2, key_fieldname);
	jimo_call->arg(AosFN::eValue3, key_value);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);

	return true;
}


AosXmlTagPtr jimoGetDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const u32 rep_policy,
		AosClusterObj *cluster)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByDocid,
		distr_id, 0, cluster);
	jimo_call->arg(AosFN::eValue1, docid);
	jimo_call->arg(AosFN::eValue2, rep_policy);

	bool rslt = jimo_call->makeCall(rdata);
	if (!rslt)
	{
		OmnAlarm << " call failed ! " << enderr;
		return 0;
	}

	jimo_call->waitForFinish();

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


u64 jimoCreateDoc(
		AosRundata *rdata, 
		const AosXmlTagPtr &doc)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC);
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC);

	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDoc, -1, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eMethod, "CreateDoc");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
	
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateDoc failed, errmsg: " << errmsg << enderr;
		jimo_call->dump();
		return 0;
	}
	
	u64 docid = jimo_call->getU64(rdata, AosFN::eDocid, 0);
	return docid;
}


bool jimoDeleteObj(
		AosRundata *rdata,
		const OmnString &docid,
		const OmnString &theobjid,
		const OmnString &container,
		const bool deleteFlag)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteObj,
		distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eMethod, "DeleteObj");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eObjid, theobjid);
	jimo_call->arg(AosFN::eContainer, container);
	jimo_call->arg(AosFN::eFlag, deleteFlag);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoDeleteObj failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	return true;
}


AosXmlTagPtr jimoGetDocByObjid(
		AosRundata *rdata,
		const OmnString &objid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByObjid,
		distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetDoc");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	if (docstr == "")
		return 0;

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}



bool jimoAddReq(
		AosRundata *rdata,
		const AosTransPtr &trans)
{
	OmnAlarm << enderr;
	return false;
}


bool jimoAppendBinaryDoc(
		AosRundata *rdata,
		const OmnString &objid,
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return true;
}


bool jimoBatchDataReceived(
		AosRundata *rdata,
		const OmnString &scannerid,
		const AosBuffPtr &contents,
		const int serverid,
		const bool finished,
		const int num)
{
	aos_assert_rr(scannerid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, scannerid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eBatchDataReceived,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, scannerid);
	jimo_call->arg(AosFN::eBuff, contents);
	jimo_call->arg(AosFN::eValue1, serverid);
	jimo_call->arg(AosFN::eFlag, finished);
	jimo_call->arg(AosFN::eValue2, num);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}


void jimoBatchGetDocs(
		AosRundata *rdata,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &sizeid_len,
		const AosAsyncRespCallerPtr &resp_caller)
{
	OmnAlarm << enderr;
	return ;
}


AosBuffPtr jimoBatchGetDocs(
		AosRundata *rdata,
		const OmnString scanner_id,
		const int server_id)
{
	OmnNotImplementedYet
	return 0;
}


bool jimoBatchSendDocids(
		AosRundata *rdata,
		const OmnString &scanner_id,
		const int serverid,
		const AosBitmapObjPtr &bitmap)
{
	OmnNotImplementedYet
	return false;
}


bool jimoBatchSendDocids(
		AosRundata *rdata,
		const OmnString &scanner_id,
		const int serverid,
		const AosBitmapObjPtr &bitmap,
		const AosDocClientCallerPtr &caller)
{
	OmnNotImplementedYet
	return false;
}


bool jimoBindCloudid_doc(
		AosRundata *rdata,
		const OmnString &cid,
		const u64 &docid)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(cid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eBindCloudid_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "BindCloudid");
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoBindCloudid_doc failed, errmsg: " << errmsg << enderr;
		return false;
	}

	bool flag = jimo_call->getBool(rdata, AosFN::eFlag, false);
    return flag;
}


bool jimoCreateDoc1(
		AosRundata *rdata,
		const AosXmlTagPtr &cmd,
		const OmnString &target_cid,
		const AosXmlTagPtr &newdoc,
		const bool resolveObjid,
		const AosDocSvrCbObjPtr &caller,
		void *userdata,
		const bool keepDocid)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(target_cid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, target_cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDoc1,
		distr_id, cluster, cluster);
	
	OmnString cmdstr;
	if(cmd)	cmdstr = cmd->toString();
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "CreateDoc1");
	jimo_call->arg(AosFN::eValue, cmdstr);
	jimo_call->arg(AosFN::eCloudid, target_cid);
	jimo_call->arg(AosFN::eDoc, newdoc->toString());
	jimo_call->arg(AosFN::eValue1, resolveObjid);
	jimo_call->arg(AosFN::eValue2, keepDocid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	aos_assert_r(!caller, false);
	return true;
}


AosXmlTagPtr jimoCreateDocSafe1(
		AosRundata *rdata,
		const OmnString &docstr,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid,
		const bool reserved,
		const bool cid_required,
		const bool check_ctnr)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docstr!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docstr);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDocSafe1,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "CreateDocSafe1");
	jimo_call->arg(AosFN::eDoc, docstr);
	jimo_call->arg(AosFN::eCloudid, cloudid);
	jimo_call->arg(AosFN::eObjid, objid_base);
	jimo_call->arg(AosFN::eFlag, is_public);
	jimo_call->arg(AosFN::eValue, checkCreation);
	jimo_call->arg(AosFN::eValue1, keepDocid);
	jimo_call->arg(AosFN::eReverse, reserved);
	jimo_call->arg(AosFN::eValue2, cid_required);
	jimo_call->arg(AosFN::eValue3, check_ctnr);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateDocSafe1 failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoCreateDocSafe3(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid,
		const bool reserved,
		const bool cid_required,
		const bool check_ctnr,
		const bool saveDocFlag)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(doc, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, doc->toString());

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDocSafe3,
		distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "CreateDocSafe3");
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eCloudid, cloudid);
	jimo_call->arg(AosFN::eObjid, objid_base);
	jimo_call->arg(AosFN::eFlag, is_public);
	jimo_call->arg(AosFN::eValue, checkCreation);
	jimo_call->arg(AosFN::eValue1, keepDocid);
	jimo_call->arg(AosFN::eReverse, reserved);
	jimo_call->arg(AosFN::eValue2, cid_required);
	jimo_call->arg(AosFN::eValue3, check_ctnr);
	jimo_call->arg(AosFN::eSave, saveDocFlag);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateDocSafe3 failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	if( docstr == "" )
	{
		OmnAlarm << "jimoCreateDocSafe3_failed, docstr_is_null, prames [ cloudid:" << cloudid
			   	 << " objid_base:" << objid_base << " is_public:" << is_public                 
			     << " checkCreation:" << checkCreation << " keepDocid:" << keepDocid           
			     << " reserved:" << reserved << " cid_required:" << cid_required               
			     << " check_ctnr" << check_ctnr << " saveDocFlag:" << saveDocFlag << "  ]" << enderr;     
	}
	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosAccessRcdPtr jimoGetAccessRcd(
		AosRundata *rdata,
		const AosXmlTagPtr &ref_doc,
		const u64 &ref_docid,
		const OmnString &ref_objid,
		const bool parent_flag)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(ref_docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, ref_docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetAccessRcd,
		distr_id, cluster, cluster);
	
	OmnString docstr;
	if(ref_doc.getPtr()!=0)
	{
		docstr = ref_doc->toString();
	}
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetAccessRcd");
	jimo_call->arg(AosFN::eDoc, docstr);
	jimo_call->arg(AosFN::eDocid, ref_docid);
	jimo_call->arg(AosFN::eObjid, ref_objid);
	jimo_call->arg(AosFN::eFlag, parent_flag);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetAccessRcd failed, errmsg" << errmsg <<enderr;
		return 0;
	}

	docstr = jimo_call->getOmnStr(rdata, AosFN::eValue);
	AosXmlTagPtr acddoc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(acddoc, 0);

	AosAccessRcdPtr arcd = OmnNew AosAccessRcd(acddoc, rdata);

	return arcd;
}


AosXmlTagPtr jimoGetDoc(
		AosRundata *rdata,
		const u64 &docid,
		const OmnString &objid,
		bool &duplicated)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDoc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eFlag, duplicated);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


u64 jimoGetDocidByCloudid(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &cid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(cid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocidByCloudid_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDocidByCloudid");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eDocid, 0);
}


u64 jimoGetDocidByObjid(
		AosRundata *rdata,
		const OmnString &objid,
		const bool flag)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocidByObjid_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDocidByObjid");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eDocid, 0);
}

AosXmlTagPtr jimoGetDocByDocid(
		AosRundata *rdata,
		const u64 docid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByU64Docid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetDoc");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm<<"call failed"<<enderr;
		return 0;
	}
	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	if (docstr == "")
		return 0;

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoGetDocByDocid(
		AosRundata *rdata,
		const OmnString &docid)
{
	aos_assert_rr(docid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByStrDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm<<"call failed"<<enderr;
		return 0;
	}
	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoGetDocByCloudid(
		AosRundata *rdata,
		const OmnString &cid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(cid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByCloudid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetDocByCloudid");
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetDocByCloudid failed on cid:" << cid 
			     << " errmsg:" << errmsg << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}

AosUserAcctObjPtr jimoGetUserAcct(
		AosRundata *rdata,
		const u64 &user_docid)
{
	//Xuqi 2015/11/10
	/*
	aos_assert_rr(user_docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, user_docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetUserAcct,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetUserAcct");
	jimo_call->arg(AosFN::eDocid, user_docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr1 = jimo_call->getOmnStr(rdata, AosFN::eValue1);
	AosXmlTagPtr doc = AosXmlParser::parse(docstr1);

	OmnString docstr2 = jimo_call->getOmnStr(rdata, AosFN::eValue2);
	AosXmlTagPtr userOprArd = AosXmlParser::parse(docstr2);

	bool OprArdTried = jimo_call->getBool(rdata, AosFN::eValue3);
	AosUserAcct* useracct = OmnNew AosUserAcct(doc, rdata);

	useracct->setValue(userOprArd, OprArdTried);
	return AosUserAcctObjPtr(useracct, false);
	*/
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	AosXmlTagPtr userdoc = jimoGetDocByDocid(rdata, user_docid);       
	if (!userdoc) return 0;                                        
	AosUserMgmtObjPtr usrmgmt_obj = AosUserMgmtObj::getUserMgmt(); 
	aos_assert_r(usrmgmt_obj, 0);                                  
	return usrmgmt_obj->getUserAcct(userdoc, rdata);
}

AosXmlTagPtr jimoGetDeletedDoc(
		AosRundata *rdata,
		const OmnString &objid,
		const u64 &docid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDeletedDoc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDeletedDoc");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetDeletedDoc failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


OmnString jimoGetObjidByDocid(
		AosRundata *rdata,
		const u64 &docid)
{
	/*
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetObjidByDocid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetObjidByDocid");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return "";
	}

	return jimo_call->getOmnStr(rdata, AosFN::eObjid, "");
    */
	AosXmlTagPtr doc = jimoGetDocByDocid(rdata, docid);
	if (!doc) return "";                           
	OmnString ss = doc->getAttrStr(AOSTAG_OBJID);  
	doc = 0;                                       
	return ss; 
}


bool jimoIsCloudidBound(
		AosRundata *rdata,
		const OmnString &cid,
		u64 &userid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(cid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eIsCloudidBound,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "IsCloudidBound");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoIsCloudidBound failed" << errmsg << enderr;
		return false;
	}

	return true;
}


bool jimoIsShowLog()
{
	AosRundata* rdata= OmnApp::getRundata().getPtr();
	aos_assert_r(rdata, false);

	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoCallHandler, JimoDocFunc::eIsShowLog, cluster);
	jimo_call->makeCall(rdata);
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm<<"call failed"<<enderr;
		return false;
	}

	return jimo_call->getBool(rdata, AosFN::eFlag, false);
}


/*
AosXmlTagPtr jimoGetDoc(
		AosRundata *rdata,
		const u64 &docid,
		const OmnString &objid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDoc,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");

	return AosXmlParser::parse(docstr);
}
*/
OmnString jimoGetCloudid(
		AosRundata *rdata,
		const u64 &user_docid)
{
	/*
	aos_assert_rr(user_docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, user_docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetCloudid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eUserid, user_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return "";
	}

	return jimo_call->getOmnStr(rdata, AosFN::eCloudid, "");
	*/
	AosUserAcctObjPtr acct = jimoGetUserAcct(rdata, user_docid); 
	if (!acct) return "";                                    
	return acct->getCloudid();
}

AosAccessRcdPtr jimoGetAccessRecord(
		AosRundata *rdata,
		const u64 &docid,
		const bool parent_flag)
{
	return jimoGetAccessRcd(rdata, 0, docid, "", parent_flag);
}



bool jimoInsertInto(
		AosRundata *rdata,
		const AosXmlTagPtr &data)
{
	OmnNotImplementedYet;
	return false;
}


OmnString jimoIncrementValue(
		AosRundata *rdata,
		const u64 docid,
		const OmnString &objid,
		const OmnString &aname,
		const OmnString &initvalue,
		const bool value_unique,
		const bool docid_unique)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eIncrementValue,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "IncrementValue");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eValue, aname);
	jimo_call->arg(AosFN::eValue1, initvalue);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);//for iil, value is the key of index
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoIncrementValue failed errmsg: " << errmsg << enderr;
		return "";
	}

	return jimo_call->getOmnStr(rdata, AosFN::eValue, "");
}


bool jimoIsLog(const u64 &docid)
{
	/*
	AosRundata* rdata= OmnApp::getRundata().getPtr();
	aos_assert_r(rdata, false);

	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoCallHandler, JimoDocFunc::eIsLog, cluster);


	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm<<"call failed"<<enderr;
		return false;
	}

	return jimo_call->getBool(rdata, AosFN::eFlag, false);
    */
	AosDocType::E type = AosDocType::getDocType(docid);
	if (type == AosDocType::eLogDoc) return true;      
	return false;
}


bool jimoIsRepairing()
{
	//Xuqi 2015/11/3
	AosRundata* rdata= OmnApp::getRundata().getPtr();
	aos_assert_r(rdata, false);

	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eIsRepairing, -1, cluster, cluster);

	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "IsRepairing");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoIsRepairing failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return jimo_call->getBool(rdata, AosFN::eFlag, false);
}


bool jimoModifyAttrStr1(
		AosRundata *rdata,
		const u64 &docid,
		const OmnString &objid,
		const OmnString &aname,
		const OmnString &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		const bool need_trigger)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyAttrStr1,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "ModifyAttrStr1");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eName, aname);
	jimo_call->arg(AosFN::eValue, newvalue);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, checkSecurity);
	jimo_call->arg(AosFN::eValue1, need_trigger);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoModifyAttrStr1 failed, errmsg: " << errmsg <<enderr;
		return false;
	}

	return true;
}


bool jimoModifyAttrStr(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const OmnString &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		const bool need_trigger)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(aname != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, aname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyAttrStr,
		distr_id, cluster, cluster);
	
	aos_assert_r(doc, false);
	jimo_call->arg(AosFN::eMethod, "ModifyAttrStr");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eName, aname);
	jimo_call->arg(AosFN::eValue, newvalue);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, checkSecurity);
	jimo_call->arg(AosFN::eValue1, need_trigger);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoModifyAttrStr failed, errmsg:" << errmsg <<enderr;
		return false;
	}

	return true;
}


bool jimoModifyAttrU64(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const u64 &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		const bool need_trigger)
{
	aos_assert_rr(aname != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, aname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyAttrU64,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	aos_assert_r(doc, false);
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eName, aname);
	jimo_call->arg(AosFN::eValue, newvalue);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, checkSecurity);
	jimo_call->arg(AosFN::eValue1, need_trigger);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoModifyAttrXPath(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const OmnString &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		const bool need_trigger)
{
	aos_assert_rr(aname != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, aname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyAttrXPath,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	aos_assert_r(doc, false);
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eName, aname);
	jimo_call->arg(AosFN::eValue, newvalue);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, checkSecurity);
	jimo_call->arg(AosFN::eValue1, need_trigger);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoModifyObj(
		AosRundata *rdata,
		const AosXmlTagPtr &doc)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyObj, -1, cluster, cluster);

	aos_assert_r(doc, false);
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eMethod, "ModifyDoc");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoModifyObj failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoModifyObj(
		AosRundata *rdata,
		const AosXmlTagPtr &newxml,
		const OmnString &rename,
		const bool synobj)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(rename != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, rename);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyObjAdd,
		distr_id, cluster, cluster);
	
	aos_assert_r(newxml, false);
	jimo_call->arg(AosFN::eMethod, "ModifyDoc");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDoc, newxml->toString());
	jimo_call->arg(AosFN::eName, rename);
	jimo_call->arg(AosFN::eFlag, synobj);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoModifyObj failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}


bool jimoPurgeDoc(
		AosRundata *rdata,
		const OmnString &objid)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::ePurgeDocByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoPurgeDoc(
		AosRundata *rdata,
		const u64 &docid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::ePurgeDocBydocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoPurgeDocByCloudid(
		AosRundata *rdata,
		const OmnString &cid)
{
	aos_assert_rr(cid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::ePurgeDocByCloudid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoWriteLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const int waittimer,
		const int holdtimer)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteLockDocByTime,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue1, waittimer);
	jimo_call->arg(AosFN::eValue2, holdtimer);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoWriteUnlockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const AosXmlTagPtr &newdoc)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteUnlockDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	aos_assert_r(newdoc, false);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, newdoc->toString());
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

AosXmlTagPtr jimoWriteLockDocByObjid(
		AosRundata *rdata,
		const OmnString &objid)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteLockDocByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoWriteLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteLockDoc,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}

bool jimoWriteAccessDoc(
		AosRundata *rdata,
		const u64 &owndocid,
		AosXmlTagPtr &arcd)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(owndocid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, owndocid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteAccessDoc,
		distr_id, cluster, cluster);
	
	aos_assert_r(arcd, false);
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "WriteAccessDoc");
	jimo_call->arg(AosFN::eDocid, owndocid);
	jimo_call->arg(AosFN::eValue, arcd->toString());
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoWriteAccessDoc failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc);
	arcd = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	bool flag = jimo_call->getBool(rdata, AosFN::eFlag, false);
	return flag;
}

AosXmlTagPtr jimoReleaseWriteLock(
		AosRundata *rdata,
		const AosXmlTagPtr &doc)
{
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoCallHandler, JimoDocFunc::eWriteAccessDoc, cluster);

	aos_assert_r(doc, 0);
	jimo_call->arg(AosFN::eValue, doc->toString());
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoReleaseWriteLockDocByObjid(
		AosRundata *rdata,
		const OmnString &objid)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eReleaseWriteLockDocByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoReleaseWriteLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eReleaseWriteLockDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoCreateBinaryDoc(
		AosRundata *rdata,
		const OmnString &docstr,
		const bool is_public,
		const AosBuffPtr &buff)
{
	aos_assert_rr(docstr != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docstr);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateBinaryDoc,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDoc, docstr);
	jimo_call->arg(AosFN::eFlag, is_public);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoCreateBinaryDoc(
		AosRundata *rdata,
		const OmnString &docstr,
		const bool is_public,
		const AosBuffPtr &buff,
		const int &vid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docstr != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docstr);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateBinaryDocByVid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CreateBinaryDoc");
	jimo_call->arg(AosFN::eDoc, docstr);
	jimo_call->arg(AosFN::eFlag, is_public);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, vid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

bool jimoDeleteBinaryDoc(
		AosRundata *rdata,
		const OmnString &objid)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteBinaryDocByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoDeleteBinaryDoc(
		AosRundata *rdata,
		const u64 &docid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteBinaryDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoModifyBinaryDocByStat(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const AosBuffPtr &buff,
		const u64 &normal_snap_id,
		const u64 &binary_snap_id)
{
	aos_assert_rr(binary_snap_id != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, binary_snap_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eModifyBinaryDocByStat,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	aos_assert_r(doc, false);
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue1, normal_snap_id);
	jimo_call->arg(AosFN::eValue2, binary_snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

AosXmlTagPtr jimoRetrieveBinaryDocByStat(
		AosRundata *rdata,
		const u64 &docid,
		AosBuffPtr &docBuff,
		const u64 &normal_snap_id,
		const u64 &binary_snap_id)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eRetrieveBinaryDocByStat,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eBuff, docBuff);
	jimo_call->arg(AosFN::eValue1, normal_snap_id);
	jimo_call->arg(AosFN::eValue2, binary_snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	docBuff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);
	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoRetrieveBinaryDoc(
		AosRundata *rdata,
		const OmnString &objid,
		AosBuffPtr &buff)
{
	/*
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eRetrieveBinaryDocByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	buff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
	*/
	aos_assert_r(objid != "", 0);                            
	                                                         
	u64 docid = jimoGetDocidByObjid(rdata, objid);               
	if (!docid) return 0;                                    
	AosXmlTagPtr doc = jimoRetrieveBinaryDoc(rdata, docid, buff);
	return doc;
}

AosXmlTagPtr jimoRetrieveBinaryDoc(
		AosRundata *rdata,
		const u64 &docid,
		AosBuffPtr &buff)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eRetrieveBinaryDocByDocid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "RetrieveBinaryDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	buff = jimo_call->getBuffRaw(rdata, AosFN::eBuff, 0);

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

void jimoRetrieveBinaryDoc(
		AosRundata *rdata,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &docid)
{
	OmnAlarm << enderr;
	return ;
}

bool jimoProcDocLock(
		AosRundata *rdata,
		const AosXmlTagPtr &req,
		const u64 &docid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eProcDocLock,
		distr_id, cluster, cluster);
	
	aos_assert_r(req, false);
	jimo_call->arg(AosFN::eMethod, "ProcDocLock");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, req->toString());
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoProcDocLock failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

void jimoProcNotifyMsg(
		AosRundata *rdata,
		const u64 &docid,
		const u32 siteid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	if(docid==0)
	{
		OmnAlarm << enderr;
		return;
	}
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	if(cluster==0)
	{
		OmnAlarm << enderr;
		return;
	}
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eProcNotifyMsg,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "ProcNotifyMsg");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoProcNotifyMsg failed, errmsg: " << errmsg << enderr;
		return;
	}

	return;
}

AosXmlTagPtr jimoGetParentContainer(
		AosRundata *rdata,
		const OmnString &container_objid,
		const bool ispublic)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(container_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, container_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetParentContainer,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetParentContainer");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue, container_objid);
	jimo_call->arg(AosFN::eFlag, ispublic);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetParentContainer failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

bool jimoGetTableNames(
		AosRundata *rdata,
		vector<string> &values)
{
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetTableNames, cluster);


	OmnString str_req = "";
	for(vector<string>::const_iterator itr=values.begin(); itr!=values.end(); itr++)
	{
		str_req << *itr;
		str_req << ";";
	}
	jimo_call->arg(AosFN::eValue, str_req);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	OmnString str_resp = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	AosStrSplit split(str_resp, ";");
	vector<OmnString> tmp_vec = split.entriesV();

	values.clear();
	for(vector<OmnString>::iterator itr = tmp_vec.begin(); itr != tmp_vec.end(), *itr!=""; itr++)
	{
		values.push_back((*itr).data());
	}


	return true;
}

bool jimoGetJQLDocs(
		AosRundata *rdata,
		const JQLTypes::DocType	&type,
		vector<string> &values)
{
	//Xuqi 2015/11/6
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetJQLDocs, 
		0, cluster, cluster);

	OmnString str_req = "";
	for(vector<string>::const_iterator itr=values.begin(); itr!=values.end(); itr++)
	{
		str_req << *itr;
		str_req << ";";
	}
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "GetJQLDocs");
	jimo_call->arg(AosFN::eType, type);
	jimo_call->arg(AosFN::eValue, str_req);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetParentContainer failed, errmsg: " << errmsg << enderr;
		return false;
	}

	OmnString str_resp = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	AosStrSplit split(str_resp, ";");
	vector<OmnString> tmp_vec = split.entriesV();

	if ( tmp_vec.size() == 0 )
	{
		return true;
	}

	values.clear();
	for(vector<OmnString>::iterator itr = tmp_vec.begin(); itr != tmp_vec.end(), *itr!=""; itr++)
	{
		values.push_back((*itr).data());
	}

	return true;
}

u64 jimoGetJQLDocidByTablename(
		AosRundata *rdata,
		const string &tablename)
{
	aos_assert_rr(tablename != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, tablename);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetJQLDocidByTablename,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eName, tablename);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return jimo_call->getU64(rdata, AosFN::eValue, 0);
}

bool jimoDropJQLTable(
		AosRundata *rdata,
		const OmnString &table_name)
{
	aos_assert_rr(table_name != "", rdata, false);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, table_name);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eDropJQLTable,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eName, table_name);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoCreateJQLTable(
		AosRundata *rdata,
		const OmnString &table_def)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(table_def!= "", rdata, false);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, table_def);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateJQLTable,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CreateJQLTable");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eName, table_def);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateJQLTable failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoCreateDocByJQL(
		AosRundata *rdata,
		const OmnString &container_objid,
		const OmnString &objid,
		const OmnString &doc)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDocByJQL,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eContainer, container_objid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDoc, doc);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoDeleteDocByDocid(
		AosRundata *rdata,
		const int64_t docid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, (u64)docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteDocByDocid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eDocid, (u64)docid);
	jimo_call->arg(AosFN::eMethod, "DeleteDocByDocid");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoDeleteDocByDocid failed, errmsg: " << errmsg << enderr;
	    return false;
	}

	return true;
}

bool jimoDeleteDocByObjid(
		AosRundata *rdata,
		const string &objid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, OmnString(objid));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteDocByObjid,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "DeleteDocByObjid");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eObjid, OmnString(objid));
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoDeleteDocByObjid failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoDeleteDoc(
		AosRundata *rdata,
		const AosXmlTagPtr &origdocroot)
{
	OmnAlarm << enderr;
	return true;
}

bool jimoDeleteDoc(
		AosRundata *rdata,
		const AosXmlTagPtr &origdocroot,
		const OmnString &ctnr_objid,
		const bool deleteFlag)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, OmnString(ctnr_objid));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eDeleteDoc,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	
	aos_assert_r(origdocroot, false);
	jimo_call->arg(AosFN::eMethod, "DeleteDoc");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eData, origdocroot->toString());
	jimo_call->arg(AosFN::eValue, ctnr_objid);
	jimo_call->arg(AosFN::eFlag, deleteFlag);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoDeleteDoc failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoGetDocByJQL(
		AosRundata *rdata,
		const string &objid,
		string &doc)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, OmnString(objid));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByJQL,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDocByJQL");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eObjid, OmnString(objid));
	jimo_call->arg(AosFN::eValue, OmnString(doc));
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetDocByJQL failed, errmsg" << errmsg <<enderr;
		return false;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	doc = string(str.data(), str.length());

	return true;
}

bool jimoGetParentContainerByObjid(
		AosRundata *rdata,
		const string &objid,
		string &doc)
{
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, OmnString(objid));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetParentContainerByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, OmnString(objid));
	jimo_call->arg(AosFN::eDoc, OmnString(doc));
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	doc = string(str.data(), str.length());

	return true;
}

bool jimoGetParentContainerByDocid(
		AosRundata *rdata,
		const int64_t docid,
		string &doc)
{
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, (u64)docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetParentContainerByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, (u64)docid);
	jimo_call->arg(AosFN::eDoc, OmnString(doc));
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	doc = string(str.data(), str.length());

	return true;
}

AosXmlTagPtr jimoGetDocByKey(
		AosRundata *rdata,
		const OmnString &container,
		const OmnString &key_name,
		const OmnString &key_value)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(key_value != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, key_value);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByKey1,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod,"GetDocByKey1");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue1, container);
	jimo_call->arg(AosFN::eValue2, key_name);
	jimo_call->arg(AosFN::eValue3, key_value);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetDocByKey failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");

	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoGetDocByKey(
		AosRundata *rdata,
		const OmnString &tablename)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(tablename != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, tablename);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByKey2,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDocByKey2");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eName, tablename);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoGetDocByKey failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

AosXmlTagPtr jimoGetDocByKeys(
		AosRundata *rdata,
		const OmnString &container,
		const vector<OmnString> &key_names,
		const vector<OmnString> &key_values)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(key_values[0] != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, key_values[0]);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByKeys,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "GetDocByKeys");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eContainer, container);
	OmnString str_names = "", str_values = "";
	for(vector<OmnString>::const_iterator itr1=key_names.begin(); itr1!=key_names.end(); itr1++)
	{
		str_names << *itr1;
		str_names << ";";
	}
	for(vector<OmnString>::const_iterator itr2=key_values.begin(); itr2!=key_values.end(); itr2++)
	{
		str_values << *itr2;
		str_values << ";";
	}
	jimo_call->arg(AosFN::eValue1, str_names);
	jimo_call->arg(AosFN::eValue2, str_values);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eValue, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

bool jimoMergeSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &target_snap_id,
		const u64 &merge_snap_id)
{
return true;
	
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(target_snap_id != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, target_snap_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eMergeSnapshot_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "MergeSnapshot");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, (int)doc_type);
	jimo_call->arg(AosFN::eValue1, target_snap_id);
	jimo_call->arg(AosFN::eValue2, merge_snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoMergeSnapshot failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

u64 jimoCreateSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const u64 snap_id,
		const AosDocType::E doc_type,
		const u64 &task_docid)
{
return 0;

	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	//aos_assert_rr(snap_id != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, snap_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateSnapshot_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CreateSnapshot");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eType, (int)doc_type);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateSnapshot failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eData, 0);
}

bool jimoCommitSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
return true;

	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	//aos_assert_rr(snap_id != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, snap_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCommitSnapshot_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CommitSnapshot");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, (int)doc_type);
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCommitSnapshot failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoRollbackSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid)
{
return true;

	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	//aos_assert_rr(snap_id != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, snap_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eRollbackSnapshot,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "RollbackSnapshot");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, (int)doc_type);
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoRollbackSnapshot failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

AosXmlTagPtr jimoCreateRootCtnr(
		AosRundata *rdata,
		const OmnString &docstr)
{
	//Xuqi 2015/11/3
	
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	
	aos_assert_rr(docstr != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docstr);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateRootCtnr,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "CreateRootCtnr");
	jimo_call->arg(AosFN::eValue, docstr);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateRootCtnr failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	bool flag = jimo_call->getBool(rdata, AosFN::eFlag, false);
	if(flag)
	{	
		return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	}
	else
	{
		return 0;
	}
}


AosXmlTagPtr jimoCreateDocByTemplate1(
		AosRundata *rdata,
		const OmnString &cid,
		const OmnString &objid,
		const OmnString &template_objid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCreateDocByTemplate1,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CreateDocByTemplate1");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eValue, template_objid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCreateDocByTemplate1 failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

bool jimoBindObjid(
		AosRundata *rdata,
		OmnString &objid,
		u64 &docid,
		bool &objid_changed,
		const bool resolve,
		const bool keepDocid)
{
	//Xuqi 2015/11/3
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eBindObjid_Doc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eMethod, "BindObjid");
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, objid_changed);
	jimo_call->arg(AosFN::eValue1, resolve);
	jimo_call->arg(AosFN::eValue2, keepDocid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoBindObjid failed, errmsg: " << errmsg << enderr;
		return false;
	}

	bool flag = jimo_call->getBool(rdata, AosFN::eFlag, false);
    return flag;
}

u64 jimoDoesObjidExist(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &objid)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eDoesObjidExist,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "DoesObjidExist");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoDoesObjidExist failed, errmsg: " << errmsg << enderr;
		return false;
	}

	return jimo_call->getU64(rdata, AosFN::eValue, 0);
}

AosXmlTagPtr jimoCloneDoc(
		AosRundata *rdata,
		const OmnString &cloudid,
		const OmnString &fromobjid,
		const OmnString &toobjid,
		const bool is_public)
{
	if (!jimoIsValidJPID(rdata->getJPID())) rdata->setJPID(AOSJPID_PUBLIC); 
	if (!jimoIsValidJSID(rdata->getJSID())) rdata->setJSID(AOSJSID_PUBLIC); 
	aos_assert_rr(cloudid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cloudid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoCallHandler, JimoDocFunc::eCloneDoc,
		distr_id, cluster, cluster);
	
	jimo_call->arg(AosFN::eMethod, "CloneDoc");
	jimo_call->arg(AosFN::eDocType, OmnString("Xml"));
	jimo_call->arg(AosFN::eCloudid, cloudid);
	jimo_call->arg(AosFN::eValue1, fromobjid);
	jimo_call->arg(AosFN::eValue2, toobjid);
	jimo_call->arg(AosFN::eFlag, is_public);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");
		OmnAlarm << "jimoCloneDoc failed, errmsg: " << errmsg << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

bool jimoReadLockDocByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const int waittimer,
		const int holdtimer)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eReadLockDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue1, waittimer);
	jimo_call->arg(AosFN::eValue2, holdtimer);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoReadUnlockDocByDocid(
		AosRundata *rdata,
		const u64 &docid)
{
	aos_assert_rr(docid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eReadUnlockDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}


AosXmlTagPtr jimoGetDocByName(
		AosRundata *rdata,
		const OmnString &container_objid,
		const OmnString &key_field_name,
		const OmnString &key_value,
		const OmnString &cluster_name)
{
	aos_assert_rr(container_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, container_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eGetDocByName,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue1, container_objid);
	jimo_call->arg(AosFN::eValue2, key_field_name);
	jimo_call->arg(AosFN::eValue3, key_value);
	jimo_call->arg(AosFN::eValue,  cluster_name);

	bool rslt = jimo_call->makeCall(rdata);
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return 0;
	}

	OmnString docstr = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	return AosXmlParser::parse(docstr AosMemoryCheckerArgs);
}


AosXmlTagPtr jimoGetAdminDocByName(
		AosRundata *rdata,
		const OmnString &container_objid,
		const OmnString &key_field_name,
		const OmnString &key_value)
{
	//AosClusterIDO *lsAdminCluster = jimoGetAdminEngineCluster(rdata);
	aos_assert_rr(container_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, container_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eReadUnlockDocByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eContainer, container_objid);
	jimo_call->arg(AosFN::eName, key_field_name);
	jimo_call->arg(AosFN::eValue, key_value);
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess());
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}

	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eDoc, "");
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}


bool jimoAddCreateLogRequest(
		AosRundata *rdata,
		const u64 &userid,
		const AosXmlTagPtr &doc)
{
	aos_assert_rr( userid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, userid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eAddCreateLogRequest,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	if(!doc)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess());
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}

	return true;
}


bool jimoAddDeleteLogRequest(
		AosRundata *rdata,
		const AosXmlTagPtr &doc,
		const u64 userid)
{
	aos_assert_rr( userid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, userid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoCallHandler, JimoDocFunc::eAddDeleteLogRequest,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	if(!doc)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eDoc, doc->toString());
	jimo_call->makeCall(rdata);

	if (!jimo_call->isCallSuccess());
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return 0;
	}

	return true;
}

};
