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
// 2014/11/24 Created by Chen Ding
// 2015/03/14 Copied from Jimo.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoIndexEngine_h
#define Aos_JimoAPI_JimoIndexEngine_h


#include "IILUtil/IILUtil.h"
#include "CounterUtil/CounterQuery.h"
#include "Rundata/Rundata.h"
#include "Util/Opr.h"
#include "Util/String.h"

namespace Jimo
{
extern AosXmlTagPtr getXmlFromBuff(
					const AosBuffPtr &buff,    
					const AosRundataPtr &rdata);

extern bool	jimoAddStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id);

extern bool jimoAddStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &the_value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);   

extern bool	jimoAddStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);

extern bool jimoAddStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool ispersis);

extern bool jimoAddU64ValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool ispersis);

extern bool jimoAddU64ValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool ispersis);

extern bool jimoAddU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id);

extern bool jimoAddHitDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid);

extern bool jimoAddHitDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id);

extern bool jimoAddInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType);

extern bool jimoAppendManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid,
						u64 &value);

extern bool jimoBindCloudid(
						AosRundata *rdata,
						const OmnString &cid,
						const u64 &docid);

extern bool	jimoBindObjid(
						AosRundata *rdata,
						const OmnString &objid, 
						const u64 &docid);

extern bool jimoBitmapQueryNewSafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);

extern bool jimoBitmapRsltQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);

extern bool	jimoCounterRange(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query);

extern bool jimoCounterRange(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query);

extern bool	jimoCommitSnapshot(
						AosRundata *rdata,
						const u32 &_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern u64 jimoCreateSnapshot(
						AosRundata *rdata,
						const u32 _id,
						const u64 &snap_id,
						const u64 &task_docid);
extern bool jimoCreateTablePublic(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype);

extern bool jimoCreateTablePublic(
						AosRundata *rdata,
						u64 &iilid,
						const AosIILType iiltype);

extern bool jimoCreateIILPublic(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype);

extern bool jimoCreateIILPublic(
						AosRundata *rdata,
						u64 &iilid,
						const AosIILType iiltype);

extern bool jimoDeleteIIL(
						AosRundata *rdata,
						const u64 &iilid,
						const bool true_delete);

extern bool jimoDeleteIIL(
						AosRundata *rdata,
						const OmnString &iilname,
						const bool true_delete);

extern bool jimoGetBatchDocids(
						AosRundata *rdata,
						const AosBuffPtr &column,
						const OmnString &iilname);

extern u64 jimoGetCtnrMemDocid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated);

extern u64 jimoGetCtnrMemDocid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated);

extern bool jimoGetDocid(
						const OmnString &iilname,
						const OmnString &the_value,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

extern bool jimoGetDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &the_value,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique);

extern bool jimoGetDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique);

extern bool jimoGetDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique);

extern inline bool  jimoGetDocid(
                    AosRundata *rdata,
                    const OmnString &iilname,
                    const OmnString &key,
                    u64 &docid)
{
    bool isunique = false;                     
    return jimoGetDocid( iilname, key, 
			eAosOpr_eq, false, docid, isunique, rdata); 
}

extern inline bool  jimoGetDocid(
                    AosRundata *rdata,
                    const OmnString &iilname,
                    const OmnString &key,
                    u64 &docid,
                    bool &isunique)
{
    return jimoGetDocid(rdata, iilname, key, 
			eAosOpr_eq, false, docid, isunique); 
}

extern bool jimoGetDocids(
						AosRundata *rdata,
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size);

extern bool jimoGetDocidByCloudid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique);

extern u64	jimoGetDocidByObjid(
						AosRundata *rdata,
						const OmnString &objid);
	 
extern bool jimoGetDocidByObjid(
						AosRundata *rdata,
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique);

extern bool jimoGetDocidsByKeys(
						AosRundata *rdata,
						const u64 &iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff);

extern bool jimoGetDocidsByKeys(
						AosRundata *rdata,
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff);

extern	u64 jimoGetIILID(
						AosRundata *rdata,
						const OmnString &iilname);

extern bool jimoGetSplitValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts);

extern int64_t jimoGetTotalNumDocs(
						AosRundata *rdata,
						const OmnString &iilname);

extern bool jimoHitBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoIncrementDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);

extern bool jimoIncrementDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						AosRundata *rdata);

extern bool jimoIncrementDocid(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue);

extern bool jimoIncrementDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dft_value);

extern bool jimoIncrementDocidToTable(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);

extern bool jimoIncrementInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue);

extern bool jimoMergeSnapshot(
						AosRundata *rdata,
						const u32 _id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id);

extern bool jimoMoveManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag);

extern bool jimoModifyInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType);

