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
// 	Created: 12/13/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClient_IILClient_h
#define AOS_IILClient_IILClient_h

#include "CounterUtil/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "SEUtil/IILName.h"
#include "IILUtil/IILFuncType.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Util/Opr.h"
#include "Util/Orders.h"
#include "UtilHash/Ptrs.h"
#include <deque>
#include <vector>

OmnDefineSingletonClass(AosIILClientSingleton,
						AosIILClient,
						AosIILClientSelf,
						OmnSingletonObjId::eIILClient,
						"IILClient");


class AosIILClient : public AosIILClientObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxIILPerCreation = 500,
		eMaxVirtualsForCid = 100,
	};

public:
	AosIILClient();
	~AosIILClient();

	// Singleton class interface
	static AosIILClient*	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &config);
	
private:
	vector<u64>		getAncestorPriv(
						const OmnString &ancestoriilname,
	 					const u64 &parent_docid,
	 					const AosRundataPtr &rdata);
public:
	bool			createIILPublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool			createIILPublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool			createTablePublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	bool			createTablePublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);
	
	bool			addHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);
	bool			addHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			addStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata);
	bool			addStrValueDoc(
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	bool			addStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	bool			addStrValueDocToTable(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);

	bool			addU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata);
	bool			addU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	bool			addU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	bool			addU64ValueDocToTable(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	bool			addU64ValueDocToTable(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
	
	bool			removeHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			removeHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);
	bool			removeStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			removeStrValueDoc(
						const u64 &iilid,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);
	bool			removeU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			removeU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			removeU64DocByValue(
						const u64 &iilid,
						const u64 &value,
						int &entriesRemoved,
						const AosRundataPtr &rdata);
	bool			removeU64DocByValue(
						const OmnString &iilname,
						const u64 &value,
						int &entriesRemoved,
						const AosRundataPtr &rdata);

	bool 			modifyStrValueDoc(
						const u64 &iilid,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);
	bool 			modifyStrValueDoc(
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);

	bool			modifyU64ValueDoc(
						const u64 &iilid,
						const u64 &oldvalue,
						const u64 &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool 			modifyU64ValueDoc(
						const OmnString &iilname,
						const u64 &oldvalue,
						const u64 &newvalue,
						const bool value_unique,
						const bool docid_unique,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			getDocid(
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
	bool			getDocid(
						const OmnString &iilname,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
	bool 			getDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &isunique,
						const AosRundataPtr &rdata);

	bool			getDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique,
						const AosRundataPtr &rdata);
	bool			getDocid(
						const OmnString &iilname,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique,
						const AosRundataPtr &rdata);

	u64 			getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata);
	bool			getDocidByObjid(
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
	bool			getDocidByCloudid(
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
	{
		aos_assert_rr(siteid != 0 && cid != "", rdata, false);
		aos_assert_rr(cid.length() > 0 && cid.length() < eAosMaxCloudidLength, rdata, false);
		docid = 0;
		OmnString iilname = AosIILName::composeCloudidListingName(siteid);
		return getDocid(iilname, cid, eAosOpr_eq, false, docid, isunique, rdata);
	}

	bool			addAncestor(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const u64 &parent_docid,
						const AosRundataPtr &rdata);
	bool			addDescendant(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &ancestoriilname,
						const u64 &docid,
						const u64 &parent_docid,
						const AosRundataPtr &rdata);
	bool			removeAncestor(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const u64 &parent_docid,
						const AosRundataPtr &rdata);
	bool			removeDescendant(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &ancestoriilname,
						const u64 &docid,
						const u64 &parent_docid,
						const AosRundataPtr &rdata);
	
	bool			bindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool			bindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool 			unbindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool 			unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool 			rebindCloudid(
						const OmnString &old_cloudid,
						const OmnString &new_cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata);
	bool 			rebindObjid(
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			incrementDocid(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
	bool			incrementDocidToTable(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
	bool			incrementDocid(
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool isPersis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
	bool			incrementDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool isPersis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue,
						const AosRundataPtr &rdata);
	bool			incrementDocid(
						const OmnString &iilname,
						const u64 &key,
						u64 &new_value,
						const bool isPersis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dft_value,
						const AosRundataPtr &rdata);

	u64				getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated,
						const AosRundataPtr &rdata);

	u64				getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated,
						const AosRundataPtr &rdata);

	bool 			querySafe(
						const OmnString &iilname,
						const OmnString &docscanner_id,
						const u64 &blocksize,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	bool			querySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosRundataPtr &rdata);
	bool			querySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	bool			querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata);
	bool			querySafe(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			preQuerySafe(
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	bool			preQuerySafe(
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			addInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);
	bool			removeInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);
	bool			modifyInlineSearchValue(
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata);
	bool			incrementInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue,
						const AosRundataPtr &rdata);

	bool			removeStrFirstValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata);

	bool			setStrValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);
	bool			setStrValueDocUniqueToTable(
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);

	bool			setU64ValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);
	bool			setU64ValueDocUniqueToTable(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata);
	bool			setU64ValueDocUnique(
						const OmnString &iilname,
						const u64 &key,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			appendManualOrder(
						const OmnString &iilname,
						const u64 &docid,
						u64 &value,
						const AosRundataPtr &rdata);
	bool			moveManualOrder(
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag,
						const AosRundataPtr &rdata);
	bool			swapManualOrder(
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2,
						const AosRundataPtr &rdata);
	bool			removeManualOrder(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			updateKeyedValue(
						const OmnString &iilname,
						const u64 &key,
						const u64 &value,
						const AosRundataPtr &rdata);
	bool 			updateKeyedValue(
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value,
						const AosRundataPtr &rdata);

	bool			queryValueSafe(
						const u64 &iilid,
						vector<OmnString> &values,
						const AosOpr opr,
						const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);
	bool			queryValueSafe(
						const OmnString &iilname,
						vector<OmnString> &values,
						const AosOpr opr,
						const OmnString &value,
						const bool unique_value,
						const AosRundataPtr &rdata);
	bool			queryU64ValueSafe(
						const u64 &iilid,
						vector<u64> &values,
						const AosOpr opr,
						const u64 &value,
						const bool unique_value,
						const AosRundataPtr &rdata);
	bool			queryU64ValueSafe(
						const OmnString &iilname,
						vector<u64> &values,
						const AosOpr opr,
						const u64 &value,
						const bool unique_value,
						const AosRundataPtr &rdata);

	bool			getDocids(
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size,
						const AosRundataPtr &rdata)
					{
						OmnNotImplementedYet;
						return false;
					}
	
	void 			printHitStat();

	bool			firstDocidSafe(
						const u64 &iilid,
						const bool reverse,
						u64 &value,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
	
	bool			firstDocidSafe(
						const u64 &iilid,
						const bool reverse,
						const OmnString &value,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);

	bool 			reCreateIILPublic(
						const OmnString &iilname,
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	u64				nextId1(
						const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue,
						const AosRundataPtr &rdata);


	bool			sendHitBuff(
						const AosBuffPtr &buff,
						const OmnString &iilname,
						const bool isPersis,
						const AosRundataPtr &rdata);

	bool			getSplitValue(
						const OmnString &iilname,
						const int &num_blocks,
						const AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata);

	virtual bool	getDocidsByKeys( 	
	 					const u64 &iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
			 			const AosRundataPtr &rdata);
	virtual bool 	getDocidsByKeys( 	
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata);

	bool			sendAttrBuff(
						const AosBuffPtr &buff,
						const AosIILFuncType::E iiltype,
						const OmnString &iilname,
						const bool isPersis,
						const AosRundataPtr &rdata);

	bool			queryData(
						const OmnString &iilname,
						const OmnString &value1,
						const AosOpr opr1,
						const OmnString &value2,
						const AosOpr opr2,
						const int order_by,
						const AosDataTablePtr &table,
						const AosRundataPtr &rdata);
	
	bool			deleteIIL(
						const u64 &iilid,
						const bool true_delete,
						const AosRundataPtr &rdata);
	bool			deleteIIL(
						const OmnString &iilname,
						const bool true_delete,
						const AosRundataPtr &rdata);

	virtual bool	getSplitValue(
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts,
						const AosRundataPtr &rdata);

	u64				getIILID(
						const OmnString &iilname,
						const AosRundataPtr &rdata);

	bool			counterRange(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);
	bool			counterRange(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);

	virtual bool	resetKeyedValue(
						const AosRundataPtr &rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue);

	virtual bool	StrBatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	virtual bool	StrBatchDel(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	virtual bool	StrBatchInc(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
	
	virtual bool	bitmapQuerySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	// Chen Ding, 2014/02/05
	virtual bool	bitmapQueryNewSafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	virtual bool	bitmapRsltQuerySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	virtual bool	bitmapQueryByPhysicalSafe(
						const int physical_id,
						vector<OmnString> &iilnames,
						vector<AosQueryRsltObjPtr> &bitmap_idlists,
						vector<AosBitmapObjPtr> &partial_bitmaps,
						const AosBitmapObjPtr &bitmap_rslt_phy,
						const AosRundataPtr &rdata);

	virtual bool	mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosRundataPtr &rdata);

	virtual u64		createSnapshot(
						const u32 virtual_id,
						const u64 &task_docid,
						const u64 &u64,
						const AosRundataPtr &rata);

	virtual bool	commitSnapshot(
						const u32 &virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	virtual bool	rollBackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	virtual	int64_t	getTotalNumDocs(
						const OmnString &iilname,
						const AosRundataPtr &rdata);		
	
	virtual	int64_t	getTotalNumDocs(
						const OmnString &iilname,
						const u64 &snap_id,
						const AosRundataPtr &rdata);		

	virtual bool	StrBatchAddMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete,
						const AosRundataPtr &rdata);

	virtual bool	StrBatchIncMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata);
	
	virtual bool	HitBatchAdd(
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	virtual bool	U64BatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	bool			getBatchDocids(
						const AosRundataPtr &rdata,
						const AosBuffPtr &column,
						const OmnString &iilname);
	bool			rebuildBitmap(
						const OmnString &iilname,
						const AosRundataPtr &rdata);


	// Add by Shawn 15/03/10

	bool 			addValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata);
								
	bool 			addValueDoc(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
				
	bool 			addValueDoc(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
				
	bool 			addValueDocToTable(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata);
						
	bool			 BatchAdd(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
						
	bool 			BatchDel(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);
				
	bool 			getDocid(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
				
	bool 			getDocid(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata);
				
	bool 			incrementDocid(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
				
	bool 			incrementDocid(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &key,
						u64 &value,
						const bool isPersis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
				
	bool			 incrementDocidToTable(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata);
				
	bool 			modifyValueDoc(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);
				
	bool 			modifyValueDoc(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata);
				
	bool 			removeValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata);
				
	bool 			removeValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const AosRundataPtr &rdata);
				
	bool 			removeValueDoc(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const AosRundataPtr &rdata);
				
	bool 			removeValueDoc(
						const u64 &iilid,
						const AosIILType &iiltype,
						const OmnString &the_value,
						const u64 &docid,
						const AosRundataPtr &rdata);
						
						




private:
	bool			addTransToVector(const AosTransPtr &trans,
						vector<AosTransPtr> *allTrans,
						const u32 arr_len);
	
	bool			addTrans(
						const AosTransPtr &trans,
						AosBuffPtr &resp,
						const AosRundataPtr &rdata);

	bool			addTrans(
						const AosTransPtr &trans,
						const AosRundataPtr &rdata);

	AosXmlTagPtr	getXmlFromBuff(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
	
	bool			querySafeNorm(
						const u64 &iilid,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	bool			querySafeNorm(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			querySafeBig(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);

	bool			counterRangeNorm(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);

	bool			counterRangeBig(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata);

	//felicia, 2013/08/05
	bool			querySafeNormAsync(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata);

	bool			querySafeBigAsync(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata);

	virtual bool	JimoTableBatchAdd(
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata);

	// Chen Ding, 2014/10/25
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
						const int page_size);

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
						const int page_size);

	virtual bool incrementKeyedValue(
					AosRundata *rdata,
					const OmnString &iilname,
					const OmnString &key,
					u64 &value,
					const bool persis_flag,
					const u64 &incValue,
					const u64 &initValue,
					const bool add_flag) {return false;}

	virtual bool getMapValues( 
					const OmnString &iilname,
					set<OmnString> &keys,
					vector<u64> &values,
					const AosRundataPtr &rdata);
};
#endif

