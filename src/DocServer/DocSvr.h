////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_DocSvr_h
#define AOS_DocServer_DocSvr_h

#include "Rundata/Ptrs.h"
#include "DfmUtil/DfmConfig.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "DocServer/Ptrs.h"
#include "DocServer/ReadWriteLock.h"
#include "DocServer/DocBatchReaderMgr.h"
#include "DocServer/DocBatchReaderReq.h"
#include "QueryRslt/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/DocSvrObj.h"
#include "Thread/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransProc.h"
#include "TransUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>

class AosDocFileMgrNewObj;
using namespace std;


OmnDefineSingletonClass(AosDocSvrSingleton,
						AosDocSvr,
						AosDocSvrSelf,
						OmnSingletonObjId::eDocSvr,
						"DocSvr");

class AosDocSvr : public AosDocSvrObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxFiles = 2000,
		eMaxSize = 1000000000,   // 1G
	
		eError = 0,		// Ketty 2012/10/24
		eOk,
	};

private:
	OmnMutexPtr				mLock;
	AosReadWriteLockPtr		mReadWriteLock;
	bool					mIsCaChe;
	bool					mIsGroupedDocWithComp;
	AosDfmConfig *			mDfmConfigs;	
	u32						mMaxRepositories;
	// DfmEntry*				mDfms;
	u32						mNumRepositories;

