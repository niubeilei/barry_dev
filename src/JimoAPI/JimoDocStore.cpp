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
// 2015/03/31 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoDocStore.h"

#include "JimoAPI/JimoPackage.h"
#include "JimoAPI/JimoDocFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCallMgr.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallAsync.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallSyncNorm.h"
#include "JimoCall/JimoCallSyncWrite.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/StrSplit.h"
#include "UtilData/ModuleId.h"
#include "SEInterfaces/ClusterObj.h"
#include <vector>
#include "SEInterfaces/DocFileMgrObj.h"
#include "API/AosApiG.h"

static OmnString sgJimoDocStorePackage_Delete = "AosDeleteDataletProc";
static OmnString sgJimoDocStorePackage_Get = "AosGetDataletProc";
static OmnString sgJimoDocStorePackage_Modify = "AosModifyDataletProc";
static OmnString sgJimoDocStorePackage_Create = "AosCreateDataletProc";
//static u64 sgOprID = 10000;
//static OmnMutex sgLock;

namespace Jimo
{
//Gavin 2015/10/13
extern bool jimoStoreCreateSE(
						AosRundata *rdata, 
						const AosDfmConfig &config)
{
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	//u32 distr_id = cluster->getDistrID(rdata, docid);
	u32 distr_id = 0;

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,         
			        "AosDocStoreJimoCallHandler", JimoDocFunc::eCreateSE,
					        distr_id, cluster, cluster);                              

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = config.serializeTo(buff);	
	aos_assert_r(rslt, false);

	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eMethod, "CreateSE");
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
    rslt = jimo_call->isCallSuccess();
	if( !rslt )                                 
	{   
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << " jimoStoreCreateSE failed ! errmsg: " << errmsg << enderr;
		return false;                           
	}                                           
	                                            
	return true;                                
}


//Gavin 2015/09/07
bool jimoStoreCreateDatalets(
		AosRundata *rdata,
		const u32 cubeid,
		const u64 aseid,			
		AosBuffPtr &buff)
{
		
	buff->reset();
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);

	int crtidx = 0;
	int crtlen = 0;
	while (crtlen < buff->dataLen())
	{
		u32 crtEntryLen = buff->getU32(0);

		// the struct of entry
		// u64      docid
		// u32      status, Opr
		// int64    header_data_len
		// char *   header
		// int64    compress_len
		// int64    orig_len
		// int64    body_data_len        
		// char *   body
		AosBuffPtr entry = buff->getBuff(crtEntryLen, true AosMemoryCheckerArgs);
		u64 docid = entry->getU64(0) * AosGetNumCubes() + cubeid;

		crtlen += sizeof(u32) + crtEntryLen; 

		crtidx = entry->getCrtIdx();
		entry->setCrtIdx(crtidx + sizeof(u32));
		int64_t header_data_len = entry->getI64(0);
		AosBuffPtr custom_data = entry->getBuff(header_data_len, true AosMemoryCheckerArgs);

		crtidx = entry->getCrtIdx();
		entry->setCrtIdx(crtidx + sizeof(i64)*2);
		int64_t body_data_len = entry->getI64(0);

		AosBuffPtr body = entry->getBuff(body_data_len, true AosMemoryCheckerArgs);

		u32 distr_id = cluster->getDistrID(rdata, docid);
		u64 snap_id = 0;

		AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
				"AosDocStoreJimoCallHandler", JimoDocFunc::eCreateDatalet,
				distr_id, cluster, cluster);
		if (custom_data.isNull())
		{
			if (aseid == AosModuleId::eIIL)
			{
				OmnAlarm << " IIL need custom_data ! " << enderr; 
			}
		}

		jimo_call->arg(AosFN::eAseID, aseid);
		jimo_call->arg(AosFN::eDocid, docid);
		jimo_call->arg(AosFN::eMethod, "CreateDoc");
		jimo_call->arg(AosFN::eSnapID, snap_id);
		jimo_call->arg(AosFN::eHeaderCustomData, custom_data);
		jimo_call->arg(AosFN::eDoc, body);
		
		jimo_call->makeCall(rdata); 
		jimo_call->waitForFinish();

		bool rslt = jimo_call->isCallSuccess();
		if( !rslt ) 
		{
			OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
			OmnAlarm << " jimoStoreCreateDatalets failed ! errmsg: " << errmsg << enderr;
			return false;
		}   
	}

	return true;
}   

