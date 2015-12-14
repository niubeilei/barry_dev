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
#ifndef AOS_IILMgrBig_IILMgr_h
#define AOS_IILMgrBig_IILMgr_h

//#include "DocTrans/DocTransMgr.h"
//#include "DocTrans/DocTransProc.h"
#include "IdGen/IdGen.h"
#include "IDTransMap/IDTransMap.h"
#include "IILMgrBig/IIL.h"
#include "IILMgrBig/IILCompStr.h"
#include "IILMgrBig/IILCompU64.h"
#include "IILMgrBig/IILHit.h"
#include "IILMgrBig/IILStr.h"
#include "IILMgrBig/IILU64.h"
#include "IILMgrBig/Ptrs.h"
#include "SEUtil/IILName.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/IDTransMapCaller.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/LockMonitor.h"
#include "Thread/ThreadedObj.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/IILTrans.h"
#include "TransUtil/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "Util/HashMap.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"


#include <vector>
#include <hash_map>
using namespace std;
#define AOS_CHECK_LOCK_L
#define AOS_CHECK_LOCK_LS
#define AOS_CHECK_LOCK_U
#define AOS_CHECK_LOCK_US
#define AOSIILCOUNT_TRIES	sgNumIILRequests++
#define AOSIILCOUNT_HITS	sgNumIILHits++
#define AOSIILCOUNT_MISS	sgNumIILMiss++
#define AOSIILCOUNT_LOADS	sgNumIILLoads++
#define AOSIILCOUNT_POPS	sgNumIILPops++

OmnDefineSingletonClass(AosIILMgrSingleton,
						AosIILMgr,
						AosIILMgrSelf,
						OmnSingletonObjId::eIILMgr,
						"IILMgr");


// class AosU64Hash1{
// public:
// 	size_t operator()(const u64& value) const
// 	{
// 		u16 __h = 0;
// 		__h = ((u16*)(&value))[0] +
// 			  ((u16*)(&value))[1] +
// 			  ((u16*)(&value))[2] +
// 			  ((u16*)(&value))[3];
  //       return size_t(__h);
	// }
// };


class AosIILMgr : virtual public OmnRCObject, 
				   virtual public OmnThreadedObj,
				   virtual public AosIDTransMapCaller,
				   virtual public AosIILMgrObj,
				  //virtual public AosDocTransProc
				  virtual public AosDfmProc
{
	OmnDefineRCObject;

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
		eMaxWaitForForceSave = 120
	};

	// typedef hash_map<u64,AosIILPtr,AosU64Hash1> IILIDHash;
	// typedef hash_map<u64,AosIILPtr,AosU64Hash1>::iterator IILIDHashItr;
	typedef AosHashMap<u64, AosIILPtr, AosU64Hash1, u64_cmp, 10000> IILIDHash;
	typedef AosHashMap<u64, AosIILPtr, AosU64Hash1, u64_cmp, 10000>::iterator IILIDHashItr;

private:
	u32					mMaxIILs;
	u32					mTotalIILs;
	u32					mCrtIILId;
	AosIILPtr			mIILHead[eAosIILType_Total];
	AosIILPtr			mIILTail[eAosIILType_Total];
	IILIDHash			mIILIDHash;
	OmnDynArray<AosIILPtr, eArrayInitSize, eArrayIncSize, eArrayMaxSize>		mIILs;

	//ken 2011/08/18
	AosIDTransMapObjPtr		mIDTransMap;
	//AosDocTransMgrPtr		mDocTransMgr;
	AosTransModuleSvrPtr 	mTransServer;
	
	OmnMutexPtr			mLock;
	OmnThreadPtr		mThread;
	int					mSaveFreq;
	bool				mIsStopping;

	// Administration Data
	bool 				mSanityCheck;
	bool 				mShowLog;
	bool				mShowLocking;
	bool 				mCheckRef;
	bool 				mCheckPtr;
	AosLockMonitorPtr	mLockMonitor;
	bool				mSaveBatch;

	// Chen Ding, AAAA
	//deque<AosIILPtr> 		mIILsToSave;
	list<AosIILPtr> 		mIILsToSave;		// Ketty 2013/03/06
	OmnMutexPtr             mSavingLock;
	OmnCondVarPtr           mSavingCondVar;
	vector<OmnThreadPtr>    mSavingThreads;
	OmnMutexPtr				mCreationLock;

	OmnThreadPtr			mCreateIILThread;
	OmnMutexPtr				mCreatedIILLock;
	deque<AosIILPtr>		mCreatedIILs;
	OmnCondVarPtr			mCreatedIILCondVar;

	AosTransDistributorPtr  mRobin;     // Ketty 2012/11/02
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
	virtual bool	checkThread(OmnString &err, const int thrdLogicId) const;