public:
	AosDocSvr();
	~AosDocSvr();

    // Singleton class interface
    //
    static AosDocSvr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);
	
	//virtual bool	startTrans(const AosRundataPtr &rdata);
	//virtual bool	commitTrans(const AosRundataPtr &rdata);
	//virtual bool	rollbackTrans(const AosRundataPtr &rdata);

	// Young, 2014/10/27
	// smallDocs interface for log
	bool	addSmallDocs(
				const u64 &docid, 
				const AosBuffPtr &buff, 
				const AosTransId &trans_id,
				const AosRundataPtr &rdata);

	AosBuffPtr getSmallDocs(
				const AosRundataPtr &rdata,
				const u64 &docid);

	bool	deleteSmallDocs(
				const AosRundataPtr &rdata, 
				const u64 &docid,
				const AosTransId &trans_id);
	///////////////////////////////////////////


	bool	createDocSafe(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &doc,
				const bool &saveDocFlag,
				const AosTransId &trans_id,
				const u64 snap_id);

	bool	createDoc(	
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &newdoc, 
				const AosTransId &trans_id,
				const u64 snap_id);

	bool	createDoc( 
				const AosRundataPtr &rdata,
				const u64 &docid,
				const char *doc,
				const int64_t &doc_len,
				const AosTransId &trans_id,
				const u64 &snap_id);

	bool	deleteObj(
				const AosRundataPtr &rdata,
				const u64 &docid,
				const AosTransId &trans_id,
				const u64 snap_id);

	bool	modifyObj(
				const AosRundataPtr &rdata,
 				const AosXmlTagPtr &newxml,
				const u64 &newdid,
				const AosTransId &trans_id,
				const u64 snap_id);

	bool 	saveToFile(
				const u64 &docid,
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata,
				const AosTransId &trans_id,
				const u64 snap_id);

	//felicia peng, 2014/10/10
	bool	readLockDoc(
				const u64 &docid,	
				const u64 &userid,
				const int waittimer,
				const int holdtimer,
				const AosTransPtr &trans,
				int &status,
				AosRundata *rdata);
	
	bool	writeLockDoc(
				const u64 &docid,	
				const u64 &userid,
				const int waittimer,
				const int holdtimer,
				const AosTransPtr &trans,
				int &status,
				AosRundata *rdata);

	bool		readUnLockDoc(
				const u64 &docid,
				const u64 &userid,
				AosRundata *rdata);

	bool 		writeUnLockDoc(
				const u64 &docid,
				const u64 &userid,
				const AosXmlTagPtr &newdoc,
				const AosTransId &trans_id,
				AosRundata *rdata);

	AosXmlTagPtr getDoc(
				const u64 &docid,	
				const u64 snap_id,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getFixedDoc(
				const u64 &docid, 
				const int record_len, 
				const u64 snap_id,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getCommonDoc(
				const u64 &docid, 
				const int record_len, 
				const u64 snap_id,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getCSVDoc(
				const u64 &docid, 
				const int record_len, 
				const u64 snap_id,
				const AosRundataPtr &rdata);

	bool	getDocs(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff,
				const u32 num_docids,
				const AosBuffPtr &docid_buff,
				map<u64, int> &sizeid_len,
				u64 &end_docid,
				const u64 snap_id);

	AosBuffPtr	getFixedDocs(
					const AosRundataPtr &rdata,
					const vector<u64> &docids,
					const int record_size);

	AosBuffPtr	getCommonDocs(
					const AosRundataPtr &rdata,
					const vector<u64> &docids,
					const int record_size);

	AosBuffPtr	getCSVDocs(
					const AosRundataPtr &rdata,
					const vector<u64> &docids,
					const int record_size);

	bool	isDocDeleted(
				const u64 &docid, 
				bool &result, 
				const AosRundataPtr &rdata);
	
	AosDocFileMgrObjPtr getDocFileMgr(
				const u64 &docid,
				const AosRundataPtr &rdata);

	AosDocFileMgrObjPtr getGroupedDocFileMgr(
				const int vid,
				const bool create_flag,
				const AosRundataPtr &rdata);

	//bool	procDocLock(
	//			const AosRundataPtr &rdata,
	//			const AosTransPtr &trans,
	//			const u64 &docid,
	//			const OmnString &lcok_type,
	//			const u64 &lock_timer,
	//			const u64 &lockid);

	//bool	procCheckLock(
	//			const AosRundataPtr &rdata,
	//			const u64 &docid,
	//			const OmnString &type);

	bool	notifyToClients(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata);

	/*
	bool	incrementValue(
				const AosRundataPtr &rdata,
		 		const u64 &docid,
				const OmnString &aname,
				const u64 &initvalue,
				const u64 &incValue,
				u64 &newvalue,
				const AosTransId &trans_id);

	bool	batchGetDocs(
				const OmnString &scanner_id,
				AosBuffPtr &resp_buff,
				const AosRundataPtr &rdata);

	bool 	batchReader(
				const OmnString &scanner_id,
				const AosDocBatchReaderReq::E type,
				const AosBuffPtr &cont,
				const u32 &client_id,
				const AosRundataPtr &rdata);
	*/

	bool	batchSaveGroupedDoc(
				const AosTransPtr &trans, 
				const int virtual_id,
				const u32 sizeid,
				const u64 &num_docs,
				const int record_len,
				const AosBuffPtr &docids_buff,
				const AosBuffPtr &raw_data,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchSaveCSVDoc(
				const AosTransPtr &trans, 
				const int virtual_id,
				const u32 sizeid,
				const u64 &num_docs,
				const int record_len,
				const u64 &data_len,
				const AosBuffPtr &buff,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchInsertDocNew(
				const AosTransPtr &trans,
				const AosBuffPtr &buff,
				const int virtual_id,
				const u64 group_id,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchUpdateDocNew(
				const AosTransPtr &trans,
				const AosBuffPtr &buff,
				const int virtual_id,
				const u64 group_id,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchInsertCommonDoc(
				const AosTransPtr &trans, 
				const int virtual_id,
				const u32 sizeid,
				const u64 &num_docs,
				const int record_len,
				const u64 &data_len,
				const AosBuffPtr &buff,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchDeleteCSVDoc(
				const AosTransPtr &trans, 
				const int virtual_id,
				const u32 sizeid,
				const u64 &num_docs,
				const int record_len,
				const u64 &data_len,
				const AosBuffPtr &buff,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	batchUpdateCSVDoc(
				const AosTransPtr &trans, 
				const int virtual_id,
				const u32 sizeid,
				const u64 &num_docs,
				const int record_len,
				const u64 &data_len,
				const AosBuffPtr &buff,
				const u64 &snap_id,
				const AosRundataPtr &rdata);

	bool	saveBinaryDoc(
				const AosXmlTagPtr &doc,
				const AosBuffPtr &buff,
				OmnString &signature,
				const AosRundataPtr &rdata,
				const AosTransId &trans_id,
				const u64 snap_id);
	
	bool	deleteBinaryDoc(
				const u64 &docid,
				const AosRundataPtr &rdata,
				const AosTransId &trans_id,
				const u64 snap_id);

	bool	retrieveBinaryDoc(
				const AosXmlTagPtr &doc,	
				AosBuffPtr &buff,
				const AosRundataPtr &rdata,
				const u64 snap_id);

	bool 	deleteBatchDocs(
				const OmnString &scanner_id,
				const AosRundataPtr &rdata);

	bool    getGroupedDocWithComp(){return mIsGroupedDocWithComp;}

	bool    mergeSnapshot(
				const int virtual_id,
				const AosDocType::E doc_type,
				const u64 target_snap_id,
				const u64 merge_snap_id,
				const AosTransId &trans_id,
				const AosRundataPtr &rdata);

	u64 	createSnapshot(
				const int virtual_id,
				const u64 snap_id,
				const AosDocType::E doc_type,
				const AosTransId &trans_id,
				const AosRundataPtr &rdata);

	bool 	commitSnapshot(
				const int virtual_id,
				const AosDocType::E doc_type,
				const u64 &snap_id,
				const AosTransId &trans_id,
				const AosRundataPtr &rdata);

	bool 	rollbackSnapshot(
				const int virtual_id,
				const AosDocType::E doc_type,
				const u64 &snap_id,
				const AosTransId &trans_id,
				const AosRundataPtr &rdata);
private:
	u64		parseDocid(
				const AosRundataPtr &rdata,
				const u64 &docid);

	AosDocFileMgrObjPtr getDocFileMgrByVid(
				const int vid, 
				const AosDfmConfig &config,
				const AosRundataPtr &rdata);

	AosDocFileMgrObjPtr getDocFileMgrPriv(
				const int vid,
				const AosDocType::E type,
				const AosRundataPtr &rdata);

	bool 	isSvrHasFrontEnd(const int svr_id);		 // Ketty 2014/03/28

public:
	/*
	bool createDoc_3_1(AosRundata *rdata, 
				const u64 docid,
				const u64 snap_id,
				const AosBuff &doc_body);

	AosDocFileMgrNewObj* getDocFileMgr_3_1(
				AosRundata *rdata,
				const u32 repository_id,
				const int module_id,
				const u64 docid);

	u64 parseDocid_3_1(const u64 &docid);
	AosDocFileMgrNewObj *getDFM(
			const u32 repository_id, 
			const u32 module_id,
			const u32 vid)
	{
		if (repository_id >= mNumRepositoryIDs) return 0;
		return mDfms[repository_id].getDFM(module_id, vid);
	}

	bool setDFM(AosDocFileMgrNewObj *dfm, 
			const u32 repository_id,
			const u32 module_id, 
			const u32 vid)
	{
		if (repository_id >= eMaxRepository) return false;
		if (repository_id >= mNumRepositoryIDs) 
		{
			DfmEntry *entries = new DfmEntry[repository_id+1];
			for (u32 i=0; i<mNumRepositories; i++) entries[i] = mDfms[i];
			delete mDfms;
			mDfms = entries;
			mNumRepositories = repository_id+1;

			return mDfms[repository_id].setDFM(dfm, module_id, vid);
		}

			
		return mDfms[repository_id].setDFM(module_id, vid);
	*/
};
#endif
