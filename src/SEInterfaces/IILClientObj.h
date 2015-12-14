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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_IILClientObj_h
#define Aos_SEInterfaces_IILClientObj_h

#include "CounterUtil/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "TransUtil/Ptrs.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/ValueRslt.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosIILClientObj : virtual public OmnRCObject
{
private:
	static AosIILClientObjPtr smIILClient;

public:
	


	static AosIILClientObjPtr getIILClient() {return smIILClient;}
	static void setIILClient(const AosIILClientObjPtr &d) {smIILClient = d;}

	virtual bool incrementDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata) = 0;

	virtual bool incrementDocid(
						const OmnString &iilname,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dft_value,
						const AosRundataPtr &rdata) = 0;

	virtual bool incrementDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata) = 0;

	virtual bool incrementDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue,
						const AosRundataPtr &rdata) = 0;

	virtual bool	addU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool	addStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool	removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool	removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata) = 0;

	virtual bool	addU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;

	virtual bool	addU64ValueDocToTable(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;

	virtual bool	addStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;

	virtual bool bitmapQueryNewSafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool	bitmapRsltQuerySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool	 BatchAdd(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool 	BatchDel(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool createTablePublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool createIILPublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata) = 0;

	virtual bool createTablePublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata) = 0;

	virtual bool createIILPublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata) = 0;

	virtual	bool querySafe(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool setU64ValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) = 0;

	virtual bool setU64ValueDocUniqueToTable(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) = 0;

	virtual bool setStrValueDocUniqueToTable(
	 					u64 &iilid,
	 					const bool createFlag,
	 					const OmnString &key,
	 					const u64 &docid,
	 					const bool must_same,
	 					const AosRundataPtr &rdata) = 0;

	virtual bool deleteIIL(
						const u64 &iilid,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;

	virtual u64	getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata) = 0;

	virtual bool getDocid(
						const OmnString &iilname,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata) = 0;

	virtual bool getDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique,
						const AosRundataPtr &rdata) = 0;

	inline bool	getDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
	{
		return getDocid(iilid, key, eAosOpr_eq,
			false, docid, isunique, rdata);
	}

	virtual bool getDocid(
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata) = 0;
	inline bool	getDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique,
						const AosRundataPtr &rdata)
	{
		bool rslt = getDocid(iilid, key, eAosOpr_eq,
			false, docid, isunique, rdata);
		if (!rslt)
		{
			found = false;
			docid = 0;
			return false;
		}

		found = true;
		return true;
	}

	virtual bool getDocidsByKeys(
						const u64 &iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;

	virtual bool getDocidsByKeys(
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata) = 0;

	virtual bool getSplitValue(
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts,
						const AosRundataPtr &rdata) = 0;

	virtual bool rebuildBitmap(
						const OmnString &iilname,
						const AosRundataPtr &rdata) = 0;

	virtual bool querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata) = 0;

	virtual bool removeU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool resetKeyedValue(
						const AosRundataPtr &rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue) = 0;

	virtual bool removeStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool setStrValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) = 0;

	virtual bool StrBatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool StrBatchDel(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool StrBatchInc(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool U64BatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	//Actions
	virtual bool HitBatchAdd(
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;
	
	virtual u64	createSnapshot(
						const u32 virtual_id,
						const u64 &task_docid,
						const u64 &snap_id,
						const AosRundataPtr &rata) = 0;

	virtual bool removeHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool StrBatchAddMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;

	virtual bool StrBatchIncMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool JimoTableBatchAdd(
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;




	//CounterClt
	virtual u64	getIILID(
						const OmnString &iilname,
						const AosRundataPtr &rdata) = 0;

	bool	getDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &docid,
						const AosRundataPtr &rdata)
	{
	 	bool isunique = false;
		return getDocid(iilname, key, eAosOpr_eq,
			false, docid, isunique, rdata);	
	}

	virtual bool counterRange(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata) = 0;

	virtual bool counterRange(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata) = 0;

	//DbTrans (not used)
	virtual bool commitSnapshot(
						const u32 &virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool rollBackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) = 0;

	//IdGen
	virtual u64 nextId1( const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue,
						const AosRundataPtr &rdata) = 0;

	//Job
	virtual bool mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosRundataPtr &rdata) = 0;

	//JQLStatement
	virtual bool deleteIIL(
						const OmnString &iilname,
						const bool true_delete,
						const AosRundataPtr &rdata) = 0;
	
	//Microblog
	virtual u64	getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated,
						const AosRundataPtr &rdata) = 0;

	//Query
	virtual bool preQuerySafe(
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool preQuerySafe(
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) = 0;

	virtual bool getDocidByObjid(
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata) = 0;

	//SeLogClient
	virtual bool addU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool removeU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	//SeLogSvr
	virtual bool addStrValueDocToTable(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;

	virtual bool removeStrFirstValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool addU64ValueDocToTable(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;
		
	//SEModules
	virtual u64	getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated,
						const AosRundataPtr &rdata) = 0;

	//SeReqProc
	virtual bool appendManualOrder(
						const OmnString &iilname,
						const u64 &docid,
						u64 &value,
						const AosRundataPtr &rdata) = 0;

	virtual bool incrementInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue,
						const AosRundataPtr &rdata) = 0;

	virtual bool moveManualOrder(
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool removeManualOrder(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool swapManualOrder(
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2,
						const AosRundataPtr &rdata) = 0;

	//SmartDoc
	virtual bool getBatchDocids(
						const AosRundataPtr &rdata,
						const AosBuffPtr &column,
						const OmnString &iilname) = 0;
	
	virtual bool getDocids(
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size,
						const AosRundataPtr &rdata) = 0;

	virtual bool updateKeyedValue(
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value,
						const AosRundataPtr &rdata) = 0;

	virtual bool addInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata) = 0;

	virtual bool modifyInlineSearchValue(
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata) = 0;

	virtual bool removeInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata) = 0;
	
	//StatUtil
	virtual bool incrementDocidToTable(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata) = 0;

	//DocClient
	virtual bool bindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool bindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool unbindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool rebindCloudid(
						const OmnString &old_cloudid,
						const OmnString &new_cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool rebindObjid(
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	virtual bool getDocidByCloudid(
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata) = 0;

	inline bool	removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		int physical_id;
		return removeStrValueDoc(allTrans, arr_len,
			iilname, value, docid, physical_id, rdata);
	}

	inline bool	addStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata)
	{
		int physical_id;
		return addStrValueDoc(allTrans, arr_len, iilname, value, docid,
			value_unique, docid_unique, physical_id, rdata);
	}

	virtual bool addHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata) = 0;
	inline bool	addHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		int physical_id;
		return addHitDoc(allTrans, arr_len,
			iilname, docid, physical_id, rdata);
	}

	virtual bool removeHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;

	inline bool		removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		int physical_id;
		return removeU64ValueDoc(allTrans, arr_len,
			iilname, value, docid, physical_id, rdata);
	}

	inline bool	addU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata)
	{
		int physical_id;
		return addU64ValueDoc(allTrans, arr_len, iilname, value, docid,
			value_unique, docid_unique, physical_id, rdata);
	}


	//VersionServer
	inline bool	getDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
	{
		return getDocid(iilname, key, eAosOpr_eq,
			false, docid, isunique, rdata);	
	}

	virtual bool getMapValues( 
					const OmnString &iilname,
					set<OmnString> &keys,
					vector<u64> &values,
					const AosRundataPtr &rdata) = 0;

	/*
	
	virtual bool	addHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;


	virtual bool	addStrValueDoc(
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) = 0;
	
	virtual bool	removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeStrValueDoc(
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeU64DocByValue(
						const u64 &iilid,
						const u64 &value,
						int &entriesRemoved,
						const AosRundataPtr &rdata) = 0;
	virtual bool	removeU64DocByValue(
						const OmnString &iilname,
						const u64 &value,
						int &entriesRemoved,
						const AosRundataPtr &rdata) = 0;

	virtual bool	modifyStrValueDoc(
						const u64 &iilid,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata) = 0;
	virtual bool	modifyStrValueDoc(
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata) = 0;
	virtual bool	modifyStrValueDoc(
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const u64 &docid,
						const AosRundataPtr &rdata)
	{
		return modifyStrValueDoc(iilname, oldvalue, newvalue,
			value_unique, docid_unique, docid, false, rdata);
	}

	

			virtual bool	getDocid(
						const OmnString &iilname,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique,
						const AosRundataPtr &rdata) = 0;

						
			
	virtual	int64_t	getTotalNumDocs(
						const OmnString &iilname,
						const AosRundataPtr &rdata) = 0;		

	virtual	int64_t	getTotalNumDocs(
						const OmnString &iilname,
						const u64 &snap_id,
						const AosRundataPtr &rdata) = 0;		

	virtual bool querySafe(
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
						const int page_size) = 0;

	// Chen Ding, 2014/10/25
	virtual bool querySafe(
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
						const int page_size) = 0;
	
	virtual bool incrementKeyedValue(
					AosRundata *rdata,
					const OmnString &iilname,
					const OmnString &key,
					u64 &value,
					const bool persis_flag,
					const u64 &incValue,
					const u64 &initValue,
					const bool add_flag) = 0;

	virtual bool 			addValueDoc(
			vector<AosTransPtr> *allTrans,
			const u32 arr_len,
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			int &physical_id,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			addValueDoc(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			addValueDoc(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			addValueDocToTable(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			const AosRundataPtr &rdata) = 0;

		virtual bool 			getDocid(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const AosOpr opr,
			const bool reverse,
			u64 &docid,
			bool &isunique,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			getDocid(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const AosOpr opr,
			const bool reverse,
			u64 &docid,
			bool &isunique,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			incrementDocid(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_key,
			u64 &value,
			const u64 &incValue,
			const u64 &initValue,
			const bool add_flag,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			incrementDocid(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &key,
			u64 &value,
			const bool isPersis,
			const u64 &incValue,
			const u64 &initValue,
			const bool add_flag,
			const AosRundataPtr &rdata) = 0;

	virtual bool			 incrementDocidToTable(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_key,
			u64 &value,
			const u64 &incValue,
			const u64 &initValue,
			const bool add_flag,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			modifyValueDoc(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &oldvalue,
			const OmnString &newvalue,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			const bool override,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			modifyValueDoc(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &oldvalue,
			const OmnString &newvalue,
			const u64 &docid,
			const bool value_unique,
			const bool docid_unique,
			const bool override,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			removeValueDoc(
			vector<AosTransPtr> *allTrans,
			const u32 arr_len,
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			int &physical_id,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			removeValueDoc(
			vector<AosTransPtr> *allTrans,
			const u32 arr_len,
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			removeValueDoc(
			const OmnString &iilname,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const AosRundataPtr &rdata) = 0;

	virtual bool 			removeValueDoc(
			const u64 &iilid,
			const AosIILType &iiltype,
			const OmnString &the_value,
			const u64 &docid,
			const AosRundataPtr &rdata) = 0;
*/



};

#endif