private:
	bool			mainThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool			savingThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool			createIILThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

	bool			sanityCheckIdleListOrder(const AosIILType type);
	bool 			checkHeadTail(const AosIILType &type) const;
	bool 			iilListSanityCheck(
						const AosIILType type, 
						const int total,
						const AosRundataPtr &rdata);
	bool 			iilListSanityCheck2(
						const AosIILPtr &iil,
						const AosRundataPtr &rdata);
	
	bool 			removeFromIILListPriv(
						const AosIILPtr &iil,
						const AosRundataPtr &rdata);
	bool			returnIILPriv(
						const AosIILPtr &iil,
						const bool iillocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader);
	
	AosIILPtr 		createIILPriv(
						const u64 &iilid,
						const u32 siteid, 
						const AosIILType type,
						const bool isPersis,
						bool &needSave,
						const AosRundataPtr &rdata);
	AosIILPtr		createNewIILPriv(
						const u64 &wordid, 
						const u64 &iilid, 
						const u32 siteid, 
						const AosIILType type, 
						const bool ispersis, 
						const AosRundataPtr &rdata);

	bool			procAllTrans(const AosRundataPtr &rdata);
	bool			needPopIIL(const AosIILType type);

public:
	static void 	printHitStat();
	
	static AosIILType getCompType(const AosIILType type)
	{
		aos_assert_r(type == eAosIILType_Str || type == eAosIILType_U64, eAosIILType_Invalid);
		if(type == eAosIILType_Str) return eAosIILType_CompStr;
		return eAosIILType_CompU64;
	}
	
	static bool 	isCompType(const AosIILType type)
	{
		if(type == eAosIILType_CompStr || type == eAosIILType_CompU64) return true;
		return false;
	}
	
	//AosDocTransPtr	getDocTrans();
	//void			returnDocTrans(const AosDocTransPtr &doc);
	
	AosIILPtr		getIILPublic(
						const u64 &iilid, 
						const u32 siteid,
						const AosIILTransPtr &trans, 
						const AosRundataPtr &rdata);

	AosIILPtr 		createSubIILSafe(
	 					const u64 &parentId,
	 					const u32 siteid,
						const AosIILType &iil_type, 
						const bool isPersis,
	 					const bool IILMgrLocked,
	 					const AosRundataPtr &rdata);

	bool			createIILID(
						//const bool isNew,
						//const u64 &old_iilid,
						u64 &new_iilid,
						const u32 virtual_id,   // Ketty 2012/08/08
						const AosRundataPtr &rdata);
	bool			parseIILID(
						const u64 &iilid,
						u64 &localid,
						int &vid,
						const AosRundataPtr &rdata);
	// Chen Ding, 12/10/2012
	virtual u64		getIILID(
						const OmnString &word,
						const bool create,
						const AosRundataPtr &rdata);	
	bool			saveAllIILs(
						const bool reset,
						const AosRundataPtr &rdata);

	virtual bool	preQuerySafeByIIL(
						const AosIILObjPtr &iilobj,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	virtual bool	querySafeByIIL(
						const AosIILObjPtr &iilobj,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	bool			queryRangeSafeByIIL(
						const AosIILPtr &iil,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata);
	virtual bool	addTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const bool needRsp,
						const AosRundataPtr &rdata);
	virtual bool	procTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIDTransVectorPtr &p,
						const AosRundataPtr &rdata);
	bool			procOneTrans(
						const u64 &iilid,
						const u32 siteid,
						const AosIILTransPtr &trans,
						const AosRundataPtr &rdata);
	virtual bool	finishTrans(
						const u64 &global_tid,
						const OmnString &resp_msg, 
						const AosRundataPtr &rdata); 
	// Ketty 2012/11/30
	//virtual bool	finishTrans(
	//					vector<u64> &trans,
	//					const AosRundataPtr &rdata); 
	//bool		 	recoverHeaderBody(
	//					const u64 &entryid,
	//					const AosBuffPtr &headerBuff,
	//					const AosBuffPtr &bodyBuff);
	bool 			createIILPublic(
						u64 &iilid,
						const u32 siteid,
						const AosIILType iiltype,
						const bool isPersis,
						const AosRundataPtr &rdata);

	AosIILPtr 		loadIILByIDPublic(
						const u64 &iilid,
						const u32 siteid,
						AosIILType &iiltype,
						const bool iscompiil,
						const bool isiilmgrlocked,
						const AosRundataPtr &rdata); 

	AosIILPtr		loadIILByIDFromHashPublic(
						const u64 &iilid,
						const u32 siteid,
						AosIILType &iiltype,
						const bool iscompiil,
						const bool iilmgr_locked,
						const AosRundataPtr &rdata); 

	void			setSaveBatch(const bool savebatch){mSaveBatch = savebatch;}

	inline bool		returnIILPublic(
						const AosIILPtr &iil, 
						const AosRundataPtr &rdata) 
	{
		bool returned;
		return returnIILPublic(iil, false, returned, rdata, false);
	}
	
	inline bool		returnIILPublic(
						const AosIILPtr &iil, 
						const bool IILMgrLocked,
						const AosRundataPtr &rdata) 
	{
		bool returned;
		return returnIILPublic(iil, IILMgrLocked, returned, rdata, false);
	}

	bool			returnIILPublic(
						const AosIILPtr &iil,
						const bool IILMgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader)
	{
		AOSLMTR_ENTER1(mLockMonitor, IILMgrLocked);
		AOSLMTR_LOCK1(mLockMonitor, IILMgrLocked);
		bool rslt = returnIILPriv(iil, false, returned, rdata, returnHeader);
		AOSLMTR_UNLOCK1(mLockMonitor, IILMgrLocked);
		AOSLMTR_FINISH1(mLockMonitor, IILMgrLocked);
		return rslt;
	}
	
	bool			returnIILPublic(
						const AosIILPtr &iil,
						const bool iillocked,
						const bool IILMgrLocked,
						bool &returned,
						const AosRundataPtr &rdata,
						const bool returnHeader)
	{
		AOSLMTR_ENTER1(mLockMonitor, IILMgrLocked);
		AOSLMTR_LOCK1(mLockMonitor, IILMgrLocked);
		bool rslt = returnIILPriv(iil, iillocked, returned, rdata, returnHeader);
		AOSLMTR_UNLOCK1(mLockMonitor, IILMgrLocked);
		AOSLMTR_FINISH1(mLockMonitor, IILMgrLocked);
		return rslt;
	}

	virtual bool	deleteIILSafe(
						const AosIILObjPtr &iilobj,
						const AosRundataPtr &rdata);

	AosIILPtr		getAndRemoveFirstNonDirty(
						const AosIILType type,
						bool &needSave,
						const AosRundataPtr &rdata);
	
	AosIILPtr		createIILPublic1(
						const u64 &iilid, 
						const u32 siteid, 
						const AosIILType type, 
						const bool isPersis, 
						const bool isSubiil,
						const AosRundataPtr &rdata);
	
	bool			saveIILs(
						const AosIILPtr &iil,
						const bool iilNeedUnlock,
						const AosRundataPtr &rdata);

	bool			removeFromHash(
						const u64 &iilid,
						const u32 siteid,
						const bool IILMgrLocked,
						const AosRundataPtr &rdata);
	
	bool			queryNewSafe(
						const u64 &iilid,
						const u32 siteid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);

	bool			queryNewSafe(
						const AosIILPtr &iil,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata);

	virtual bool	createRootIILPublic(
						u64 &iilid,
						const u32 siteid,
						const AosIILType iiltype,
						const bool isPersis,
						const AosRundataPtr &rdata);

	AosIILStrSuperPtr createSuperIIL(
						const u64 &iilid,
						const bool iilmgrlocked,
						const AosRundataPtr &rdata);

	virtual bool	StrAddBlockSafe(
						const OmnString &iilname,
						char * entries,
						const int size,
						const int num,
						const AosIILExecutorObjPtr &executor,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	
	virtual bool	StrDelBlockSafe(
						const OmnString &iilname,
						char * entries,
						const int size,
						const int num,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);
	
	virtual bool	StrIncBlockSafe(
						const OmnString &iilname,
						char * &entries,
						const int size,
						const int num,
						const u64 &dftValue,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	virtual bool	StrDecBlockSafe(
						const OmnString &iilname,
						char * &entries,
						const int size,
						const int num,
						const bool delete_flag,
						const AosIILUtil::AosIILIncType incType,
						const bool iilmgrLocked,
						const AosRundataPtr &rdata);

	// Chen Ding, 2013/01/14
	virtual bool	retrieveIILBitmap(
						const OmnString &iilname,
						AosBitmapObjPtr &bitmap, 
						const AosBitmapObjPtr &partial_bitmap, 
						const AosBitmapTreeObjPtr &bitmap_tree, 
						AosRundataPtr &rdata);
	
	// Ketty 2012/10/29
	AosDocFileMgrObjPtr getDocFileMgr(
			const u64 &iilid, 
			u64 &local_iilid,
			const AosRundataPtr &rdata);

	// DfmProc Interface
	virtual OmnString   getFnamePrefix(){ return "IIL"; };
	virtual u32         getDocHeaderSize(){ return AosIIL::eIILHeaderSize; };
	virtual bool	finishTrans(const vector<u64> &trans, const AosRundataPtr &rdata); 
	virtual u64		parseLocId(const u64 loc_id, const u32 virtual_id);
	virtual AosDfmDocPtr  getTempDfmDoc(){ return OmnNew AosDfmDocIIL(0); };
	virtual bool		needDeltaHeader(){ return true; };
	// Interface end.
	
	// Ketty 2012/11/09
	bool	sendResp(const AosIILTransPtr &trans, const AosBuffPtr &resp_buff);

	// Ketty 2012/12/18
	bool 	cleanBkpVirtual(const AosRundataPtr &rdata, const u32 virtual_id);

	//bool    		sendResp(
	//					const AosIILTransPtr &trans,
	//					const AosRundataPtr &rdata);
	
	
	// Ketty 2013/01/29
	virtual bool	startTrans(const AosRundataPtr &rdata);
	virtual bool	commitTrans(const AosRundataPtr &rdata);
	virtual bool	rollbackTrans(const AosRundataPtr &rdata);
	
	// Ketty 2012/12/18                                           
	virtual bool    svrIsUp(const AosRundataPtr &rdata, const u32 svr_id);



	virtual bool	checkIsIILIDUsed(
						const u64 &iilid, 
						const u32 siteid, 
						const AosRundataPtr &rdata);

	// Chen Ding, 2013/03/03
	virtual bool retrieveQueryBlock(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						const AosQueryReqObjPtr &query);

	virtual bool retrieveNodeList(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						const AosQueryReqObjPtr &query);
};	
#endif

 