extern bool jimoModifyStrValueDoc(                   
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

extern bool jimoModifyStrValueDoc(                   
						const OmnString &iilid,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

extern bool jimoModifyU64ValueDoc(                   
						const OmnString &iilname,
						const u64 &oldvalue,
						const u64 &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

extern bool jimoModifyU64ValueDoc(                   
						const u64 &iilid,
						const u64 &oldvalue,
						const u64 &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

extern u64 jimoNextId1( AosRundata *rdata,
						const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue);

extern bool jimoPreQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context);

extern bool jimoPreQuerySafe(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context);

extern bool jimoQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);

extern bool jimoQuerySafe(
						AosRundata *rdata,
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context);

extern bool jimoQuerySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId);

extern bool jimoRemoveManualOrder(
				   AosRundata *rdata,
				   const OmnString &iilname,
				   const u64 &value,
				   const u64 &docid);

extern bool jimoRemoveStrValueDoc(
				  const OmnString &iilname,
				  const OmnString &the_value,
				  const u64 &docid,
				  const AosRundataPtr &rdata);

extern bool jimoRemoveStrValueDoc(
				  const OmnString &iilid,
				  const OmnString &the_value,
				  const u64 &docid,
				  const AosRundataPtr &rdata);

extern bool jimoRemoveStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid);

extern bool jimoRemoveStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid);

extern bool jimoRemoveStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id);

extern bool jimoRemoveStrValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid);

extern bool jimoRemoveU64ValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid);

extern bool jimoRemoveU64ValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid);

extern bool jimoRemoveU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id);

extern bool jimoRemoveStrFirstValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse);

extern bool jimoRemoveInlineSearchValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType);

extern bool jimoResetKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue);

extern bool	jimoRollBackSnapshot(
						AosRundata *rdata,
						const u32 _id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoStrBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoBatchAdd(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						AosRundata *rdata);

extern bool jimoStrBatchAddMerge(
						AosRundata *rdata,
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete);

extern bool jimoStrBatchDel(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoBatchDel(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						AosRundata *rdata);


extern bool jimoStrBatchInc(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoStrBatchIncMerge(
						AosRundata *rdata,
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete);

extern bool jimoSetStrValueDocUniqueToTable(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same);

extern bool jimoSetStrValueDocUnique(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same);

extern bool	jimoSetU64ValueDocUnique(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same);

extern bool jimoJimoTableBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool	jimoSetU64ValueDocUniqueToTable(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same);

extern bool jimoSwapManualOrder(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2);

extern bool jimoTableBatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoU64BatchAdd(
						AosRundata *rdata,
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid);

extern bool jimoUpdateKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value);

extern bool jimoRebindCloudid(
						AosRundata *rdata,
						const OmnString &old_cid,
						const OmnString &new_cid,
						const u64 &docid);

extern bool	jimoRebindObjid(
						AosRundata *rdata,
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid);

extern bool jimoRemoveHitDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &docid);

extern bool jimoRemoveHitDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid);

extern bool jimoRebuildBitmap(
						AosRundata *rdata,
						const OmnString &iilname);

extern bool jimoUnbindObjid(
						AosRundata *rdata,
						const OmnString &objid, 
						const u64 &docid);

extern bool jimoUnbindCloudid(
						AosRundata *rdata,
						const OmnString &cid,
						const u64 &docid);
/*


	 bool		incrementKeyedValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis_flag,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag);
	 bool		getDocid(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique);
	 bool		removeU64ValueDoc(
						AosRundata *rdata,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid);
	 bool		removeU64DocByValue(
						AosRundata *rdata,
						const u64 &iilid,
						const u64 &value,
						int &entriesRemoved);
	 bool		removeU64DocByValue(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &value,
						int &entriesRemoved);
	 bool		modifyStrValueDoc(
						AosRundata *rdata,
						const u64 &iilid,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override);
	 bool		modifyStrValueDoc(
						AosRundata *rdata,
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override);
	 bool		setU64ValueDocUnique(
						AosRundata *rdata,
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same);
	 int64_t		getTotalNumDocs(
						AosRundata *rdata,
						const OmnString &iilname,
						const u64 &snap_id);
	 bool		querySafe(
						AosRundata *rdata,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosValueRslt &start_value,
						const AosValueRslt &end_value,
						AosValueRslt &next_value,
						u64 &next_docid,
						const AosOpr opr,
						const bool reverse,
						const i64 start_pos,
						const int page_size);
	 bool		querySafe(
						AosRundata *rdata,
						const u64 iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosValueRslt &start_value,
						const AosValueRslt &end_value,
						AosValueRslt &next_value,
						u64 &next_docid,
						const AosOpr opr,
						const bool reverse,
						const i64 start_pos,
						const int page_size);

	 AosXmlTagPtr getDocByName(
						AosRundata *rdata, 
						const OmnString &container_objid, 
						const OmnString &key_field_name,
						const OmnString &key_value);

	 AosXmlTagPtr getAdminDocByName(
						AosRundata *rdata, 
						const OmnString &container_objid, 
						const OmnString &key_field_name,
						const OmnString &key_value);
*/

};

#endif

