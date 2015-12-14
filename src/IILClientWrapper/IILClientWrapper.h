////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/09/15 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILClientWrapper_IILClientWrapper_h
#define Aos_IILClientWrapper_IILClientWrapper_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILClientObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "IILClientWrapper/Ptrs.h"


OmnDefineSingletonClass(AosIILClientWrapperSingleton,              
						AosIILClientWrapper,
						AosIILClientWrapperSelf,
						OmnSingletonObjId::eIILClient,
						"IILClientWrapper");


class AosIILClientWrapper : public AosIILClientObj
{

	OmnDefineRCObject;

private:
	AosXmlTagPtr    getXmlFromBuff(                 
			const AosBuffPtr &buff,     
			const AosRundataPtr &rdata);


public:
	AosIILClientWrapper();
	~AosIILClientWrapper();

	static AosIILClientWrapper* getSelf();
	virtual bool            start();                            
	virtual bool            stop();
	virtual bool            config(const AosXmlTagPtr &config);


	bool incrementDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);

	bool incrementDocid(
						const OmnString &iilname,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dft_value,
						const AosRundataPtr &rdata);

	bool incrementDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);

	bool incrementDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue,
						const AosRundataPtr &rdata);

	bool	addU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata);

	bool	addStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata);

	bool	removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);

	bool 	modifyStrValueDoc(            
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

	bool	removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);

	bool	addU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool	addU64ValueDocToTable(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool	addStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool bitmapQueryNewSafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool	bitmapRsltQuerySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool	 BatchAdd(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool 	BatchDel(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool createTablePublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);
	
	bool createIILPublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool createTablePublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool createIILPublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool querySafe(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool setU64ValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);

	bool setU64ValueDocUniqueToTable(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);

	bool setStrValueDocUniqueToTable(
	 					u64 &iilid,
	 					const bool createFlag,
	 					const OmnString &key,
	 					const u64 &docid,
	 					const bool must_same,
	 					const AosRundataPtr &rdata);

	bool deleteIIL(
						const u64 &iilid,
						const bool true_delete,
						const AosRundataPtr &rdata);

	u64	getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata);

	bool getDocid(
						const OmnString &iilname,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	bool getDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique,
						const AosRundataPtr &rdata);

	bool getDocid(
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	bool getDocidsByKeys(
						const u64 &iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	bool getDocidsByKeys(
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	bool getSplitValue(
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts,
						const AosRundataPtr &rdata);

	bool rebuildBitmap(
						const OmnString &iilname,
						const AosRundataPtr &rdata);

	bool querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata);

	bool removeU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool resetKeyedValue(
						const AosRundataPtr &rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue);

	bool removeStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool setStrValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);

	bool StrBatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool StrBatchDel(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool StrBatchInc(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool U64BatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	//Actions
	bool HitBatchAdd(
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	
	u64	createSnapshot(
						const u32 virtual_id,
						const u64 &task_docid,
						const u64 &snap_id,
						const AosRundataPtr &rata);

	bool removeHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool StrBatchAddMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete,
						const AosRundataPtr &rdata);

	bool StrBatchIncMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata);
	
	bool JimoTableBatchAdd(
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	u64	getIILID(
						const OmnString &iilname,
						const AosRundataPtr &rdata);

	bool counterRange(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);

	bool counterRange(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);

	bool commitSnapshot(
						const u32 &virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool rollBackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	//IdGen
	u64 nextId1( const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue,
						const AosRundataPtr &rdata);

	//Job
	bool mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosRundataPtr &rdata);

	//JQLStatement
	bool deleteIIL(
						const OmnString &iilname,
						const bool true_delete,
						const AosRundataPtr &rdata);
	
	//Microblog
	u64	getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated,
						const AosRundataPtr &rdata);

	//Query
	bool preQuerySafe(
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool preQuerySafe(
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool getDocidByObjid(
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	//SeLogClient
	bool addU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	
	bool removeU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	//SeLogSvr
	bool addStrValueDocToTable(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool removeStrFirstValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata);
	
	bool addU64ValueDocToTable(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
		
	//SEModules
	u64	getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated,
						const AosRundataPtr &rdata);

	//SeReqProc
	bool appendManualOrder(
						const OmnString &iilname,
						const u64 &docid,
						u64 &value,
						const AosRundataPtr &rdata);

	bool incrementInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue,
						const AosRundataPtr &rdata);

	bool moveManualOrder(
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag,
						const AosRundataPtr &rdata);
	
	bool removeManualOrder(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool swapManualOrder(
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2,
						const AosRundataPtr &rdata);

	//SmartDoc
	bool getBatchDocids(
						const AosRundataPtr &rdata,
						const AosBuffPtr &column,
						const OmnString &iilname);
	
	bool getDocids(
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size,
						const AosRundataPtr &rdata);

	bool updateKeyedValue(
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value,
						const AosRundataPtr &rdata);

	bool addInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);

	bool modifyInlineSearchValue(
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);

	bool removeInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);
	
	//StatUtil
	bool incrementDocidToTable(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);

	//DocClient
	bool bindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool bindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool unbindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool rebindCloudid(
						const OmnString &old_cloudid,
						const OmnString &new_cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool rebindObjid(
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool getDocidByCloudid(
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	bool addHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);

	bool removeHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata);

	virtual bool getMapValues( 
					const OmnString &iilname,
					set<OmnString> &keys,
					vector<u64> &values,
					const AosRundataPtr &rdata) { return false; }
};

#endif