bool jimoStoreCreateDatalet(
		AosRundata *rdata,
		const u64 aseid,			// Gavin, 2015/09/01
		const u64 docid,
		const AosBuffPtr buff,
		const u64 snap_id,
		AosBuffPtr custom_data)
{
	aos_assert_rr(docid != 0, rdata, false);                                
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eCreateDatalet,
			distr_id, cluster, cluster);
	if (custom_data.isNull())
	{
		if (aseid == AosModuleId::eIIL)
		{
			OmnAlarm << " IIL needs custom_data ! " << enderr;
		}
	}
	aos_assert_r(buff, false);
	jimo_call->arg(AosFN::eAseID, aseid);
	jimo_call->arg(AosFN::eMethod, "CreateDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	if (custom_data.notNull())
	{
		jimo_call->arg(AosFN::eHeaderCustomData, custom_data);
	}
	//AosBuffPtr buff = OmnNew AosBuff(const_cast<char*>(newdoc), length + 10, length, true AosMemoryCheckerArgs);
																	//MUST set copy flag to true!
	jimo_call->arg(AosFN::eDoc, buff);
	jimo_call->makeCall(rdata); 
	jimo_call->waitForFinish();
	bool rslt = jimo_call->isCallSuccess();
	if( !rslt ) 
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreCreateDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}   

	return true;
} 

bool jimoStoreCreateDataletSafe(AosRundata *rdata,
		const u64 aseid,
		const u64 docid,
		const AosBuffPtr &doc_buff,
		const bool save_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		 AosBuffPtr custom_data)
{
    //xuqi 2015/10/21
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eCreateDatalet,
			distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eMethod, "CreateDoc");
	jimo_call->arg(AosFN::eAseID, aseid);
	if (custom_data.notNull())
	{
		jimo_call->arg(AosFN::eHeaderCustomData, custom_data);
	}
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->arg(AosFN::eDoc, doc_buff);
	jimo_call->arg(AosFN::eFlag, save_flag);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
	return jimo_call->isCallSuccess();
}

bool jimoStoreDeleteDataletByDocid(
		AosRundata *rdata, 
		const u64 aseid,
		const u64 docid, 
		const u64 snap_id,
		AosBuffPtr custom_data)
{
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eDeleteDatalet,
			distr_id, cluster, cluster);
	if (custom_data.isNull())                                 
	{                                                         
		if (aseid == AosModuleId::eIIL)            
		{                                                     
			OmnAlarm << " IIL need custom_data ! " << enderr; 
		}                                                     
	}                                                         

	jimo_call->arg(AosFN::eAseID, aseid);
	jimo_call->arg(AosFN::eMethod, "DeleteDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->arg(AosFN::eHeaderCustomData, custom_data);
	jimo_call->makeCall(rdata);                                                 
	jimo_call->waitForFinish();
	return jimo_call->isCallSuccess();
}


bool jimoStoreBatchGetDatalets(                           
		AosRundata *rdata,
		const AosAsyncRespCallerPtr &resp_caller,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &len_buff,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp)
{
	OmnNotImplementedYet;
	return true;
}

bool jimoStoreGetCSVDatalet(
		AosRundata *rdata,
		const u64 &docid,
		const int record_len,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eGetCSVDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eLength, record_len);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreGetCSVDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreWriteLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const int waittimer,
		const int holdtimer,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eWriteLockDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eData, waittimer);
	jimo_call->arg(AosFN::eValue, holdtimer);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreWriteLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}


bool jimoStoreGetFixedDatalet(                                           
		AosRundata *rdata,
		const u64 &docid,
		const int record_len,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eGetFixedDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eLength, record_len);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreGetFixedDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}




bool jimoStoreWriteUnLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const AosXmlTagPtr &newdoc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eWriteUnLockDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	aos_assert_r(newdoc, false);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eDoc, newdoc->toString());
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreWriteUnLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreGetDataletByObjid(
		AosRundata *rdata, 
		const OmnString &objid, 
		const bool &need_binarydata,
		const u64 &snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(objid!= "", rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eGetDataletByObjid,
			distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eFlag, need_binarydata);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();
	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreGetDataletByObjid failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}


