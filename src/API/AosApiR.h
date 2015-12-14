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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiR_h
#define AOS_API_ApiR_h

#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/CounterCltObj.h"
#include "SEInterfaces/ExprObj.h"
#include "SEInterfaces/Ptrs.h"

#include "JQLStatement/Ptrs.h"

#include "TransServer/Ptrs.h"

#include "DfmUtil/DfmConfig.h"
#include "Rundata/Rundata.h"
#include "Util/DiskStat.h"
#include "Util/String.h"
#include "HDFS/hdfs.h"

extern AosDocClientObjPtr g__AosDocClient;

extern AosXmlTagPtr AosRetrieveDocByObjid(
			const OmnString &objid, 
			const bool force_retrieve,
			const AosRundataPtr &rdata);

extern AosXmlTagPtr AosRetrieveDocByDocid(
			const u64 &docid, 
			const bool force_retrieve,
			const AosRundataPtr &rdata);

extern AosXmlTagPtr AosRetrieveDocByCloudId(
			const OmnString &cid,
			const bool force_retrieve,
			AosRundata *rdata);

extern AosIILObjPtr AosRetrieveIIL(
			const OmnString &iilname,
			const bool read_only,
			const int timer,
			AosRundata *rdata);

extern AosIILObjPtr AosRetrieveIIL(
			const u64 &iilid,
			const bool read_only,
			const int timer,
			AosRundata *rdata);

extern bool AosReturnIIL(const AosIILObjPtr &iil);

inline bool AosRetrieveSingleCounter(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveSingleCounter(counter_id, cname, stat_types, start_time, 
			end_time, dft_value, buff, rdata);
}

inline bool AosRetrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveMultiCounters(counter_id, cnames, dft_value, buff, rdata);
}


inline bool AosRetrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveMultiCounters(counter_id, cnames, start_time,
			end_time, dft_value, buff, rdata);
}


inline bool AosRetrieveCounters(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveCounters(counter_id, cname, stat_types, 
			buff, rdata);
}

inline bool AosRetrieveCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveCounters(counter_id, cnames, stat_types, buff, rdata);
}


inline bool AosRetrieveCounters(
			const vector<OmnString> &counter_ids,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveCounters(counter_ids, cname, stat_types, 
			buff, use_iilname, rdata);
}

inline bool AosRetrieveCounters(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveCounters(counter_ids, cnames, stat_types, buff, use_iilname, rdata);
}


inline bool AosRetrieveSubCountersBySingle(
		const vector<OmnString> &counter_ids,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const OmnString &member,
		AosBuffPtr &buff,
		bool use_iilname,
		const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->retrieveSubCountersBySingle(counter_ids, cnames, stat_types, member, buff, use_iilname, rdata);
}

inline bool AosReleaseWriteLock(const AosXmlTagPtr &doc, AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	return g__AosDocClient->releaseWriteLock(doc, rdata);
}


inline bool AosReleaseWriteLockDocByObjid(const OmnString &objid, AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	return g__AosDocClient->releaseWriteLockDocByObjid(objid, rdata);
}


inline bool AosReleaseWriteLockDocByDocid(const u64 &docid, AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	return g__AosDocClient->releaseWriteLockDocByDocid(docid, rdata);
}


extern OmnString AosRetrieveServerInfo(
			const AosXmlTagPtr &doc, 
			AosRundata *rdata);

extern bool AosRemoveKeyedU64Value(
			const u64 &iilid, 
			const u64 &key, 
			AosRundata *rdata);

extern bool AosRemoveDataRecordBySizeId(
			const u32 siteid, 
			const u64 &sizeid, 
			const bool flag, 
			const AosRundataPtr &rdata);

extern AosXmlTagPtr AosRetrieveBinaryDoc(
			const OmnString &objid, 
			AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

extern AosXmlTagPtr AosRetrieveBinaryDoc(
			const u64 &docid, 
			AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

extern AosXmlTagPtr AosRetrieveBinaryDoc(
			const OmnString &objid, 
			AosBuffPtr &buff, 
			u64	&startpos,
			u64	&needlen,
			u64	&readsize,
			const AosRundataPtr &rdata);

extern AosDocFileMgrObjPtr AosRetrieveDocFileMgr(
		AosRundata *rdata,
		const u32   virtual_id,
		const AosDfmConfig &config);

// Chen Ding, 2013/02/08
//extern void AosReturnBitmap(const AosBitmapObjPtr &bitmap);
extern bool AosResetKeyedValue(
			const AosRundataPtr &rdata,
			const OmnString &iilname,
			const OmnString &key,
			u64 &value,
			const bool persis, 
			const u64 &incValue);

// Chen Ding, 2013/03/03
extern AosBitmapTreeObjPtr AosRetrieveBitmapTree(
			AosRundata *rdata, 
			const u64 &tree_id);

extern bool AosRunQuery(
			AosRundata *rdata,
			const int startidx, 
			const int psize,
	 		const AosXmlTagPtr &query,
			AosQueryRsltObjPtr &query_rslt);

// Chen Ding, 2013/05/30
extern bool AosRunSmartDoc(
		const AosRundataPtr &rdata, 
		const OmnString &objid);

extern bool AosRunSmartDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc);

extern bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc);

extern bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const OmnString &jimo_objid); 

extern bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc);

extern AosXmlTagPtr AosRunDocSelector(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc);

extern AosXmlTagPtr AosRunDocSelector(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const OmnString &tagname);

extern bool AosRunAction(
		AosRundata *rdata, 
		const OmnString &action_id,
		const AosXmlTagPtr &worker_doc);

// Chen Ding, 2013/10/28
extern bool AosRunAction(
		AosRundata *rdata, 
		const OmnString &act_def);
extern bool AosReadDataFile(
		AosRundata *rdata, 
		AosBuffPtr &buff,
		const int phy_id,
		const OmnString &fname, 
		const int64_t start_pos,
		const int64_t bytes_to_read, 
		AosDiskStat &disk_stat);

// Chen Ding, 2013/12/15
extern AosXmlTagPtr AosGetReferencedDoc(
		AosRundata *rdata, 
		const AosXmlTagPtr &tag);

extern AosXmlTagPtr AosResolveReference(
		AosRundata *rdata, 
		const AosXmlTagPtr &tag);

extern bool AosRunProcsSync(
		AosRundata *rdata, 
		const vector<OmnThrdShellProcPtr> runners);

extern bool AosRunProcsAsync(
		AosRundata *rdata, 
		const vector<OmnThrdShellProcPtr> runners);

extern bool AosRemoveIndexEntry(
		AosRundata *rdata,
		const OmnString &table_name,
		const OmnString &field_name);

extern bool AosRemoveStrValueDoc(
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata);

#endif

