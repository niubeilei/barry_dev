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
// 	Created: 2011/07/21 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgr_IILMgr_h
#define AOS_IILMgr_IILMgr_h

#include "IdGen/IdGen.h"
#include "IDTransMap/IDTransMap.h"
#include "TransUtil/IILTrans.h"
#include "DfmUtil/DfmDocIIL.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILCompStr.h"
#include "IILMgr/IILCompU64.h"
#include "IILMgr/IILHit.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "IILMgr/IILBigStr.h"
#include "IILMgr/IILBigU64.h"
#include "IILMgr/Ptrs.h"
#include "SEUtil/IILName.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/IDTransMapCaller.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/LockMonitor.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadPool.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "Util/HashMap.h"
#include "Util/DynArray.h"
#include "Util/LRUCache.h"
#include "UtilComm/Ptrs.h"

#include <vector>
#include <hash_map>

using namespace std;

OmnDefineSingletonClass(AosIILMgrSingleton,
						AosIILMgr,
						AosIILMgrSelf,
						OmnSingletonObjId::eIILMgr,
						"IILMgr");


class AosIILMgr : virtual public OmnRCObject, 
				   virtual public OmnThreadedObj,
				   virtual public AosIDTransMapCaller,
				   virtual public AosIILMgrObj
{
	OmnDefineRCObject;

	struct LockGen 
	{
		OmnMutexPtr getData(const u64 iilid)
		{
			return OmnNew OmnMutex();
		}

		OmnMutexPtr getData(const u64 iilid, const OmnMutexPtr &lock)
		{
			return lock;
		}
	};
	
	AosLRUCache<u64, OmnMutexPtr> mLockPool;
	LockGen 					  mLockGen;


public:
	enum
	{
		eDftMaxFixedIILSize = 10000,
		eArrayInitSize = 20000,
		eArrayIncSize = 10000,
		eIdBlocksize = 10,
		eArrayMaxSize = 100000,
		eDftSaveFreq = 10,
		eSaveBatchSize = 100,
		eMaxRetrieveIILTries = 10,
		eMaxPageSize = 10000,
		eMaxObjidTries = 100,
		eNotPossible = 2,
		eSecLength = 5,
		eMaxCreatedIILs = 10000,
		eMaxWaitForForceSave = 120,
		eSavingThreadNum = 3
	};

	typedef AosHashMap<u64, AosIILObjPtr, AosU64Hash1, u64_cmp, 10000> IILIDHash;
	typedef typename AosHashMap<u64, AosIILObjPtr, AosU64Hash1, u64_cmp, 10000>::iterator IILIDHashItr;

private:
	OmnMutexPtr				mLock;
	OmnThreadPtr			mThread;
	bool					mIsStopping;
	bool					mIsFreshing;
	bool					mCleanCache;
	int						mVirServerNum;
	int						mSaveFreq;
	u32						mMaxIILs;

	AosIILObjPtr			mIILHead[eAosIILType_Total];
	AosIILObjPtr			mIILTail[eAosIILType_Total];

	AosTransDistributorPtr	mRobin;
	IILIDHash				mIILIDHash;
	AosIDTransMapObjPtr		mIDTransMap;
	
	OmnMutexPtr             mSavingLock;
	OmnCondVarPtr           mSavingCondVar;
	vector<OmnThreadPtr>    mSavingThreads;
	list<AosIILObjPtr> 		mSavingList;
	
public:
	static bool 			smShowLog;
	static OmnThreadPoolPtr	smThreadPool;
	static bool				smCopyDataWithThrd;
	static i64				smCopyDataWithThrdIILSize;
	static i64				smCopyDataWithThrdNum;

public:
	AosIILMgr();
	~AosIILMgr();

    // Singleton class interface
    static AosIILMgr*	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
	virtual bool	checkThread111(OmnString &err, const int thrdLogicId);
	//virtual bool	checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool			mainThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool			savingThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread, const u32 thread_id);

	bool 			checkHeadTailLocked(const AosIILType &type) const;
	
	bool 			removeIILFromIdleListPriv(
						const AosIILObjPtr &iil,
						const AosRundataPtr &rdata);

	bool			returnIILPriv(
						const AosIILObjPtr &iil,
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);
	
	AosIILObjPtr		getIILFromHashPriv(
						const u64 &iilid,
						const u32 siteid,
						AosIILType &iiltype,
						const AosRundataPtr &rdata); 

	bool			needPopIIL(const AosIILType type);

	AosIILObjPtr		getAndRemoveFirstNonDirtyLocked(
						const AosIILType type,
						bool &needSave,
						const AosRundataPtr &rdata);

	AosIILObjPtr		getNoneUsedIILPriv(
						const AosIILType &iiltype,
						const bool force_create_new,
						const AosRundataPtr &rdata);

	bool			switchSnapShot(
						const AosIILObjPtr &iil,
						const u64 &snap_id,
						const AosRundataPtr &rdata);

	bool			flushContentsBySnapId(
						const u64 &snap_id,
						const AosRundataPtr &rdata);
public:
	bool			saveAllIILs(const AosRundataPtr &rdata);

	bool			saveIIL(
						const AosIILObjPtr &iil,
						const AosRundataPtr &rdata);

	//inline bool		returnIILPublic(
	bool		returnIILPublic(
						const AosIILObjPtr &iil, 
						const AosRundataPtr &rdata) 
	{
		bool returned;
		return returnIILPublic(iil, returned, true, rdata);
	}

	bool			returnIILPublic(
						const AosIILObjPtr &iil,
						bool &returned,
						const bool returnHeader,
						const AosRundataPtr &rdata);

	AosIILObjPtr 		loadIILPublic(
						const u64 &iilid,
						const u32 siteid,
						const u64 &snap_id,
						AosIILType &iiltype,
						const AosRundataPtr &rdata); 

	AosIILObjPtr		getIILFromHashPublic(
						const u64 &iilid,
						const u32 siteid,
						AosIILType &iiltype,
						const AosRundataPtr &rdata); 

	bool			removeIILFromHashPublic(
						const u64 &iilid,
						const u32 siteid,
						const AosRundataPtr &rdata);

	bool			createIILID(
						u64 &new_iilid,
						const u32 virtual_id,	// Ketty 2012/08/08
						const AosRundataPtr &rdata);
	bool			parseIILID(
						const u64 &iilid,
						u64 &localid,
						int &vid,
						const AosRundataPtr &rdata);

	virtual bool	addTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata);

	virtual AosIILObjPtr getIILPublic(
						const u64 &iilid, 
						const u32 siteid,
						const u64 snap_id, 
						const AosIILType iiltype,
						const AosRundataPtr &rdata);

	virtual AosIILObjPtr getIILPublic(
						const u64 &iilid, 
						const u32 siteid,
						const AosIILTransPtr &trans, 
						const AosRundataPtr &rdata);

	virtual bool	procTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIDTransVectorPtr &p,
						const AosRundataPtr &rdata);
	virtual bool	procOneTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata);
	virtual bool	checkNeedProc(
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata);
	bool			isSpecialIILName(const OmnString &iilname);
	bool			isSpecialIILID(const u64 &iilid);

	bool			procAllTrans(const AosRundataPtr &rdata);

	AosIILObjPtr		createIILPublic(
						const u64 &iilid, 
						const u32 siteid, 
						const u64 &snap_id,
						const AosIILType type, 
						const bool isPersis, 
						const AosRundataPtr &rdata);

	AosIILObjPtr	createNewIIL(
						const AosIILType type, 
						const AosRundataPtr &rdata);

	virtual u64		getIILID(
						const OmnString &word,
						const bool create,
						const AosRundataPtr &rdata);	

	virtual bool	querySafeByIIL(
						const AosIILObjPtr &iil,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);
	virtual bool	rebuildBitmap(
						const AosIILObjPtr &iil,
						const AosRundataPtr &rdata);


	AosIILObjPtr 	createSubIILSafe(
	 					const u64 &parentId,
	 					const u32 siteid,
						const u64 &snap_id,
						const AosIILType &iiltype, 
						const bool isPersis,
	 					const AosRundataPtr &rdata);

	virtual bool 	StrBatchAddMergeSafe(
						const OmnString &iilname,
						const i64 &size,
						const AosIILExecutorObjPtr &executor,
						const bool true_delete,
						const AosRundataPtr &rdata);

	virtual bool	StrBatchIncMergeSafe(
						const OmnString &iilname,
						const i64 &size,
						const u64 &dftvalue, 
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata);

	// Ketty 2012/10/29
	AosDocFileMgrObjPtr getDocFileMgr(
						const u64 &iilid, 
						u64 &local_iilid,
						const AosRundataPtr &rdata);

	virtual bool    svrIsUp(
						const AosRundataPtr &rdata,
						const u32 svr_id);

	// Ketty 2012/12/18
	bool 			cleanBkpVirtual(
						const AosRundataPtr &rdata,
						const u32 virtual_id);

	virtual bool	checkIsIILIDUsed(
						const u64 &iilid, 
						const u32 siteid, 
						const AosRundataPtr &rdata);

	// Chen Ding, 2013/04/19
	virtual AosIILObjPtr nextLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx);

	virtual AosIILObjPtr firstLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx);

	virtual AosIILObjPtr nextLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree);

	virtual AosIILObjPtr firstLeaf(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						AosIILIdx &idx,
						AosBitmapTreeObjPtr &tree);

	//// felicia, 2013/05/14
	//virtual bool 		createSnapShot(
	//						u32 &snap_id,
	//						const u32 virtual_id,
	//						const bool need_remove,
	//						const AosRundataPtr &rdata);

	//virtual bool		commitSnapShot(
	//						const OmnString &iilname,
	//						const AosRundataPtr &rdata);
	//virtual bool		commitSnapShot(const AosRundataPtr &rdata);
	//
	//virtual bool		rollBackSnapShot(
	//						const u32 snap_id,
	//						const u32 virtual_id,
	//						const AosRundataPtr &rdata);

	// Ken Lee, 2013/05/17
	virtual void		cleanCache();

	virtual bool mergeSnapshot(         
			const u32 virtual_id,
			const u64 &target_snap_id,
			const u64 &merge_snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	virtual u64	createSnapshot(
			const u32 virtual_id,
			const u64 snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	virtual bool commitSnapshot(
			const u32 virtual_id,
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);

	virtual bool rollbackSnapshot(
			const u32 virtual_id,
			const u64 &snap_id,
			const AosTransId &trans_id,
			const AosRundataPtr &rdata);


	//virtual bool	createSnapShot(
	//					const AosRundataPtr &rdata,
	//					const AosIILObjPtr &iil);

	//virtual bool	commitSnapShot(
	//					const AosRundataPtr &rdata,
	//					const AosIILObjPtr &iil);

	//virtual bool 	rollBackSnapShot(
	//					const AosRundataPtr &rdata,
	//					const AosIILObjPtr &iil);
	bool flushContents(const AosRundataPtr &rdata);
	bool clearOpenedIILs(const AosRundataPtr &rdata);

	AosIILObjPtr	createJimoTable(
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosRundataPtr &rdata);
 
	virtual u64		getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata);

	virtual bool	unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata);

	virtual OmnString getMemoryStatus();
};	

#endif