bool jimoStoreMergeSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 target_snap_id,
		const u64 merge_snap_id,
		const bool need_save,
		const bool need_resp,
		AosBuffPtr &resp)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eMergeSnapshot,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, target_snap_id);
	jimo_call->arg(AosFN::eValue2, merge_snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreMergeSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreModifyDatalet(                
		AosRundata *rdata,
		const u64 &docid,
		const u64 &aseid,
		const AosBuffPtr &doc_buff,
		const bool need_save,
		const bool need_resp,
		const u64 &snap_id,
		AosBuffPtr custom_data)
{
	//Xuqi 2015/11/9
	aos_assert_rr(docid != 0, rdata, false);                                
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eModifyDatalet,
			distr_id, cluster, cluster);

	aos_assert_r(doc_buff, false);
	//hardcode for now
	jimo_call->arg(AosFN::eMethod, "ModifyDoc");
	jimo_call->arg(AosFN::eAseID, aseid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->arg(AosFN::eDoc, doc_buff);
	if (custom_data.notNull())
	{
		jimo_call->arg(AosFN::eHeaderCustomData, custom_data);
	}
	jimo_call->makeCall(rdata); 
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt ) 
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreModifyDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}   

	return true;
}


bool jimoStoreCreateSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &task_docid,
		AosBuffPtr &resp)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eCreateSnapshot,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue2, task_docid);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreCreateSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreCheckDataletLock(
		AosRundata *rdata,
		const u64 docid,
		const OmnString &type,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eCheckDataletLock,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eType, type);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreCheckDataletLock failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreIsDataletDeleted(
		AosRundata *rdata,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eGetDataletByDocid,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocType, "xml");
	jimo_call->arg(AosFN::eMethod, "GetDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, need_save);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreIsDataletDeleted failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}


bool jimoStoreProcDataletLock(
		AosRundata *rdata,
		const u64 docid,
		const OmnString &lock_type,
		const u64 lock_timer,
		const u64 lockid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eProcDataletLock,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eType, lock_type);
	jimo_call->arg(AosFN::eValue1, lock_timer);
	jimo_call->arg(AosFN::eValue2, lockid);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreProcDataletLock failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoStoreReadLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const int waittimer,
		const int holdtimer,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eReadLockDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eData, waittimer);
	jimo_call->arg(AosFN::eValue, holdtimer);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreReadLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreCommitSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eCommitSnapshot,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->arg(AosFN::eFlag, need_save);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreCommitSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoStoreReadUnLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eReadUnLockDatalet,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreReadUnLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoStoreRollbackSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Modify, JimoDocFunc::eRollbackSnapshotOnCube,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->arg(AosFN::eFlag, need_save);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreRollbackSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}


