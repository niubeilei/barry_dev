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
//   DocStore API from DocPkg to Cube
//
// Modification History:
// 2015/03/31 Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoDocStore_h
#define Aos_JimoAPI_JimoDocStore_h

#include "DfmUtil/DfmConfig.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"
#include "Util/String.h"

namespace Jimo
{
//Gavin 2015/10/13
extern bool jimoStoreCreateSE(
						AosRundata *rdata, 
						const AosDfmConfig &config);

//Gavin 2015/09/07
extern bool jimoStoreCreateDatalets(
						AosRundata *rdata, 
						const u32 cubeid,
						const u64 aseid,	
						AosBuffPtr &buff);

extern bool jimoStoreCreateDatalet(
						AosRundata *rdata, 
						const u64 aseid,			// Gavin, 2015/09/01
						const u64 docid, 
						const AosBuffPtr buff,
						const u64 snap_id,
						AosBuffPtr custom_data = NULL);

extern bool jimoStoreCreateDataletSafe(
						AosRundata *rdata,
						const u64 aseid,
						const u64 docid,
		 				const AosBuffPtr &doc_buff,
						const bool save_flag,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr custom_data = NULL);

extern bool jimoStoreDeleteDataletByDocid(
						AosRundata *rdata, 
						const u64 aseid,
						const u64 docid, 
						const u64 snap_id,
						AosBuffPtr custom_data);

extern bool jimoStoreGetDataletByObjid(
						AosRundata *rdata, 
						const OmnString &objid, 
						const bool &need_binarydata,
						const u64 &snap_id,
						AosBuffPtr &resp);


extern bool jimoStoreBatchSaveStatDatalets(
						AosRundata *rdata,
						const u32 cube_id, 
						const AosXmlTagPtr &cube_conf,
						const AosBuffPtr &input_data,
						const AosBuffPtr &stat_mdf_info,
						bool &svr_death,
						AosBuffPtr &resp);

extern bool jimoStoreRetrieveBinaryDatalet(
						AosRundata *rdata,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &docid,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id);

extern bool jimoStoreGetDataletByDocid(
						AosRundata *rdata,
						const u64 aseid,    //Gavin 2015/09/02
						const u64 &docid,
						const bool need_binarydata,
						const u64 snap_id,
						AosBuffPtr &resp,
						AosBuffPtr &custom_data);

extern bool jimoStoreIsDataletDeleted(
						AosRundata *rdata,
						const u64 docid,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreModifyDatalet(
						AosRundata *rdata,
						const u64 &docid,
						const u64 &aseid,
		 				const AosBuffPtr &doc_buff,
						const bool need_save,
						const bool need_resp,
						const u64 &snap_id,
						AosBuffPtr custom_data = NULL);

extern bool jimoStoreWriteUnLockDatalet(
						AosRundata *rdata,
						const u64 docid,
						const u64 userid,
						const AosXmlTagPtr &newdoc,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreWriteLockDatalet(
						AosRundata *rdata,
						const u64 docid,
						const u64 userid,
						const int waittimer,
						const int holdtimer,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreReadUnLockDatalet(
						AosRundata *rdata,
						const u64 docid,
						const u64 userid,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreReadLockDatalet(
						AosRundata *rdata,
						const u64 docid,
						const u64 userid,
						const int waittimer,
						const int holdtimer,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreMergeSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 target_snap_id,
						const u64 merge_snap_id,
						const bool need_save,
						const bool need_resp,
						AosBuffPtr &resp);

extern bool jimoStoreCommitSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid,
						const bool need_save);

extern bool jimoStoreRollbackSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid,
						const bool need_save);

extern bool jimoStoreCreateSnapshot(
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &task_docid,
						AosBuffPtr &resp);

extern bool jimoStoreCheckDataletLock(
						AosRundata *rdata,
						const u64 docid,
						const OmnString &type,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreProcDataletLock(
						AosRundata *rdata,
						const u64 docid,
						const OmnString &lock_type,
						const u64 lock_timer,
						const u64 lockid,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id);

extern bool jimoStoreBatchGetDatalets(
						AosRundata *rdata,
						const AosAsyncRespCallerPtr &resp_caller,
						const int vid,
						const u32 num_docs,
						const u32 blocksize,
						const AosBuffPtr &docid_buff,
						const AosBuffPtr &len_buff,
						const u64 snap_id,
						const bool need_save,
						const bool need_resp);

extern bool jimoStoreGetCSVDatalet(
						AosRundata *rdata,
						const u64 &docid,
						const int record_len,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreGetFixedDatalet(
						AosRundata *rdata,
						const u64 &docid,
						const int record_len,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id,
						AosBuffPtr &resp);

extern bool jimoStoreAppendEntryToDocStore(
						AosRundata *rdata, 
						const u64 docid, 
						const AosBuff &buff);

extern bool jimoCommitSnapshot(                   
						AosRundata *rdata,
						const int virtual_id,
						const AosDocType::E doc_type,
						const u64 &snap_id,
						const u64 &task_docid,
						const bool need_save);

extern 	bool jimoProcDataletLock(
						AosRundata *rdata,
						const u64 docid,
						const OmnString &lock_type,
						const u64 lock_timer,
						const u64 lockid,
						const bool need_save,
						const bool need_resp,
						const u64 snap_id);

};
#endif
