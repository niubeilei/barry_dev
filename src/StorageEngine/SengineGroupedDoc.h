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
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_SengineGroupedDoc_h
#define Aos_StorageEngine_SengineGroupedDoc_h

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "DocFileMgr/DfmHeader.h"
#include "DfmUtil/DfmGroupedDoc.h"
#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "StorageEngine/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Util/TransId.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"


class AosSengineGroupedDoc : public AosStorageEngineObj,
							  public OmnThreadedObj
{
	enum
	{
		eSizeIdHeaderSize = 1000000, // 1M
		eDocHeaderEntrySize = AosDfmGroupedDoc::eGroupDocsHeaderSize + AosDfmHeader::eHeaderMetaLen, 
		eMaxHeaderPerSizeId = eSizeIdHeaderSize / eDocHeaderEntrySize
	};

	OmnThreadPtr						mThread;
	OmnMutexPtr							mLock;
	OmnCondVarPtr						mCondVar;
	queue<AosSengineDocReqObjPtr>		mQueue;		
	AosRundataPtr						mRundata;
	bool								mFlush;

public:
	AosSengineGroupedDoc(const bool regflag);
	~AosSengineGroupedDoc();

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	virtual AosXmlTagPtr	readCommonDocFromFile(
								const u64 &docid,
								const int record_len,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosXmlTagPtr	readCSVDocFromFile(
								const u64 &docid,
								const int record_len,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosXmlTagPtr	readFixedDocFromFile(
								const u64 &docid,
								const int record_len,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosXmlTagPtr 	serializeToXmlDoc(
								const AosXmlTagPtr &xml,
								const AosDataRecordObjPtr rcd,
								const AosRundataPtr &rdata);

	virtual AosBuffPtr		readCommonDocsFromFile(
								const u64 &docid, 
								int &num,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosBuffPtr		readCSVDocsFromFile(
								const u64 &docid, 
								int &num,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosBuffPtr		readCommonDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size);

	virtual AosBuffPtr		readCSVDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size);

	virtual AosBuffPtr		readFixedDocsFromFile(
								const u64 &docid, 
								int &num,
								const AosDocFileMgrObjPtr &docfilemgr, 
								const AosRundataPtr &rdata);

	virtual AosBuffPtr		readFixedDocsFromFile(
								const AosRundataPtr &rdata,
								const vector<u64> &docids,
								const int record_size);

	virtual bool 			addReq(
								const AosSengineDocReqObjPtr &req,
								const AosRundataPtr &rdata);

	virtual AosDocFileMgrObjPtr retrieveDocFileMgrByKey(
								const u32   virtual_id,
								const OmnString dfm_key,
								const bool create_flag,
								const AosRundataPtr &rdata);

	virtual bool			insertCSVDoc(
								const u64 &snap_id,
								const u64 &sizeid,
								const int record_size,
								const AosBuffPtr &buff,
								const AosDocFileMgrObjPtr &docfilemgr,
								const AosTransId &trans_id,
								const AosRundataPtr &rdata);

	static  u64 getLocalId(const u64 &docid)
	{
		u32 vnum = (u32)AosGetNumCubes();
		aos_assert_r(vnum > 0, 0);
		u64 local_id = (AosGetLocalIdByDocid(docid)) / vnum;
		return local_id;
	}

	static bool getGroupid(
			const u64 &sizeid, 
			const u64 &local_docid, 
			const u32 max_docs_per_group,
			u64 &groupid)
	{
		aos_assert_r(max_docs_per_group > 0, false);
		groupid = 10 + local_docid / max_docs_per_group;
		return true;
	}

	static bool AosAssembleGroupDoc(
								AosRundata* rdata,
								const u64 docid,
								const u64 schema_docid,
								const int len,
								const char * const doc,
								const AosBuffPtr &doc_buff);

	virtual void flushContents();

private:
	bool 	saveGroupDocsCommon(
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &trans_id,
			const u64 &snap_id,
			const u32 &groupid,
			const AosBuffPtr &doc_buff,
			const bool need_sort,
			const AosRundataPtr &rdata);

	bool 	saveGroupDocsCSV(
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &trans_id,
			const u64 &snap_id,
			const u32 &groupid,
			const AosBuffPtr &doc_buff,
			const bool need_sort,
			const AosRundataPtr &rdata);

	bool 	saveGroupDocsFixed(
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &trans_id,
			const u64 &snap_id,
			const u32 &groupid,
			const AosBuffPtr &doc_buff,
			const AosRundataPtr &rdata);

	bool	readGroupDocsCommon(
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const u32 &groupid,
			const u64 &max_docs_per_group,
			AosBuffPtr &doc_buff,
			const AosRundataPtr &rdata);

	bool	readGroupDocsCSV(
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const u32 &groupid,
			const u64 &max_docs_per_group,
			AosBuffPtr &doc_buff,
			const AosRundataPtr &rdata);

	bool	readGroupDocsFixed(
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const u32 &groupid,
			AosBuffPtr &doc_buff,
			const AosRundataPtr &rdata);

	bool	deleteGroupDocs(
			const u32 &headerid,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	bool	readRecordCommon(
			const u64 &sizeid,
			const u64 &local_id,
			char **data,
			int &len,
			u64 &schema_docid,
			const int record_size,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	bool	readRecordCSV(
			const u64 &sizeid,
			const u64 &local_id,
			char **data,
			int &len,
			const int record_size,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	bool	readRecordFixed(
			const u64 &sizeid,
			const u64 &local_id,
			char *data,
			const int record_size,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	bool	readRecordsCommon(
			const u64 &sizeid,
			const u64 &local_id,
			const int &record_size,
			const AosBuffPtr &buff,
			const u32 max,
			int &read_num_docs,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	bool	readRecordsCSV(
			const u64 &sizeid,
			const u64 &local_id,
			const int &record_size,
			const AosBuffPtr &buff,
			const u32 max,
			int &read_num_docs,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	bool	readRecordsFixed(
			const u64 &sizeid,
			const u64 &local_id,
			const int &record_size,
			const AosBuffPtr &buff,
			const u32 max,
			int &read_num_docs,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosRundataPtr &rdata);

	
	bool getMaxDocsPerGroup(const u32 record_size, u64 &max_docs_per_group)
	{
		max_docs_per_group = eMaxDocPerGroup;
		return true;
		/*
		aos_assert_r(record_size > 0, false);
		u64 max_group_size = AosStorageEngineObj::getGroupedDocGroupSize();
		aos_assert_r(max_group_size > 0, false);
		u32 reserved_size = AosDocFileMgrObj::getReservedSize();
		max_docs_per_group = (max_group_size - reserved_size) /record_size;
		aos_assert_r(max_docs_per_group > 0, false);
		return true;
		*/
	}

	bool	sanityCheck(
			const u32 &offset,
			const char *data,
			const int &len,
			const int &record_len);

	bool 	batchInsertCSVDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	bool 	batchInsertCommonDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	bool 	batchUpdateCommonDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	bool 	batchDeleteCSVDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	bool 	batchUpdateCSVDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	bool 	saveBatchFixedDocPriv(
			const AosSengineDocReqObjPtr& req,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &transid,
			const AosRundataPtr &rdata);

	AosDfmDocPtr readDocPriv(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 &snap_id,
			const u64 &groupid,
			const bool read_body);

	bool	saveDocPriv(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &docfilemgr,
			const AosTransId &trans_id,
			const u64 &snap_id,
			const AosDfmDocPtr &p_doc);

	void	checkFullDocs(
			const u32 len,
			const char *cmp_data,
			const char *data_array,
			const int &record_size,
			bool &is_full,
			const AosRundataPtr &rdata);

	bool	batchReadFixedDocs(
			const int record_size, 
			const u32 start_idx, 
			const u32 num_docs, 
			const vector<u64> &docids, 
			const u64 crt_groupid,
			AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

	bool	batchReadCommonDocs(
			const int record_size, 
			const u32 start_idx, 
			const u32 num_docs, 
			const vector<u64> &docids, 
			const u64 crt_groupid,
			AosBuffPtr &buff, 
			const AosRundataPtr &rdata);


	bool	batchReadCSVDocs(
			const int record_size, 
			const u32 start_idx, 
			const u32 num_docs, 
			const vector<u64> &docids, 
			const u64 crt_groupid,
			AosBuffPtr &buff, 
			const AosRundataPtr &rdata);
};
#endif