bool jimoStoreRetrieveBinaryDatalet(                 
		AosRundata *rdata,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{//eRetrieveBinaryDatalet
	OmnNotImplementedYet;
	return false;
}


bool jimoCreateDatalet(
		AosRundata *rdata,
		const u64 docid,
		const AosXmlTagPtr &newdoc,
		const u64 snap_id)
{
	// This function should be called by DocEngine. It sends a request to 
	// DocStore to actually create a doc. 
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			sgJimoDocStorePackage_Create, JimoDocFunc::eCreateDatalet,
			distr_id, cluster, cluster);

	// sgLock.lock();
	// jimo_call->arg(AosFN::eOprID, sgOprID++);
	// sgLock.unlock();
	aos_assert_r(newdoc, false);
	// jimo_call->arg(AosFN::eDocType, (int)AosDocType::eXmlDoc);
	jimo_call->arg(AosFN::eDoc, newdoc->toString());
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoCreateDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoCreateDataletSafe(AosRundata *rdata,
		const u64 docid,
		const AosXmlTagPtr &newdoc,
		const bool save_flag,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoDocStorePackage_Create, JimoDocFunc::eCreateDataletSafe,
		distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->arg(AosFN::eDoc, newdoc->toString());
	jimo_call->arg(AosFN::eFlag, save_flag);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	return jimo_call->isCallSuccess();
}

bool jimoDeleteDataletByDocid(
		AosRundata *rdata, 
		const u64 docid, 
		const u64 snap_id)
{
	aos_assert_rr(docid != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoDocStorePackage_Delete, JimoDocFunc::eDeleteDatalet,
		distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	return jimo_call->isCallSuccess();
}


bool jimoGetDataletByObjid(
		AosRundata *rdata, 
		const OmnString &objid, 
		const bool &need_binarydata,
		const u64 &snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(objid!= "", rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Get, JimoDocFunc::eGetDataletByObjid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eFlag, need_binarydata);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoGetDataletByObjid failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}

bool jimoWriteAccessDatalet(
		AosRundata *rdata, 
		const u64 docid,
		const AosXmlTagPtr &arcd,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoDocStorePackage_Create, JimoDocFunc::eWriteAccessDatalet,
		distr_id, cluster, cluster);

	aos_assert_r(arcd, false);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eDoc, arcd->toString());
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoWriteAccessDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoBatchSaveStatDatalets(
		AosRundata *rdata,
		const u32 cube_id, 
		const AosXmlTagPtr &cube_conf,
		const AosBuffPtr &input_data,
		//AosStatModifyInfo statMdfInfo,
		const AosBuffPtr &stat_mdf_info,
		bool &svr_death,
		AosBuffPtr &resp)
{
	aos_assert_rr(cube_id!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, cube_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
		sgJimoDocStorePackage_Create, JimoDocFunc::eBatchSaveStatDatalets,
		distr_id, cluster, cluster);

	aos_assert_r(cube_conf, false);
	jimo_call->arg(AosFN::eValue, cube_conf->toString());
	jimo_call->arg(AosFN::eBuff, input_data);

	jimo_call->arg(AosFN::eData, stat_mdf_info);

	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoBatchSaveStatDatalets failed ! errmsg: " << errmsg << enderr;
		return false;
	}
	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}

bool jimoRetrieveBinaryDatalet(
		AosRundata *rdata,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{//eRetrieveBinaryDatalet
	OmnNotImplementedYet;
	return false;
}

#if 0
bool jimoStoreGetDataletByDocid(
		AosRundata *rdata,
		const u64 &docid,
		const bool need_binarydata,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoDocStorePackage_Get, JimoDocFunc::eGetDataletByDocid,
			distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, need_binarydata);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnAlarm << " call failed ! " << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}
#endif

bool jimoStoreGetDataletByDocid(
		AosRundata *rdata,
		const u64 aseid, 				// Gavin 2015/09/02
		const u64 &docid,
		const bool need_binarydata,
		const u64 snap_id,
		AosBuffPtr &resp,
		AosBuffPtr &custom_data)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			"AosDocStoreJimoCallHandler", JimoDocFunc::eGetDataletByDocid,
			distr_id, cluster, cluster);

	jimo_call->arg(AosFN::eAseID, aseid);
	jimo_call->arg(AosFN::eMethod, "GetDoc");
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, need_binarydata);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreGetDataletByDocid failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	//resp = jimo_call->getBuff(rdata, AosFN::eDoc);
	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	custom_data = jimo_call->getBuff(rdata, AosFN::eHeaderCustomData);
	return true;
}


bool jimoStoreBatchSaveStatDatalets(
		AosRundata *rdata,
		const u32 cube_id, 
		const AosXmlTagPtr &cube_conf,
		const AosBuffPtr &input_data,
		//AosStatModifyInfo statMdfInfo,
		const AosBuffPtr &stat_mdf_info,                                             
		bool &svr_death, 
		AosBuffPtr &resp)
{       
	aos_assert_rr(cube_id!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreClusterAsync(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, cube_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallAsync(rdata,
			sgJimoDocStorePackage_Create, JimoDocFunc::eBatchSaveStatDatalets,
			distr_id, cluster, cluster);

	aos_assert_r(cube_conf, false);
	jimo_call->arg(AosFN::eValue, cube_conf->toString());
	jimo_call->arg(AosFN::eBuff, input_data);

	jimo_call->arg(AosFN::eData, stat_mdf_info);

	jimo_call->makeCall(rdata);
	jimo_call->waitForFinish();

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt ) 
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoStoreBatchSaveStatDatalets failed ! errmsg: " << errmsg << enderr;
		return false;
	}   
	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}   


bool jimoIsDataletDeleted(
		AosRundata *rdata,
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Get, JimoDocFunc::eGetDataletByDocid,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, need_save);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoIsDataletDeleted failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);

	return true;
}

bool jimoModifyDatalet(
		AosRundata *rdata,
		const u64 docid,
		const AosXmlTagPtr &new_doc,
		const AosXmlTagPtr &orig_doc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eModifyDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eFlag, need_save);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoModifyDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoWriteUnLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const AosXmlTagPtr &newdoc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eWriteUnLockDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	aos_assert_r(newdoc, false);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eDoc, newdoc->toString());
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoWriteUnLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoWriteLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const int waittimer,
		const int holdtimer,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eWriteLockDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eData, waittimer);
	jimo_call->arg(AosFN::eValue, holdtimer);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoWriteLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoReadUnLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eReadUnLockDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoReadUnLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoReadLockDatalet(
		AosRundata *rdata,
		const u64 docid,
		const u64 userid,
		const int waittimer,
		const int holdtimer,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eReadLockDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eUserid, userid);
	jimo_call->arg(AosFN::eData, waittimer);
	jimo_call->arg(AosFN::eValue, holdtimer);
	jimo_call->arg(AosFN::eValue1, need_save);
	jimo_call->arg(AosFN::eValue2, need_resp);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoReadLockDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}


bool jimoMergeSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 target_snap_id,
		const u64 merge_snap_id,
		const bool need_save,
		const bool need_resp,
		AosBuffPtr &resp)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eMergeSnapshot,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, target_snap_id);
	jimo_call->arg(AosFN::eValue2, merge_snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoMergeSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoCommitSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eCommitSnapshot,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->arg(AosFN::eFlag, need_save);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoCommitSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoRollbackSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eRollbackSnapshotOnCube,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->arg(AosFN::eFlag, need_save);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoRollbackSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}


bool jimoCreatebackSnapshot(
		AosRundata *rdata,
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &task_docid,
		const u64 &snap_id,
		const bool need_save,
		const bool need_resp,
		AosBuffPtr &resp)
{
	aos_assert_rr((u32)doc_type != 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, u32(doc_type));

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eCreatebackSnapshot,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eValue, virtual_id);
	jimo_call->arg(AosFN::eType, u32(doc_type));
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoCreatebackSnapshot failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}


bool jimoCheckDataletLock(
		AosRundata *rdata,
		const u64 docid,
		const OmnString &type,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eCheckDataletLock,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eType, type);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoCheckDataletLock failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}


bool jimoProcDataletLock(
		AosRundata *rdata,
		const u64 docid,
		const OmnString &lock_type,
		const u64 lock_timer,
		const u64 lockid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eProcDataletLock,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eType, lock_type);
	jimo_call->arg(AosFN::eValue1, lock_timer);
	jimo_call->arg(AosFN::eValue2, lockid);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoProcDataletLock failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}

bool jimoBatchGetDatalets(
		AosRundata *rdata,
		const AosAsyncRespCallerPtr &resp_caller,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &len_buff,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp)
{
	OmnNotImplementedYet;
	return true;
}

bool jimoGetCSVDatalet(
		AosRundata *rdata,
		const u64 &docid,
		const int record_len,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eGetCSVDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eLength, record_len);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoGetCSVDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

bool jimoGetFixedDatalet(
		AosRundata *rdata,
		const u64 &docid,
		const int record_len,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id,
		AosBuffPtr &resp)
{
	aos_assert_rr(docid!= 0, rdata, false);
	AosClusterObj *cluster = jimoGetDocStoreCluster(rdata);
	aos_assert_rr(cluster, rdata, false);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoDocStorePackage_Modify, JimoDocFunc::eGetFixedDatalet,
		distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eLength, record_len);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if( !rslt )
	{
		OmnString errmsg = jimo_call->getOmnStr(rdata, AosFN::eErrmsg, "");        
		OmnAlarm << "jimoGetFixedDatalet failed ! errmsg: " << errmsg << enderr;
		return false;
	}

	resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	return true;
}

/*
bool jimoAppendEntryToDocStore(
		AosRundata *rdata, 
		const u64 docid, 
		const AosBuff &buff, 
		AosDocFileMgrNewObj *docfilemgr)
{
	// This function calls Raft to append entry. 
}
*/
};
