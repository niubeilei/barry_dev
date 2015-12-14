////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// 1. It maintains a list of all IILs loaded into memory. 
// 2. For each wordid, there is an IIL. IILs are identified by IILID. 
//    Each wordid has an eight-byte 'ptr' that is used to store the IILID
//    and the pointer to the IIL. The upper four bytes are used to store
//    IILID, and the lower four bytes the pointer. When an IIL is not
//    loaded in memory, the lower four bytes are set to 0. Otherwise, 
//    it is set to the pointer. 
//
//    It is extremely important that it manages the memory correctly.
//    When an IIL is created, it sets its pointer. When it is reused
//    to another IIL, it needs to reset the pointer. 
// 3. When an external object needs to do something, it should call
//    one of the public member functions that are designed to manipulate
//    docs. These functions first use retrieveIIL(...) to retrieve the
//    IIL. It should always be able to retrieve the IIL. When the IIL
//    is retrieved, the IIL is locked. After finishing the operation, 
//    it decrements the reference count, and then unlock the IIL. It then
//    return the IIL to the pool.
//
// Modification History:
// 	Created: 2011/07/21 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IILMgr.h"

#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "IdGen/IdGenMgr.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "BitmapMgr/BitmapMgr.h"
#include "SearchEngine/SeError.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEModules/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "SEModules/LogMgr.h"
#include "SEUtil/Docid.h"
#include "SEUtil/Siteid.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgrUtil/DevLocation.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Thread.h"
#include "Thread/LockMonitor.h"
#include "Thread/LockMonitorMgr.h"
#include "TransServer/TransProc.h"
#include "TransUtil/IdRobin.h"
//#include "TransServer/TransSvrConnFast.h"
//#include "TransServer/TransSvrConnSync.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util1/Wait.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"

extern bool sgIsBigIILServer;

OmnSingletonImpl(AosIILMgrSingleton,
		AosIILMgr,
		AosIILMgrSelf,
		"AosIILMgr");

static int sgTotalIdleIIL[eAosIILType_Total];

static OmnCondVarPtr sgSavingCondVar = OmnNew OmnCondVar();
static OmnMutexPtr   sgSavingLock = OmnNew OmnMutex();

static OmnMutexPtr   sgTailLock = OmnNew OmnMutex();

const int sgSavingWait = 50;

static int sgNumIILRequests = 0;
static int sgNumIILHits = 0;
static int sgNumIILLoads = 0;
static int sgNumIILMiss = 0;
static int sgNumIILPops = 0;

static int sgLastIILRequests = 0;
static int sgLastIILHits = 0;
static int sgLastIILLoads = 0;
static int sgLastIILMiss = 0;
static int sgLastIILPops = 0;

//static int sgServerNum = 0;
static int sgVirServerNum = 0;
//static int sgServerId = 0;
//static int sgVirServerId = 0;

// Chen Ding, 01/25/2012
const u32 sgMaxDocFileMgrs = 100000;
static AosDevLocationPtr sgDevLocation;

AosIILMgr::AosIILMgr()
:
mMaxIILs(0),
mTotalIILs(0),
mCrtIILId(0),
mLock(OmnNew OmnMutex()),
mSaveFreq(eDftSaveFreq),
mIsStopping(false),
mSanityCheck(false),
mShowLog(false),
mShowLocking(false),
mCheckRef(false),
mCheckPtr(false),
mSaveBatch(true),
mSavingLock(OmnNew OmnMutex()),
mSavingCondVar(OmnNew OmnCondVar()),
mCreationLock(OmnNew OmnMutex()),
mCreatedIILLock(OmnNew OmnMutex()),
mCreatedIILCondVar(OmnNew OmnCondVar())
{
	mLockMonitor = OmnNew AosLockMonitor(mLock);
	mLock->mMonitor = mLockMonitor;
	AosLockMonitorMgr::getSelf()->addLock(mLockMonitor);
}


AosIILMgr::~AosIILMgr()
{
	// OmnDelete [] mIILs;
}


bool      	
AosIILMgr::start()
{
	return true;
}


bool        
AosIILMgr::stop()
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	if (mShowLocking) OmnScreen << "Locking" << endl;
	OmnScreen << "To stop IILMgr!" << endl;
	mIsStopping = true;
	if (mShowLocking) OmnScreen << "Unlocking" << endl;
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	OmnScreen << "IILMgr stopping!" << endl;
	
	AosRundataPtr rdata = OmnApp::getRundata();
	// Ketty 2013/01/30
	mIDTransMap->setIsStopping();
	procAllTrans(rdata);
	saveAllIILs(true, rdata);
	OmnScreen << "IILMgr stopped!" << endl;
	return true;
}


bool    
AosIILMgr::signal(const int threadLogicId)
{
	mSavingLock->lock();
	mSavingCondVar->signal();
	mSavingLock->unlock();
	return true;
}


bool    
AosIILMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosIILMgr::mainThread(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	// Chen Ding, 01/25/2012
	// TSK001NOTE
	// Need to resolve Siteid issue.
//	rdata->setSiteid("100");
	rdata->setAppname(AOSAPPNAME_SYSTEM);
	rdata->setUserid(AOSDOCID_IILMGRROOT);
	bool trans_processed = false;
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(mSaveFreq);

		if (mIsStopping || !mIDTransMap)
		{
			break;
		}
			
		if(mIDTransMap->needProcQueue())
		{
			mIDTransMap->procQueues(rdata);
			continue;
		}

		if(mIDTransMap->needProcTrans())
		{
			mIDTransMap->procTrans(rdata);
			continue;
		}

		while(1)
		{
			if(mIDTransMap->isFree())
			{
				trans_processed = false;
				mIDTransMap->postProc(trans_processed,rdata);
				if(!trans_processed)
				{
					break;
				}
				continue;				
			}
			break;
		}
	}
	return true;
	/*
	AosIILPtr iils[eSaveBatchSize];

	int idx = 0;
	AOSLMTR_ENTER(mLockMonitor);
	AosRundataPtr rdata = OmnNew AosRundata();
	rdata->setAppname(AOSAPPNAME_SYSTEM);
	rdata->setUserid(AOSDOCID_IILMGRROOT);
	while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(mSaveFreq);
		if(!mSaveBatch)
		{
			continue;
		}
		// Each time it wakes up, it gets a number of IILs
		// from 'mIILs' and then save them. 
		if (mShowLocking) OmnScreen << "Locking" << endl;
		AOSLMTR_LOCK(mLockMonitor);

		if (mIsStopping)
		{
			AOSLMTR_UNLOCK(mLockMonitor);
			break;
		}
		int num = 0;
		// OmnScreen << "To save: " << idx << ":" << mIILs.entries() << endl;
		for (int i=idx; i<mIILs.entries() && num<eSaveBatchSize; i++)
		{
			iils[num++] = mIILs[i];
		}

		idx += eSaveBatchSize;
		if (idx >= mIILs.entries()) idx = 0;
		if (mShowLocking) OmnScreen << "Unlocking" << endl;
		AOSLMTR_UNLOCK(mLockMonitor);

		for (int i=0; i<num; i++)
		{
			if (iils[i]) iils[i]->saveToFileSafe(false, rdata);
			iils[i] = 0;
		}
	}
	AOSLMTR_FINISH(mLockMonitor);
	return true;
	*/
}


bool
AosIILMgr::config(const AosXmlTagPtr &theconfig) 
{
	aos_assert_r(theconfig, false);
	AosIIL::staticInit(theconfig);
	AosIILStr::staticInit(theconfig);

	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;

	mSanityCheck = config->getAttrBool(AOSCONFIG_IILMGR_SANITYCHECK);
	mCheckPtr = config->getAttrBool(AOSCONFIG_IILMGR_CHECKPTR);
	mCheckRef = config->getAttrBool(AOSCONFIG_IILMGR_CHECKREF);
	mShowLog = config->getAttrBool(AOSCONFIG_SHOWLOG);

	// IIL Configurations are defined in the subtag AOSCONFIG_IILMGR
	if (mShowLocking) OmnScreen << "Locking" << endl;
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mMaxIILs = config->getAttrU64(AOSCONFIG_MAX_IILS, 0);
	aos_assert_rk(mMaxIILs > 0, mLock, false);;
	OmnScreen << "Max IILs: " << mMaxIILs << endl;

	//mIdgenName = config->getAttrStr(AOSCONFIG_IIL_IDGENNAME);
	//mIdBlocksize = config->getAttrInt(AOSCONFIG_IDBLOCK_SIZE, 10);
	
	//sgServerNum = config->getAttrU32(AOSTAG_NUM_SERVERS, 1);
	//sgVirServerNum = config->getAttrU32(AOSTAG_NUM_VIRTUALS, 1);
	
	// Ketty 2012/11/02
	//sgServerNum = AosGetNumPhysicals();	
	sgVirServerNum = AosGetNumCubes();	
	if (sgVirServerNum <= 0)
	{
		OmnAlarm << "Number of virtual IIL servers is less than 1."
			<< " Will set to 1!" << enderr;
		sgVirServerNum = 1;
	}
		
	//sgServerId = config->getAttrU32(AOSTAG_SERVERID, 0);
	//sgServerId = AosGetSelfServerId();
	//sgVirServerId = sgServerId; 

	for (int i=0; i<eAosIILType_Total; i++)
	{
		sgTotalIdleIIL[i] = 0;
	}
	if (mShowLocking) OmnScreen << "Unlocking" << endl;
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	AosIILTransServerPtr transServer = OmnNew AosIILTransServer();
	transServer->start(); // Ketty 2012/11/21
	if (sgIsBigIILServer)
	{
		mTransServer = OmnNew AosTransModuleSvr(AosModuleId::eSIIL, transServer);
	}
	else
	{
		mTransServer = OmnNew AosTransModuleSvr(AosModuleId::eIIL, transServer);
	}
	aos_assert_r(mTransServer, false);
	
	// Ketty 2013/01/17
	//AosXmlTagPtr doctrans = config->getFirstChild("iil_doc_trans");
	//aos_assert_r(doctrans, false);
	//AosDocTransProcPtr ptr(this, false);
	//mDocTransMgr = OmnNew AosDocTransMgr(doctrans, ptr);

	AosXmlTagPtr iil_map = config->getFirstChild("iil_map");
	aos_assert_r(iil_map, false);
	AosIDTransMapCallerPtr ptr2(this, false);
	mIDTransMap = OmnNew AosIDTransMap(ptr2, iil_map);

	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "IILThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	
	mSavingThreads.push_back(OmnNew OmnThread(thisPtr, "SavingIIL", 1, true, true, __FILE__, __LINE__));
	for(u32 i = 0; i < mSavingThreads.size(); i++)
	{
		mSavingThreads[i]->start();
	}

	// Chen Ding, 10/15/2011
	mCreateIILThread = OmnNew OmnThread(thisPtr, "CreateIIL", 2, true, true, __FILE__, __LINE__);
	mCreateIILThread->start();

	sgDevLocation = OmnNew AosDevLocation1();
	
	mRobin = OmnNew AosIdRobin();
	return true;
}

	
bool
AosIILMgr::saveAllIILs(const bool reset, const AosRundataPtr &rdata)
{
	for(int i = 0;i < eMaxWaitForForceSave;i++)
	{
		if(mIILsToSave.size() <= 0)
		break;
		
		OmnSleep(1);
	}
	// IMPORTANT: This will erase all the IILs. 
	IILIDHashItr itr = mIILIDHash.begin();
	AosIILPtr iil;
	for(; itr!=mIILIDHash.end(); itr++)
	{
		iil = itr->second;
		if(!iil)
		{
			OmnAlarm << "iil not exist, id:" << itr->first;
		}
		else if(!iil->isLeafIIL() && !iil->isCompIIL())
		{
			u32 refcount = iil->addRefCountByIILMgr();
			if (refcount == 1)
			{
				sgTailLock->lock();
				removeFromIILListPriv(iil, rdata);
				sgTailLock->unlock();
			}
			saveIILs(iil, false, rdata);
		}
	}

	if (reset)
	{
		mTotalIILs = 0;
	}
	return true;
}


bool
AosIILMgr::removeFromIILListPriv(
		const AosIILPtr &iil, 
		const AosRundataPtr &rdata)
{
	// IMPORTANT: In order to avoid locking, it is important
	// for this function not to call other functions that may
	// potentially lock the iil or AosIILMgrSelf.
	//
	// This function is a private function. It assumes 'mLock' 
	// is locked.
	// 
	// Chen Ding, 10/15/2011
	// Need to check whether the IIL is 'creation only', which happens
	// when an IIL is first needed but not created yet. In that case, 
	// the IIL may not be put in the idle list. This function needs
	// to check whether the iil is 'creation only'. If yes, it should not
	// remove from the idle list.

	// Chen Ding, 2013/01/16
	// !!!!!!!!!!!!! Important !!!!!!!!!!!!!!!
	// The caller must sgTailLock lock this function.
	if (iil->isCreationOnly())
	{
		// It is creation only. 
		AosIILPtr prev = iil->getPrevByIILMgr();		// Safe
		AosIILPtr next = iil->getNextByIILMgr();		// Safe
		aos_assert_r(!prev && !next, false);
		return true;
	}

	AosIILType type = iil->getIILType();		// Safe
	AosIILPtr prev = iil->getPrevByIILMgr();		// Safe
	AosIILPtr next = iil->getNextByIILMgr();		// Safe
	if (next) next->setPrevByIILMgr(prev);			// Safe
	if (prev) prev->setNextByIILMgr(next);			// Safe
	iil->resetPtrsByIILMgr();
	if (iil == mIILHead[type])
	{
		mIILHead[type] = next;
	}

	// Chen Ding, 2013/01/16
	if (iil == mIILTail[type])
	{
		mIILTail[type] = prev;
		if (!prev)
		{
			aos_assert_r(!mIILHead[type], false);
		}
	}

	sgTotalIdleIIL[type]--;

	prev = 0;
	next = 0;
//OmnScreen << "remove form idle, iilid:" << iil->getIILID() << ",iiltype:" << AosIILType_toStr(type)
//		  << ",idleSize:" << sgTotalIdleIIL[type] << ",total:" << AosIIL::getTotalIILs(type) << endl;
	if (sgTotalIdleIIL[type] < 0)
	{
		rdata->setError() << "Total iil incorrect: " << type << ":" << sgTotalIdleIIL[type];
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


bool
AosIILMgr::checkHeadTail(const AosIILType &type) const
{
return true;
	if(!mIILHead[type] || !mIILTail[type]) 
	{
		aos_assert_r(!mIILHead[type] && !mIILTail[type], false);
		return true;
	}

	// Check from head to tail
	int num = 0;
	int c = 0;
	AosIILPtr iil = mIILHead[type];
	while(iil->getNextByIILMgr())
	{
		num++;
		c = iil->getRefCountByIILMgr();
		aos_assert_r(c==0, false);

		iil = iil->getNextByIILMgr();
	}

	num++;
	aos_assert_r(iil == mIILTail[type], false);

	int nn = 0;
	iil = mIILTail[type];
	while(iil->getPrevByIILMgr())
	{
		nn++;
		c = iil->getRefCountByIILMgr();
		aos_assert_r(c==0, false);
		
		iil = iil->getPrevByIILMgr();
	}
	nn++;
	aos_assert_r(iil == mIILHead[type], false);

	aos_assert_r(nn == num, false);
	return true;
}
	

bool
AosIILMgr::returnIILPriv(
		const AosIILPtr &iil,
		const bool iillocked,
		bool &returned,
		const AosRundataPtr &rdata,
		const bool returnHeader)
{
	// Someone finished the use of 'iil'. This function puts
	// 'iil' in its corresponding iil queue. 
	//
	// IMPORTANT: In order to avoid locking, it is important
	// for this function not to call other functions that may
	// potentially lock the iil or AosIILMgrSelf.
	
	returned = false;
	aos_assert_r(iil, false);
	
	u32 cc = iil->removeRefCountByIILMgr();			// Safe
	if (cc > 0) 
	{
		// OmnScreen << "To return an IIL but ref count is not 0: " 
		//	<< iil->getIILID() << ". Type: "
		//	<< AosIILType_toStr(iil->getIILType())
		//	<< ", count:" << cc << ",idleSzie:" 
		//	<< sgTotalIdleIIL[iil->getIILType()] << enderr;
		return true;
	}
	returned = true;

	if(iil->isParentIIL())
	{
		//return all the subiil
		bool subreturned = false;
		if(iillocked)
		{
			iil->returnSubIILsPriv(true, subreturned, rdata, returnHeader);	
		}
		else
		{
			iil->returnSubIILsSafe(true, subreturned, rdata, returnHeader);	
		}
	}

	AosIILType type = iil->getIILType();				// Safe
	aos_assert_r(type < eAosIILType_Total, false);
	
	if (iil->isCreationOnly())
	{
		// It is creation only. 
		AosIILPtr prev = iil->getPrevByIILMgr();		// Safe
		AosIILPtr next = iil->getNextByIILMgr();		// Safe
		aos_assert_r(!prev && !next, false);
		return true;
	}

	sgTailLock->lock();
	sgTotalIdleIIL[type]++;

//OmnScreen << "return ill, iilid:" << iil->getIILID() << ",type:" << AosIILType_toStr(iil->getIILType())
//		  << ",count:" << cc << ",idleSzie:" << sgTotalIdleIIL[iil->getIILType()] << endl;
sanityCheckIdleListOrder(type);
	if (returnHeader)
	{
iil->setLastAddTimestamp(true);
		if (!mIILHead[type])
		{
			// The iil list is empty
			aos_assert_rl(!mIILTail[type], sgTailLock, false);
			if (sgTotalIdleIIL[type] != 1)
			{
				sgTailLock->unlock();
				rdata->setError() << "total iil incorrect: " << sgTotalIdleIIL[type];
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			iil->resetPtrsByIILMgr();							// Safe
			mIILHead[type] = iil;
			mIILTail[type] = iil;
		}
		else
		{
			// The iil list is not empty
			iil->setPrevByIILMgr(0);					// Safe
			iil->setNextByIILMgr(mIILHead[type]);	// Safe
			mIILHead[type]->setPrevByIILMgr(iil);	// Safe
			mIILHead[type] = iil;
			aos_assert_rl(mIILHead[type], sgTailLock, false);
		}
	}
	else
	{
		iil->setLastAddTimestamp(false);
		if (!mIILTail[type])
		{
			// The iil list is empty
			aos_assert_rl(!mIILHead[type], sgTailLock, false);
			if (sgTotalIdleIIL[type] != 1)
			{
				sgTailLock->unlock();
				rdata->setError() << "total iil incorrect: " << sgTotalIdleIIL[type];
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			iil->resetPtrsByIILMgr();							// Safe
			mIILHead[type] = iil;
			mIILTail[type] = iil;
		}
		else
		{
			// The iil list is not empty
			iil->setPrevByIILMgr(mIILTail[type]);	// Safe
			iil->setNextByIILMgr(0);				// Safe
			mIILTail[type]->setNextByIILMgr(iil);	// Safe
			mIILTail[type] = iil;
			aos_assert_rl(mIILTail[type], sgTailLock, false);
		}

		sgSavingLock->lock();
		sgSavingCondVar->signal();
		sgSavingLock->unlock();
	}

	if (!mIILHead[type] || !mIILTail[type]) 
	{
		if (sgTotalIdleIIL[type] != 0)
		{
			sgTailLock->unlock();
			OmnAlarm << "Internal error: " << sgTotalIdleIIL[type] << enderr;
			return false;
		}
	}
	sgTailLock->unlock();
	return true;
}


bool
AosIILMgr::iilListSanityCheck(
		const AosIILType type, 
		const int total,
		const AosRundataPtr &rdata)
{
	// The caller has locked the class. It checks whether the list
	// contains the right number of elements. In addition, no element
	// should be the same.
	return true;
	if (!mSanityCheck) return true;

	if (mIILHead[type]) 
	{
		aos_assert_r(mIILTail[type], false);
	}
	else
	{
		aos_assert_r(!mIILTail[type], false);
	}

	if (mIILTail[type]) 
	{
		aos_assert_r(mIILHead[type], false);
	}
	else
	{
		aos_assert_r(!mIILHead[type], false);
	}

	// Any IIL whose ref == 0 should be in the iil list
	AosIILPtr iil;
	for (u32 i=0; i<mTotalIILs; i++)
	{
		iil = mIILs[i];
		if (iil->getRefCountByIILMgr() == 0)
		{
			bool found = false;
			AosIILPtr theiil = mIILHead[iil->getIILType()];
			while (theiil)
			{
				if (theiil == iil)
				{
					found = true;
					break;
				}
				AosIILPtr pp = theiil->getNextByIILMgr();
				theiil = pp;
			}

			if (!(found || i==mTotalIILs-1))
			{
				rdata->setError() << "Failed sanity check: " << mTotalIILs << ":" << i;
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}
	}

	AosIILPtr ptr = mIILHead[type];
	if (total == 0)
	{
		aos_assert_r(!ptr, false);
		return true;
	}

	AosIILPtr prev;
	for (int i=0; i<total; i++)
	{
		aos_assert_r(ptr, false);
		aos_assert_r((i == total-1 && !ptr->getNextByIILMgr())||
			ptr->getNextByIILMgr(), false);
		aos_assert_r((i == 0 && !ptr->getPrevByIILMgr())||
			prev == ptr->getPrevByIILMgr(), false);

		prev = ptr;
		AosIILPtr pp = ptr->getNextByIILMgr();
		ptr = pp;
	}
	return true;
}


bool
AosIILMgr::iilListSanityCheck2(
		const AosIILPtr &iil,
		const AosRundataPtr &rdata)
{
	return true;
	if (!mSanityCheck) return true;
	// OmnScreen << "Total IIL: " << mTotalIILs
	// 	<< "\nU32 Idle: " << sgTotalIdleIIL[eAosIILType_U64]
	// 	<< "\nHIT Idle: " << sgTotalIdleIIL[eAosIILType_Hit]
	// 	<< "\nStr Idle: " << sgTotalIdleIIL[eAosIILType_Str] << endl;
	
	AosIILType type = iil->getIILType();
	AosIILPtr crt = mIILHead[type];
	if (sgTotalIdleIIL[type] == 0)
	{
		aos_assert_r(!crt, false);
		return true;
	}

	int idx = 0;
	bool found = false;
	while (crt && idx < sgTotalIdleIIL[type])
	{
		if (crt == iil) found = true;
		crt = crt->getNextByIILMgr();
		idx++;
	}

	aos_assert_r(found, false);
	aos_assert_r(idx == sgTotalIdleIIL[type], false);
	return true;
}


AosIILPtr
AosIILMgr::loadIILByIDPublic(
		const u64 &iilid,
		const u32 siteid,
		AosIILType &iiltype,
		const bool iscompiil,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata) 
{
	static int lsTotalLoads = 0;
	static int lsTotalLoadWaits = 0;
	static int lsTotalRepeatLoadWaits = 0;

	AosIILPtr iil = loadIILByIDFromHashPublic(iilid, siteid, iiltype, iscompiil, iilmgrLocked, rdata);
	if(iil) return iil;

	// Chen Ding, 2011/12/13
	// On 2011/12/12, when doing the data conversion, we found a bug that it failed
	// reading one data from buff, which should never happen. The buff is not corrupted.
	// Before the bug is located, when that happens again, we will give it another
	// try, hoping it can create the IIL in the second try.
	int guard = 2;
	while (guard--)
	{
		// Ketty 2012/11/15
		bool rslt = AosIIL::staticGetIILType(iiltype, iilid, rdata);
		aos_assert_rr(rslt, rdata, 0);
	
		if (iiltype == eAosIILType_Invalid)
		{
			// The IIL type is incorrect. Just return.
			//OmnAlarm << "Incorrect iiltype: " << iiltype << enderr;
			return 0;
		}
	
		if (iscompiil)
		{
			switch (iiltype)
			{
			case eAosIILType_Str:
			 	 iiltype = eAosIILType_CompStr;
			 	 break;

			case eAosIILType_U64:
			 	 iiltype = eAosIILType_CompU64;
			 	 break;

			case eAosIILType_CompStr:
			case eAosIILType_CompU64:
			 	 break;

			default:
			 	 OmnAlarm << "Incorrect iiltype: " << iiltype << enderr;
			 	 return 0;
			}
		}
	
		u64 iilid2 = iilid;
		if(iscompiil)
		{
			AosIIL::setCompIILIdBit(iilid2);
		}
		u64 combinedId = AosSiteid::combineSiteid(iilid2, siteid);

		bool first_load = true;
		bool first_wait = true;
		while (1)
		{
			// Will check whether the IIL is already loaded in memory. This is
			// done by checking 'mIILIDHash'.
			AOSLMTR_ENTER1(mLockMonitor,iilmgrLocked);
			AOSLMTR_LOCK1(mLockMonitor,iilmgrLocked);

			if (first_load)
			{
				first_load = false;
				lsTotalLoads++;
			}
		
			IILIDHashItr itr = mIILIDHash.find(combinedId);
			if(itr != mIILIDHash.end())
			{
				// It is already in memory
				iil = itr->second;
				u32 refcount = iil->addRefCountByIILMgr();
				if (refcount == 1)
				{
					sgTailLock->lock();
					removeFromIILListPriv(iil, rdata);
					sgTailLock->unlock();
				}
				AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
				AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
				return iil;
			}
		
			if(!needPopIIL(iiltype))
			{
				// We may create new IILs
				iil = createNewIILPriv(0, iilid, siteid, iiltype, false, rdata);
				if(!iil)
				{
					AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
					AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
					AosSetError(rdata, AosErrmsgId::eExceptionCreateIIL);
					OmnAlarm << rdata->getErrmsg() << ". IILID: " << iilid
						<< ". IILType: " << iiltype << enderr;
					return 0;
				}
				iil->addRefCountByIILMgr();
				mIILIDHash[combinedId] = iil;
				iil->lockIIL();
				AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
				AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
				//iil->loadFromFileLocked(iilid, siteid, false, rdata);
				iil->loadFromFilePriv(iilid, false, rdata);
				iil->unlockIIL();
				return iil;
			}
	
			// Need to pop some existing IILs
			bool needSave = false; 
			iil = getAndRemoveFirstNonDirty(iiltype, needSave, rdata);
			if (!iil)
			{
				if(!needSave)
				{
					OmnScreen << "ken add flag, ?????????????????????????" << endl;
					// create static
					iil = createNewIILPriv(0, iilid, siteid, iiltype, false, rdata);// shawnproblem
					if(!iil)
					{
						AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
						AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
						AosSetError(rdata, AosErrmsgId::eExceptionCreateIIL);
						OmnAlarm << rdata->getErrmsg() << ". IILID: " << iilid
							<< ". IILType: " << iiltype << enderr;
						return 0;
					}
					iil->addRefCountByIILMgr();
					mIILIDHash[iil->getIILIDForHash()] = iil;
					iil->lockIIL();
					AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
					AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
					//iil->loadFromFileLocked(iilid, siteid, false, rdata);
					iil->loadFromFilePriv(iilid, false, rdata);
					iil->unlockIIL();
					return iil;
				}
				AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
				AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
	
				if(needSave)
				{
					mSavingLock->lock();
					mSavingCondVar->signal();
					mSavingLock->unlock();
				}
	
				sgSavingLock->lock();
				if (first_wait)
				{
					first_wait = false;
					lsTotalLoadWaits++;
				}
				lsTotalRepeatLoadWaits++;
	
				bool isTimeout = false;
				// u64 t1 = OmnGetTimestamp();
				if(mIILsToSave.size())
				{
					sgSavingCondVar->timedWait(sgSavingLock, isTimeout, 0,200);
				}
				// if(isTimeout)
				// {
				//	u64 t2 = OmnGetTimestamp();
				//	OmnScreen << "wait for save and return iil, but timeout.time:" 
				//	<< t2-t1 << endl;
				//}
				sgSavingLock->unlock();
			}
			else
			{
				// It is guaranteed that no one holds pointers pointing to 'iil' now.
				u64 combinedId2 = iil->getIILIDForHash();
				mIILIDHash.erase(combinedId2);
				iil->resetSafe(0, iilid, siteid, true, rdata);
				iil->addRefCountByIILMgr();
				mIILIDHash[combinedId] = iil;
				iil->lockIIL();
				AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
				AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
				//bool rslt = iil->loadFromFileLocked(iilid, siteid, false, rdata);
				bool rslt = iil->loadFromFilePriv(iilid, false, rdata);
				iil->unlockIIL();
				if (rslt) return iil;

				// For some reason, it failed loading the IIL. Give it another try
				// to see whether it can load the IIL.
			}
		}
	}

	OmnAlarm << "Failed retrieve the IIL: " << iilid << ", type: " << iiltype
		<< ". Is Comp: " << iscompiil << ". Is locked: " << iilmgrLocked << enderr;
	return 0;
}


AosIILPtr
AosIILMgr::loadIILByIDFromHashPublic(
		const u64 &iilid,
		const u32 siteid,
		AosIILType &iiltype,
		const bool iscompiil,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata) 
{
	u64 iilid2 = iilid;
	if(iscompiil)
	{
		AosIIL::setCompIILIdBit(iilid2);
	}
	u64 combinedId = AosSiteid::combineSiteid(iilid2, siteid);
	
	AosIILPtr iil;
	AOSLMTR_ENTER1(mLockMonitor,iilmgrLocked);
	AOSLMTR_LOCK1(mLockMonitor,iilmgrLocked);
	IILIDHashItr itr = mIILIDHash.find(combinedId);
	if(itr != mIILIDHash.end())
	{
		iil = itr->second;
		u32 refcount = iil->addRefCountByIILMgr();
		if (refcount == 1)
		{
			sgTailLock->lock();
			removeFromIILListPriv(iil, rdata);
			sgTailLock->unlock();
		}
		iiltype = iil->getIILType();
		AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
		AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
		return iil;
	}
	AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
	AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
	return 0;
}


void
AosIILMgr::printHitStat()
{
	if (sgNumIILRequests == 0) return;
	int totalmiss = sgNumIILLoads + sgNumIILMiss + sgNumIILPops;
	OmnScreen << "Hit Stat: " << sgNumIILRequests << ":"
		<< sgNumIILHits << ":" << sgNumIILLoads 
		<< ":" << sgNumIILMiss << ":" << sgNumIILPops 
		<< ":" << totalmiss << ":" << 1.0 * totalmiss / sgNumIILRequests << endl;

	int lasttotal = sgLastIILLoads + sgLastIILMiss + sgLastIILPops;
	float lastratio = 1.0;
	if (sgNumIILRequests - sgLastIILRequests)
	{
		lastratio = (totalmiss - lasttotal) / (sgNumIILRequests - sgLastIILRequests);
	}

	OmnScreen << "Hit Stat: " << (sgNumIILRequests-sgLastIILRequests) << ":"
		<< (sgNumIILHits-sgLastIILHits) << ":" << (sgNumIILLoads-sgLastIILLoads)
		<< ":" << (sgNumIILMiss-sgLastIILMiss) << ":" << (sgNumIILPops-sgLastIILPops)
		<< ":" << (totalmiss-lasttotal) << ":" 
		<< lastratio << endl;

	sgLastIILRequests = sgNumIILRequests;
	sgLastIILHits = sgNumIILHits;
	sgLastIILLoads = sgNumIILLoads;
	sgLastIILMiss = sgNumIILMiss;
	sgLastIILPops = sgNumIILPops;
}


bool
AosIILMgr::createIILID(
		//const bool isNew,
		//const u64 &old_iilid,
		u64 &new_iilid,
		const u32 virtual_id,
		const AosRundataPtr &rdata)
{
	// Ketty 2012/08/08
	/*
	aos_assert_r(sgVirServerNum > 0, false);

	sgIILIDLock.lock();
	u32 vid;
	if (isNew)
	{
		vid = sgVirServerId;
		sgVirServerId += sgServerNum;
		if(sgVirServerId >= sgVirServerNum)
		{
			sgVirServerId = sgServerId;
		}
	}
	else
	{
		vid = old_iilid % sgVirServerNum;
	}
	sgIILIDLock.unlock();
	*/

	new_iilid = AosIILIDIdGen::getSelf()->nextIILId(virtual_id, rdata);
	aos_assert_r(new_iilid, false);
	return true;
}		


bool
AosIILMgr::parseIILID(
		const u64 &iilid,
		u64 &localid,
		int &vid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(sgVirServerNum > 0, false);
	localid = iilid / sgVirServerNum;
	vid = iilid % sgVirServerNum;
	
	return true;
}


// Description:
// This function adds a docid into the IIL identified
// by 'wordId'. If the IIL has not been created yet, 
// it will create it. Since it may need to create IILs, 
// the caller need to specify the IIL type and IIL 
// sort type.
//
// Parameters:
// - word: the word to which the doc is to be added.
// - len: the lengt of the word.
// - docid: the ID of the doc to be added.
// - iiltype: the IIL type if the IIL needs to be created. 
// - sorttype: the IIL sort type if the IIL needs to be created.
//
// Returns:
// The function returns true upon success. 
//
// Errors:
// - If it fails to add the word into the word hash (should 
//   never happen)
// - If it fails to fetch the IIL from the files (it may 
//   happen if file system fails or system errors).
// - If it fails to create the IIL (should never happen)

bool
AosIILMgr::deleteIILSafe(
			const AosIILObjPtr &iilobj,
	        const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);

	AosIIL * iil = (AosIIL*)(iilobj.getPtr());
	AosIILType iiltype = iil->getIILType();
	aos_assert_r(iiltype == eAosIILType_Str, false);
	AosIILStr *striil = (AosIILStr *)iil;
	bool rslt = striil->deleteIILSafe(true, rdata);

	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return rslt;
}


bool	
AosIILMgr::preQuerySafeByIIL(
		const AosIILObjPtr &iilobj,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	AosIIL * iil = (AosIIL*)(iilobj.getPtr());
	bool rslt = iil->preQuerySafe(query_context, false, rdata);
	return rslt;
}


bool	
AosIILMgr::querySafeByIIL(
		const AosIILObjPtr &iilobj,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	AosIIL * iil = (AosIIL*)(iilobj.getPtr());

	AosIILType iiltype = iil->getIILType();
	aos_assert_r(query_context, false);
	if(query_rslt)
	{
		if((iiltype == eAosIILType_Str || iiltype == eAosIILType_U64) && 
			query_context->isCheckRslt())
		{
			AosIILPtr compiil = loadIILByIDPublic(iil->getIILID(),
												  iil->getSiteid(),
												  iiltype,
												  true,
												  true,
												  rdata);
			return queryNewSafe(compiil,
								query_rslt,
								query_bitmap, 
								query_context, 
								rdata);										
	
		}
		return queryNewSafe(
			iil,
			query_rslt,
			query_bitmap, 
			query_context, 
			rdata);
	}

	AosBitmapObjPtr cur_bitmap = query_bitmap;
	AosBitmapObjPtr bitmap2 = 0;
	if(!query_rslt && query_bitmap && (!query_bitmap->isEmpty()))
	{
		bitmap2 = AosBitmapMgr::getSelf()->getBitmap();
		cur_bitmap = bitmap2;
	}

	aos_assert_r(iil, false);

	bool rslt = iil->queryNewSafe(
		query_rslt, cur_bitmap, query_context, rdata);
	if(bitmap2)
	{
		// Chen Ding, 2013/02/14
		// AosBitmapObj::countRsltAndStatic(query_bitmap, bitmap2);
		query_bitmap->andBitmap(bitmap2);
		AosBitmapMgr::getSelf()->returnBitmap(bitmap2);
	}
	return rslt;
}


bool	
AosIILMgr::queryRangeSafeByIIL(
		const AosIILPtr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	AosBitmapObjPtr cur_bitmap = query_bitmap;
	AosBitmapObjPtr bitmap2 = 0;
	if(!query_rslt && query_bitmap && (!query_bitmap->isEmpty()))
	{
		bitmap2 = AosBitmapMgr::getSelf()->getBitmap();
		cur_bitmap = bitmap2;
	}

	aos_assert_r(iil, false);
	bool rslt = iil->queryRangeSafe(
						query_rslt, 
						cur_bitmap, 
						query_context, 
						rdata);
	if(bitmap2)
	{
		// Chen Ding, 2013/02/14
		// AosBitmapObj::countRsltAndStatic(query_bitmap, bitmap2);
		query_bitmap->andBitmap(bitmap2);
		AosBitmapMgr::getSelf()->returnBitmap(bitmap2);
	}
	return rslt;
}


bool
AosIILMgr::needPopIIL(const AosIILType type)
{
	int total = AosIIL::getTotalIILs(type);
/*	if(type == eAosIILType_Docid)
	{
		total -= (AosIIL::getTotalIILs(eAosIILType_Str) 
				  + AosIIL::getTotalIILs(eAosIILType_U64)
				  + AosIIL::getTotalIILs(eAosIILType_Hit));
		return total >= 1000 || total >= (int)mMaxIILs; 
	}*/
	if (type == eAosIILType_Hit)
	{
		return total >= (2 * (int)mMaxIILs); 
	}
	return total >= (int)mMaxIILs;
}


bool
AosIILMgr::addTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIILTransPtr &trans,
		const bool needRsp,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 10/05/2011
	// As far as IILMgr is concerned, 'trans' is finished when it comes
	// to this point since IILMgr will be responsible for processing
	// the transactions, regardless of whether it requires response or
	// not, whenever appropriate. 
	//AosTinyTransPtr tiny_trans = (AosTinyTrans *)trans.getPtr();
	//AOSIILTRANSTEST_IILTRANS_RECVED(iilid, trans.getPtr());
	// Ketty 2013/02/25
	//return mIDTransMap->addTrans(iilid, siteid, tiny_trans, needRsp, rdata);
	return mIDTransMap->addTrans(iilid, siteid, trans, needRsp, rdata);
}


AosIILPtr
AosIILMgr::getIILPublic(
		const u64 &iilid, 
		const u32 siteid,
		const AosIILTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the IIL 'iilid'. If the IIL has already been
	// created, it returns the iil. Otherwise, it will create it.
	aos_assert_rr(trans, rdata, 0);
	AosIILType type = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, siteid, type, false, false, rdata);
	if (iil) return iil;

	bool needCreate = trans->needCreateIIL();
	if(!needCreate) return 0;

	AosIILType iiltype = trans->getIILType();
	bool isPersis = trans->isPersis();
	iil = createIILPublic1(iilid, siteid, iiltype, isPersis, false, rdata);
	aos_assert_r(iil, 0);
	return iil;
}


bool
AosIILMgr::procTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIDTransVectorPtr &p,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilid, rdata, false);

	//ken 2012/04/01
	//if (p->size() == 0) return true;
	aos_assert_rr(p->size() > 0, rdata, false);
	
	// Ketty 2013/02/22	temp
	rdata->setSiteid(siteid);

	AosIDTransVector::iterator itr = p->begin();
	aos_assert_rr(*itr, rdata, false);

	AosIILTransPtr iiltrans = (AosIILTrans *)(*itr).getPtr();
	AosIILPtr iil = getIILPublic(iilid, siteid, iiltrans, rdata);
	if(!iil)
	{
		return false;
	}

	//AOSIILTRANSTEST_CHECKTRANS(iilid, p);

	AosIILObjPtr iilobj = (AosIILObj *)(iil.getPtr());
	for(; itr != p->end(); itr++)
	{
		iiltrans = (AosIILTrans *)(*itr).getPtr();
		iiltrans->proc(iilobj, rdata);
		iil->addTrans(iiltrans);
	}
	//mIDTransMap->clearIDTransVector(p);

	if (mIDTransMap->checkNeedSave(iil->getTransNum()))
	{
		//OmnScreen << "iil need to save: " << iil->getIILID() << ":" 
		//	<< iil->getTransNum() << endl;
		iil->saveToFileSafe(false, false, false, rdata);
	}

	returnIILPublic(iil, rdata);
	return true;
}


bool
AosIILMgr::procOneTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIILTransPtr &trans,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	AosIILPtr iil = getIILPublic(iilid, siteid, trans, rdata);
	if(!iil) return false;
	
	AosIILObjPtr iilobj = (AosIILObj *)(iil.getPtr());
	trans->proc(iilobj, rdata);
	returnIILPublic(iil, rdata);
	return true;
}


// Ketty 2013/01/17
/*
AosDocTransPtr
AosIILMgr::getDocTrans()
{
	return mDocTransMgr->getDocTrans();
}


void
AosIILMgr::returnDocTrans(const AosDocTransPtr &doc)
{
	mDocTransMgr->returnDocTrans(doc);
}
*/


bool
AosIILMgr::finishTrans(
		const u64 &global_tid,
		const OmnString &resp_msg, 
		const AosRundataPtr &rdata) 
{
	aos_assert_r(mTransServer, false);
	
	// Ketty 2012/10/30
	OmnNotImplementedYet;
	return false;
	//return mTransServer->finishTrans(global_tid, resp_msg);
}


// Ketty 2012/11/30
/*
bool
AosIILMgr::finishTrans(
		vector<u64> &trans,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(mTransServer, false);
	mTransServer->finishTrans(trans, rdata);
	return true;
}
*/


//bool
//AosIILMgr::recoverHeaderBody(
//		const u64 &entryid,
//		const AosBuffPtr &headBuff,
//		const AosBuffPtr &bodyBuff)
//{
//	// Ketty 2013/01/15
//	OmnShouldNeverComeHere;
//	return false;
//	//AosRundataPtr rdata = OmnApp::getRundata();
//	//u32 siteid = 0;
//	//u64 iilid = AosSiteid::separateSiteid(entryid, siteid);
//	//return AosIIL::saveToFileSafeStatic(iilid, siteid, headBuff, bodyBuff, rdata);
//}


bool
AosIILMgr::procAllTrans(const AosRundataPtr &rdata)
{
	return mIDTransMap->procAllTrans(rdata);
}


AosIILPtr
AosIILMgr::getAndRemoveFirstNonDirty(
		const AosIILType type,
		bool &needSave,
		const AosRundataPtr &rdata)
{
	const static int sgTryGetNonDirty = 50;
checkHeadTail(type);
	needSave = false;

	sgTailLock->lock();
	AosIILPtr iil = mIILTail[type];
	
	if (!iil)
	{
		aos_assert_rl(!mIILHead[type], sgTailLock, 0);
	}

	int guard = sgTryGetNonDirty;
	while(guard-- > 0)
	{
		if(!iil)
		{
			OmnScreen << "!!!!!!!!!!!!! mIILTail is not long enough: " 
				<< sgTryGetNonDirty-guard << ":" 
				<< sgTotalIdleIIL[type] << endl;
			break;
		}

		int c = iil->getRefCountByIILMgr();
		aos_assert_rl(c == 0, sgTailLock, 0);

		if(!iil->isDirty())
		{
			removeFromIILListPriv(iil, rdata);
			sgTailLock->unlock();
checkHeadTail(type);
			return iil;
		}
		iil = iil->getPrevByIILMgr();
	}
checkHeadTail(type);

	mSavingLock->lock();
	int size = mIILsToSave.size();

	needSave = size;

	if (size > 3)
	{
		sgTailLock->unlock();
		mSavingLock->unlock();
		return 0;
	}
	AosIILPtr rootiil;
	u64 root_combinedid;
	guard = 10;
	iil = mIILTail[type];
	while((int)mIILsToSave.size() < guard)
	{
		if(!iil)
		{
			sgTailLock->unlock();
			mSavingLock->unlock();
			return 0;
		}
		
		if (!iil->isDirty())
		{
			removeFromIILListPriv(iil, rdata);
checkHeadTail(type);
			sgTailLock->unlock();
			mSavingLock->unlock();
			return iil;
		}
		//1. get the root iil of the iil
		//2. check if the rootiil is in the idle list
		//3. if in the idle list, lock the iil
		//4. push to save queue.

		//1. get the root iil of the iil
		if(iil->isLeafIIL())
		{
			rootiil = iil->getRootIIL();
		}
		else
		{
			rootiil = iil;
		}

		if(!rootiil)
		{
			sgTailLock->unlock();
			mSavingLock->unlock();
			OmnAlarm << "error" << enderr;
			return 0;
		}
		
		if (rootiil->isCompIIL())
		{
			// the rootiil is the comp iil, we need to find the real one.
			root_combinedid = AosSiteid::combineSiteid(rootiil->getIILID(),rootiil->getSiteid());
			IILIDHashItr itr = mIILIDHash.find(root_combinedid);

			if(itr == mIILIDHash.end())
			{
				sgTailLock->unlock();
				mSavingLock->unlock();
				OmnAlarm << "error" << enderr;
				return 0;
			}
				
			rootiil = itr->second;

			if(rootiil->getIILType() == eAosIILType_CompStr ||
			   rootiil->getIILType() == eAosIILType_CompU64)
			{
				sgTailLock->unlock();
				mSavingLock->unlock();
				OmnAlarm << "error" << enderr;
				return 0;
			}
		}

		//2. check if the rootiil is in the idle list
		if(rootiil->getRefCountByIILMgr() > 0)
		{
			iil = iil->getPrevByIILMgr();
			continue;
		}
		//3. if in the idle list, lock the iil
		rootiil->lockIIL();
		removeFromIILListPriv(rootiil, rdata);
		//4. push to save queue.
		mIILsToSave.push_back(rootiil);
		rootiil->addRefCountByIILMgr();
		needSave = true;
		iil = iil->getPrevByIILMgr();
	}
	sgTailLock->unlock();
	mSavingLock->unlock();
checkHeadTail(type);
	return 0;
}


bool
AosIILMgr::createIILPublic(
		u64 &iilid,
		const u32 siteid,
		const AosIILType iiltype,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilid == 0, rdata, false);
	
	// Ketty 2012/11/02
	int vid = mRobin->routeReq(0);
	aos_assert_r(vid >=0, false);
	bool rslt = createIILID(iilid, vid, rdata);
	aos_assert_r(rslt, 0);

	rslt = createIILPublic1(iilid, siteid, iiltype, isPersis, false, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}	


AosIILPtr
AosIILMgr::createIILPublic1(
		const u64 &iilid, 
		const u32 siteid, 
		const AosIILType iiltype, 
		const bool isPersis,
		const bool isSubiil,
		const AosRundataPtr &rdata)
{
	// This function creates an IIL and saves the IIL. If needed, it will 
	// create the comp iil, too. Note that this function will not reuse
	// the cached IILs. It creates the IILs and saves them immediately. 
	// The IILs are thrown away. 
	//
	// To prevent the same IILs being created multiple times (in a multi-thread
	// environment), it will lock the creation lock, check whether the 
	// iil has been created. 
//	static int lsTotalCreateReqs = 0;
//	static int lsTotalWaitReqs = 0;
//	static int lsTotalWaits = 0;

	AosIILType type = iiltype;
	bool isNumAlpha = false; 
	if(type == eAosIILType_NumAlpha)
	{                               
		type = eAosIILType_Str;     
		isNumAlpha = true;          
	}
	
	bool needSave = false;
	AosIILPtr iil;

	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	while(1)
	{
		AosIILType type1 = eAosIILType_Invalid;
		iil = loadIILByIDFromHashPublic(iilid,
								  siteid,
								  type1,
								  false, // isCompiil
								  true,  // iilmgrlocked
								  rdata);
		if(iil)
		{
			break;
		}
		
		// iil not found by id hash
		needSave = false;
		iil = createIILPriv(iilid, siteid, type, isPersis, needSave, rdata);
		if(iil)
		{
			break;
		}
		aos_assert_rl(needSave,mLock,0);

		// Unlock mLock
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		// Signal saving thread
//		aos_assert_r(mIILsToSave.size() > 0, 0);
		mSavingLock->lock();
		mSavingCondVar->signal();
		mSavingLock->unlock();

		// Wait a while
		sgSavingLock->lock();
		bool isTimeout = false;
		if(mIILsToSave.size())
		{
			sgSavingCondVar->timedWait(sgSavingLock, isTimeout, 0,200);
		}
		sgSavingLock->unlock();

		// lock mLock
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
	}

	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	aos_assert_r(iil,0);

	if(isNumAlpha)                                    
	{                                                 
		AosIILStr *striil = (AosIILStr *)iil.getPtr();
		striil->setNumAlpha();                        
	}

	// Ketty 2013/01/15
	// comp iil not support.
	/*
	AosIILPtr compiil; 
	if(!isSubiil && iil->haveCompIIL())
	{
		// create comp iil
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
//		compiil = createIILPriv(iilid, getCompType(type), isPersis, needSave,rdata);
		while(1)
		{
			AosIILType type1 = eAosIILType_Invalid;
			compiil = loadIILByIDFromHashPublic(iilid,
									  siteid,
									  type1,
									  true, // isCompiil
									  true,  // iilmgrlocked
									  rdata);
			if(compiil)
			{
				break;
			}
			
			// iil not found by id hash
			needSave = false;
			compiil = createIILPriv(iilid, siteid, getCompType(type), isPersis, needSave, rdata);
			if(compiil)
			{
				break;
			}
			aos_assert_rl(needSave,mLock,0);
	
			// Unlock mLock
			AOSLMTR_UNLOCK(mLockMonitor);
			AOSLMTR_FINISH(mLockMonitor);
			// Signal saving thread
			aos_assert_r(mIILsToSave.size() > 0, 0);
			mSavingLock->lock();
			mSavingCondVar->signal();
			mSavingLock->unlock();
	
			// Wait a while
			sgSavingLock->lock();
			bool isTimeout = false;
			if(mIILsToSave.size())
			{
				sgSavingCondVar->timedWait(sgSavingLock, isTimeout, 0,200);
			}
			sgSavingLock->unlock();
	
			// lock mLock
			AOSLMTR_ENTER(mLockMonitor);
			AOSLMTR_LOCK(mLockMonitor);
		}
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);

		aos_assert_r(compiil,0);

		if(isNumAlpha)
		{
			AosIILCompStr *compstriil = (AosIILCompStr *)compiil.getPtr();
			compstriil->setNumAlpha();
		}
	}
	*/

	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	u64 combinedId = iil->getIILIDForHash();//AosSiteid::combineSiteid(iil->getIILIDWithCompFlag(), siteid);
	mIILIDHash[combinedId] = iil;
	
	// Ketty 2013/01/15
	//if(compiil)
	//{
	//	u64 combinedId2 = compiil->getIILIDForHash();//AosSiteid::combineSiteid(compiil->getIILIDWithCompFlag(), siteid);
	//	mIILIDHash[combinedId2] = compiil;
	//}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	
	// Ketty 2013/01/15
	//if (compiil)
	//{
	//	returnIILPublic(compiil, rdata);
	//}
	return iil;
}


AosIILPtr
AosIILMgr::createIILPriv(
		const u64 &iilid, 
		const u32 siteid, 
		const AosIILType type, 
		const bool isPersis, 
		bool  &needSave, 
		const AosRundataPtr &rdata)
{
	// Chen Ding, 11/06/2012
	if(!AosIsValidIILType(type))
	{
		OmnAlarm << "error iil type:" << type << enderr;
		return 0;
	}
	
	int guard = 100;
	AosIILPtr iil;
	while (guard--)
	{
		needSave = false;
	
		if (!needPopIIL(type))
		{
			// We may create new IILs
			iil = createNewIILPriv(0, iilid, siteid, type, isPersis, rdata);
			aos_assert_r(iil, 0);
			iil->addRefCountByIILMgr();
			iil->setDirty(true);
			iil->setNew(true);
			//OmnScreen << "create new iil, iilid:" << iilid << endl;
			return iil;
		}

		needSave = false;
		iil = getAndRemoveFirstNonDirty(type, needSave, rdata);
		if (!iil)
		{
			if(!needSave)
			{
				// static create
				iil = createNewIILPriv(0, iilid, siteid, type, isPersis, rdata);
				aos_assert_r(iil, 0);
				iil->addRefCountByIILMgr();
				iil->setDirty(true);
				iil->setNew(true);
				return iil;
			}
			// need save
			return 0;
		}
	
		// It is guaranteed that no one holds pointers pointing to 'iil' now.
		mIILIDHash.erase(iil->getIILIDForHash());
		if (iil->isGood())
		{
			iil->resetSafe(0, iilid, siteid, true, rdata);
			iil->setDirty(true);
			iil->setNew(true);
			iil->addRefCountByIILMgr();
		
			//OmnScreen << "pop exist iil, iilid:" << iilid << endl;
			return iil;
		}

		// This is a serious problem. 
		OmnAlarm << "Found an invalid IIL: " << iil.getPtr() 
			<< ":" << iil->getDebugFlag() << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}


AosIILPtr
AosIILMgr::createNewIILPriv(
		const u64 &wordid, 
		const u64 &iilid, 
		const u32 siteid, 
		const AosIILType type, 
		const bool isPersis, 
		const AosRundataPtr &rdata)
{
	// This function creates a brand new IIL. 
	AosIILPtr iil;
	try
	{
		iil = AosIIL::staticCreateIIL(wordid, iilid, siteid, type, isPersis, "", true, rdata);
	}

	catch (...)
	{
		OmnAlarm << rdata->getErrmsg() << ". Wordid: " << wordid
			<< ". IILID: " << iilid 
			<< ". Type: " << type 
			<< ". Persistent: " << isPersis << enderr;
		return 0;
	}
	return iil;
}


bool
AosIILMgr::createIILThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	AosIILPtr iil;
	while (state == OmnThrdStatus::eActive)
	{
		mCreatedIILLock->lock();
		if (mCreatedIILs.size() == 0)
		{
			mCreatedIILCondVar->wait(mCreatedIILLock);
			mCreatedIILLock->unlock();
			continue;
		}
		// OmnScreen << "Creation List: " << mCreatedIILs.size() << endl;
		AosIILPtr iil = mCreatedIILs.front();
		mCreatedIILs.pop_front();
		mCreatedIILLock->unlock();

		if (iil->isCreationOnly() && iil->isDirty())
		{
			bool rslt = iil->saveToFileSafe(false, false, false, rdata);
			//bool rslt = iil->saveRootToFileSafe(rdata);
			if (!rslt)
			{
				OmnAlarm << "Failed saving the iil: " << iil->getIILID() << enderr;
			}
		}

		// Remove it from mIILIDHash, if needed. Normall, when an IIL is created, 
		// it is put into mCreatedIILs and mIILIDHash, with refcount to be 0.
		// After this thread finishes saving the IIL, if the refcount is still 
		// 0, it is removed from mIILIDHash. Otherwise, it does nothing.
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
		
		u64 combinedId = iil->getIILIDForHash();//AosSiteid::combineSiteid(iil->getIILIDWithCompFlag(), iil->getSiteid());
		IILIDHashItr itr = mIILIDHash.find(combinedId);
		if(itr != mIILIDHash.end())
		{
			if (!itr->second)
			{
				OmnAlarm << "Null IIL found" << iil->getIILID() << enderr;
			}
			else
			{
				if (itr->second != iil)
				{
					OmnAlarm << "IIL Incorrect" << iil->getIILID() << ":" 
						<< itr->second->getIILID() << enderr;
				}
				else
				{
					int cont = iil->getRefCountByIILMgr();
					if (cont == 0)
					{
						// The IIL may be removed from mIILID.
						mIILIDHash.erase(combinedId);
						if(iil->haveCompIIL())
						{
							u64 combinedId2 = AosSiteid::combineSiteid(iil->getCompIILID(), iil->getSiteid());
							itr = mIILIDHash.find(combinedId2);
							if(itr == mIILIDHash.end())
							{
								OmnAlarm << "can not find the compiil" << enderr;
							}
							else
							{
								AosIILPtr compiil = itr->second;
								if(!compiil)
								{
									OmnAlarm << "can not find the compiil" << enderr;
								}
								else
								{
									cont = compiil->getRefCountByIILMgr();
									if(cont == 1)
									{
										mIILIDHash.erase(combinedId2);
									}
								}
							}
						}
					}
				}
			}
		}
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
	}
	return true;
}


bool
AosIILMgr::savingThread(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	// This thread listens on the condvar 'mSavingCondVar'. 
	while (state == OmnThrdStatus::eActive)
	{
		mSavingLock->lock();
		
		if (mIILsToSave.size() <= 0)
		{
			mSavingCondVar->wait(mSavingLock);
			mSavingLock->unlock();
			continue;
		}

		AosIILPtr iil = mIILsToSave.front();
		mIILsToSave.pop_front();
		AosRundataPtr rdata = OmnApp::getRundata();
		mSavingLock->unlock();
		saveIILs(iil, true, rdata);
	}
	return true;
}


bool
AosIILMgr::saveIILs(const AosIILPtr &iil, const bool iilNeedUnlock,const AosRundataPtr &rdata)
{
	// This function is called when the IILMgr has too many unsaved
	// IILs. This function saves 'iil'. 'iil' is already removed from
	// the idle list, its reference count is at least 1. But it may
	// or may not be dirty. Note that at the time when one wants to
	// save 'iil', it was dirty. But when it comes to this point, 
	// it may have been saved by some other threads. 
	aos_assert_r(iil, false);

	bool rslt = false;	
	if (!iil->isDirty())
	{
		if(iilNeedUnlock)
		{
			iil->unlockIIL();
		}
		// It is not dirty. Do nothing.
		if(!iil->isLeafIIL() && !iil->isCompIIL())
		{
			// Ketty 2013/01/15
			//rslt = iil->finishTransSafe(rdata);
		}
		return returnIILPublic(iil, rdata);
	}
/*	// Need to find the root
	AosIILPtr rootiil = iil;
	AosIILPtr root_compiil;
	AosIILType type;
	bool need_return_root = true;
	bool need_return_comproot = true;
	if (iil->isLeafIIL())
	{
		rootiil = iil->getRootIIL();

		// Note that the root retrieved this way does not increment the refcount.
		// There is no need to return it.
		need_return_root = false;
		if(!rootiil) 
		{
			OmnAlarm << "failed to get the root iil, iilid:" 
				<< iil->getIILID() << ",type:" << AosIILType_toStr(iil->getIILType()) 
				<< enderr;
			returnIILPublic(iil, rdata, true);
			return false;
		}

		// Note that when this function is called, IILMgr was not locked. 
		// This means that it is possible another thread called this function, too, 
		// to save the same IIL. Need to check whether it is still dirty. 
		// If not, do nothing.
		if (!rootiil->isDirty())
		{
			// It is not dirty. Another thread saved the IIL.
			return returnIILPublic(iil, rdata, false);
		}
	}

	if (rootiil->isCompIIL())
	{
	 	root_compiil = rootiil;
		need_return_comproot = need_return_root;
		need_return_root = true;
		rootiil = loadIILByIDFromHashPublic(root_compiil->getIILID(), 
				type, false, false, rdata);
		if(!rootiil) 
		{
			OmnAlarm << "failed to get the root iil, iilid:" 
				<< root_compiil->getIILID() << ",type:" 
				<< AosIILType_toStr(root_compiil->getIILType()) << enderr;
			returnIILPublic(root_compiil, rdata, true);
			return false;
		}
	}

	// We found the root. Ready to save the iil. Before saving the IIL, we need
	// to process all the outstanding transactions.
	rslt = rootiil->saveToFileSafe(false, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to save IIL: ";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	if (need_return_root)
	{
		returnIILPublic(rootiil, rdata, false);
	}

	if (need_return_comproot && root_compiil) 
	{
		returnIILPublic(root_compiil, rdata, false);
	}
	if (iil != rootiil && iil != root_compiil)
	{
		returnIILPublic(iil, rdata, false);
	}
*/
	rslt = iil->saveToFileSafe(true, false, false, rdata);
	if (!rslt)
	{
		rdata->setError() << "Failed to save IIL: ";
		OmnAlarm << rdata->getErrmsg() << enderr;
	}

	bool returned;
	returnIILPublic(iil, true, false,returned, rdata, false);
	if(iilNeedUnlock)
	{
		iil->unlockIIL();
	}
	return true;
}


bool
AosIILMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
	case 0:
		 return mainThread(state, thread);

	case 1:
		 return savingThread(state, thread);

	case 2:
		 return createIILThread(state, thread);

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


// Chen Ding, 09/30/2011
bool
AosIILMgr::sanityCheckIdleListOrder(const AosIILType type)
{
return true;
	int guard = 10000;
	if (!mIILHead[type]) return true;
	AosIILPtr iil = mIILHead[type];

	bool isFromHead = true;
	u64 prev = 0;
	bool first = true;
	while (guard-- > 0)
	{
		if (!iil) return true;

		int c = iil->getRefCountByIILMgr();
		aos_assert_r(c == 0, 0);

		if (first)
		{
			first = false;
			prev = iil->getLastAddTimestamp();
		}
		else
		{
			u64 crt = iil->getLastAddTimestamp();
			if (isFromHead)
			{
				if (iil->isLastAddToHead())
				{
					aos_assert_r(prev > crt, false);
				}
				else
				{
					isFromHead = false;
				}
			}
			else
			{
				isFromHead = false;
				aos_assert_r(!iil->isLastAddToHead(), false);
				aos_assert_r(prev < crt, false);
			}
			prev = crt;
		}
		iil = iil->getNextByIILMgr();
	}

	return true;
}


bool
AosIILMgr::createRootIILPublic(
		u64 &iilid,
		const u32 siteid,
		const AosIILType iiltype,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	// This function creates an IIL and saves the IIL. If needed, it will 
	// create the comp iil, too. Note that this function will not reuse
	// the cached IILs. It creates the IILs and saves them immediately. 
	// The IILs are thrown away. 
	//
	// To prevent the same IILs being created multiple times (in a multi-thread
	// environment), it will lock the creation lock, check whether the 
	// iil has been created. 
	
	//aos_assert_rr(iilid == 0, rdata, false);
	bool rslt;
	if(iilid == 0)
	{
		// Ketty 2012/11/02
		int vid = mRobin->routeReq(0);
		aos_assert_r(vid >=0, false);
		rslt = createIILID(iilid, vid, rdata);
		aos_assert_r(rslt, 0);
	}
	AosIILType type = iiltype;
	bool isNumAlpha = false; 
	if (type == eAosIILType_NumAlpha)
	{                               
		type = eAosIILType_Str;     
		isNumAlpha = true;          
	}
	
	//mCreationLock->lock();
	if (AosIIL::staticIsIILCreated(iilid, siteid, type, rdata))
	{
		// It has already been created. 
		OmnAlarm << "IIL already created: " << iilid << ":" << iiltype << enderr;
	//	mCreationLock->unlock();
		return true;
	}

	AosIILPtr iil = createNewIILPriv(0, iilid, siteid, type, isPersis, rdata);
	aos_assert_r(iil, false);
	iil->setDirty(true);
	iil->setNew(true);

	if(isNumAlpha)                                    
	{                                                 
		AosIILStr *striil = (AosIILStr *)iil.getPtr();
		striil->setNumAlpha();                        
	}

	AosIILPtr compiil; 
	if (iil->haveCompIIL())
	{
		compiil = createNewIILPriv(0, iilid, siteid, getCompType(type), isPersis, rdata);
		aos_assert_r(compiil, false);
		compiil->setDirty(true);
		compiil->setNew(true);
		
		if(isNumAlpha)                                    
		{                                                 
			AosIILCompStr *compstriil = (AosIILCompStr *)iil.getPtr();
			compstriil->setNumAlpha();                        
		}
	}
	
	// Put the IILs into the queue
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	iil->setCreationOnly(true);
	u64 combinedId = AosSiteid::combineSiteid(iil->getIILIDWithCompFlag(), siteid);
	mIILIDHash[combinedId] = iil;
	if (compiil)
	{
		compiil->setCreationOnly(true);
		u64 combinedId2 = AosSiteid::combineSiteid(compiil->getIILIDWithCompFlag(), siteid);
		mIILIDHash[combinedId2] = compiil;
	}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	// Save the IILs. 
	//mCreatedIILLock->lock();
//	if (mCreatedIILs.size() < eMaxCreatedIILs)
//	{
//		// Put the IILs into the queue
//		mCreatedIILs.push_back(iil);
//		mCreatedIILCondVar->signal();
//		mCreatedIILLock->unlock();
//	}
//	else
//	{
		// The queue is too full. Will save them directly.
	rslt = iil->saveToFileSafe(false, false, false, rdata);
		
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mIILIDHash.erase(combinedId);
	if (compiil)
	{	
		u64 combinedId2 = AosSiteid::combineSiteid(compiil->getIILIDWithCompFlag(), siteid);
		mIILIDHash.erase(combinedId2);
		removeFromIILListPriv(compiil, rdata);
	}
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
		
//	aos_assert_rl(rslt, mCreationLock, false);
//	}
//	mCreationLock->unlock();
	return true;
}


u64
AosIILMgr::getIILID(
		const OmnString &word,
		const bool create,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 06/09/2012
	static OmnMutex lsLocalLock;

	lsLocalLock.lock();
	u64 wordid = AosWordMgr1::getSelf()->getWordId((u8 *)word.data(), word.length(), false);
	if (!wordid)
	{
		if(!create) 
		{
			lsLocalLock.unlock();
			return 0;
		}

		// Ketty 2012/08/08
		int vid = AosWordMgr1::getSelf()->getVirtualIdByWord(word.data(), word.length());
		aos_assert_rl(vid >=0, &lsLocalLock, 0);

		u64 iilid = 0;
		bool rslt = createIILID(iilid, vid, rdata);
		aos_assert_rl(rslt && iilid, &lsLocalLock, 0);
		wordid = AosWordMgr1::getSelf()->addWord(word.data(), word.length(), iilid);
		aos_assert_rl(wordid, &lsLocalLock, 0);
		if(mShowLog) OmnScreen << "create iilid, word:" << word << ", iilid:" << iilid << endl;
		lsLocalLock.unlock();
		return iilid;
	}
	lsLocalLock.unlock();

	u64 iilid;
	bool rslt = AosWordMgr1::getSelf()->getId(wordid, iilid);
	aos_assert_r(rslt, 0);
	if(mShowLog) OmnScreen << "get iilid, word:" << word << ", iilid:" << iilid << endl;
	return iilid;
}


bool	
AosIILMgr::queryNewSafe(
		const u64 &iilid,
		const u32 siteid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	// case 1: query_rslt  bitmap				: 
	//			means we get docs and check by bitmap, then set to rslt
	// case 2: query_rslt  !bitmap	check_rslt	: 
	//			means we use comp iil to check all the docs in the rslt
	// case 3: query_rslt  !bitmap	!check_rslt	: 
	//			means we get docs and set them into rslt
	// case 4: !query_rslt  bitmap (bitmap empty): 
	//			means we get docs and set them into bitmap
	// case 5: !query_rslt  bitmap (bitmap not empty): 
	//			means we get docs and check them by input bitmap. Finally we set them into output bitmap

	//1. switch which kind of IIL and function we should call
	aos_assert_r(query_context,false);
	bool isCheck = false;
	if(query_context->isCheckRslt())
	{
		isCheck = true;
	}
	
	AosIILType type = eAosIILType_Invalid;
	AosIILPtr iil = loadIILByIDPublic(iilid, siteid, type, isCheck, false, rdata);
	if(!iil)
	{
		// iil not exist
		if(query_rslt)
		{
			// case 2: query_rslt  !bitmap	check_rslt	: 
			//			means we use comp iil to check all the docs in the rslt
			query_rslt->clear();
		}
		else
		{
			aos_assert_r(query_bitmap,false);
			query_bitmap->clear();
		}
		return true;		
	}
	bool rslt = queryNewSafe(iil,
						query_rslt,
						query_bitmap, 
						query_context, 
						rdata);
	
	returnIILPublic(iil, rdata);
	return rslt;
}


bool	
AosIILMgr::queryNewSafe(
		const AosIILPtr &iil,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
//	AosIILType type = iil->getIILType();
	aos_assert_r(iil, false);
	aos_assert_r(query_context, false);
	AosBitmapObjPtr cur_bitmap = query_bitmap;
	AosBitmapObjPtr bitmap2 = 0;
	if(!query_rslt && query_bitmap && !query_bitmap->isEmpty())
	{
		bitmap2 = AosBitmapMgr::getSelf()->getBitmap();
		cur_bitmap = bitmap2;
	}

	bool rslt = iil->queryNewSafe(query_rslt, cur_bitmap, query_context, rdata);
	if(bitmap2)
	{
		// Chen Ding, 2013/02/09
		// AosBitmapObj::countRsltAndStatic(query_bitmap, bitmap2);
		query_bitmap->andBitmap(bitmap2);
	}
	return rslt;
}

bool
AosIILMgr::removeFromHash(const u64 &iilid,
		const u32 siteid,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{	
	//1. remove normal iil	
	//2. remove compiil if exist
	u64 combinedId = AosSiteid::combineSiteid(iilid, siteid);
	AOSLMTR_ENTER1(mLockMonitor,iilmgrLocked);
	AOSLMTR_LOCK1(mLockMonitor,iilmgrLocked);
	mIILIDHash.erase(combinedId);

	//2. remove compiil if exist
	AosIIL::setCompIILIdBit(combinedId);
	mIILIDHash.erase(combinedId);
	
	
	AOSLMTR_UNLOCK1(mLockMonitor,iilmgrLocked);
	AOSLMTR_FINISH1(mLockMonitor,iilmgrLocked);
	
	return true;
}

AosIILPtr
AosIILMgr::createSubIILSafe(
		const u64 &parentid,
		const u32 siteid,
		const AosIILType &iiltype, 
		const bool isPersis,
		const bool IILMgrLocked,
		const AosRundataPtr &rdata)
{
	// This function creates a new IIL for 'wordId'. This is a 
	// private function. The lock should have been locked.
	//
	// If the function created an IIL, the IIL is locked. It is
	// the caller's responsibility to unlock the IIL when it is
	// not needed.
	//
	// 05/13/2010, Chen Ding
	// This function is used to create sub-iils, too. In that case, 
	// 'word' is null. Need to handle this case. 
	u64 iilid;
	int vid = parentid % sgVirServerNum;
	aos_assert_r(vid >=0, 0);
	bool rslt = createIILID(iilid, vid, rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r(parentid != iilid, 0);

	return createIILPublic1(iilid, siteid, iiltype, isPersis, true, rdata);
}


bool	
AosIILMgr::StrAddBlockSafe(
		const OmnString &iilname,
		char * entries,
		const int size,
		const int num,
		const AosIILExecutorObjPtr &executor,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	OmnString parallel_iilname = AosIILName::composeParallelIILName(iilname);
	u64 iilid = getIILID(parallel_iilname, false, rdata);

	//felicia, 2013/03/10
	AosIILType type;
	AosIILPtr parallel_iil;
	if (iilid == 0)
	{
		//create iil
		iilid = getIILID(parallel_iilname, true, rdata);
		type = eAosIILType_BigStr;
		parallel_iil = createIILPublic1(iilid, rdata->getSiteid(), type, false, false, rdata);
	}
	else
	{
		parallel_iil = loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
		aos_assert_r(type == eAosIILType_BigStr, false);
	}
	aos_assert_r(parallel_iil, false);

	AosIILBigStrPtr parallel_iilstr = (AosIILBigStr*)parallel_iil.getPtr();
	if (parallel_iilstr->getLevel() > 1)
	{
		type = eAosIILType_BigStr;
		AosIILPtr iil;
		u64 iilid = getIILID(iilname, false, rdata);
		if (iilid == 0)
		{
			iilid = getIILID(iilname, true, rdata);
			iil = createIILPublic1(iilid, rdata->getSiteid(), type, false, false, rdata);
		}
		else
		{
			iil = loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
		}
		aos_assert_r(iil, false);
		aos_assert_r(type == eAosIILType_BigStr, false);

		AosIILBigStrPtr iilstr = (AosIILBigStr*)iil.getPtr();
		iilstr->addMergeIIL(parallel_iilstr, size, rdata);
		
		returnIILPublic(iilstr, rdata);
		parallel_iilstr->deleteIILSafe(false, rdata);
	}

	bool rslt = parallel_iilstr->addBlockSafe(entries, size, num,  false, rdata);

	returnIILPublic(parallel_iil, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool	
AosIILMgr::StrDelBlockSafe(
		const OmnString &iilname,
		char * entries,
		const int size,
		const int num,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, false);

	AosIILType type;
	AosIILPtr iil = loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
	aos_assert_r(iil, false);
	aos_assert_r(type == eAosIILType_Str, false);

	AosIILStrPtr striil = (AosIILStr*)(iil.getPtr());
	bool rslt = striil->delBlockSafe(
		entries, size, num, false, rdata);
	returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosIILMgr::StrIncBlockSafe(
		const OmnString &iilname,
		char * &entries,
		const int size,
		const int num,
		const u64 &dftValue,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	OmnString parallel_iilname = AosIILName::composeParallelIILName(iilname);
	u64 iilid = getIILID(parallel_iilname, false, rdata);
	
	//felicia, 2013/03/10
	AosIILType type;
	AosIILPtr parallel_iil;
	if (iilid == 0)
	{
		//create iil
		iilid = getIILID(parallel_iilname, true, rdata);
		type = eAosIILType_Str;
		parallel_iil = createIILPublic1(iilid, rdata->getSiteid(), type, false, false, rdata);
	}
	else
	{
		parallel_iil = 
			loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
		aos_assert_r(type == eAosIILType_Str, false);
	}
	aos_assert_r(parallel_iil, false);

	AosIILStrPtr parallel_iilstr = (AosIILStr*)parallel_iil.getPtr();
	if (parallel_iilstr->getLevel() > 1)
	{
		AosIILType type;
		AosIILPtr iil;
		iilid = getIILID(iilname, false, rdata);
		if (iilid == 0)
		{
			type = eAosIILType_Str;
			iilid = getIILID(iilname, true, rdata);
			iil = createIILPublic1(iilid, rdata->getSiteid(), type, false, false, rdata);
		}
		else
		{
			iil = loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
			aos_assert_r(type == eAosIILType_Str, false);
		}
		aos_assert_r(iil, false);

		AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
		iilstr->incMergeIIL(parallel_iilstr, size, dftValue, incType, rdata);
		
		returnIILPublic(iilstr, rdata);
		parallel_iilstr->deleteIILSafe(false, rdata);
	}


	bool rslt = parallel_iilstr->incBlockSafe(
		entries, size, num, dftValue, incType, false, rdata);

	returnIILPublic(parallel_iil, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosIILMgr::StrDecBlockSafe(
		const OmnString &iilname,
		char * &entries,
		const int size,
		const int num,
		const bool delete_flag,
		const AosIILUtil::AosIILIncType incType,
		const bool iilmgrLocked,
		const AosRundataPtr &rdata)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, false);
	
	AosIILType type;
	AosIILPtr iil = loadIILByIDPublic(iilid, rdata->getSiteid(), type, false, false, rdata);
	aos_assert_r(iil, false);
	aos_assert_r(type == eAosIILType_Str, false);

	AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	bool rslt = iilstr->decBlockSafe(
		entries, size, num, delete_flag, incType, false, rdata);
	returnIILPublic(iil, rdata);
	aos_assert_r(rslt, false);
	return true;
}


// moved from IIL.cp
// by Ketty 2012/10/29
AosDocFileMgrObjPtr
AosIILMgr::getDocFileMgr(
		const u64 &iilid, 
		u64 &local_iilid,
		const AosRundataPtr &rdata)
{
	int virtual_id = 0;
	bool rslt = parseIILID(iilid, local_iilid, virtual_id, rdata);
	aos_assert_rr(rslt, rdata, 0);

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid > 0, rdata, 0);
	aos_assert_r(virtual_id >= 0 && (u32)virtual_id < sgMaxDocFileMgrs, 0);
	
	// Chen Ding, 02/18/2012
	/*
	if (isPersis)
	{
		if (sgPersisDocFileMgrs[virtual_id]) return sgPersisDocFileMgrs[virtual_id];
	}
	else
	{
		if (sgNormDocFileMgrs[virtual_id]) return sgNormDocFileMgrs[virtual_id];
	}
	*/

	OmnString prefix;
	// if (isPersis)
	// {
	// 	prefix = "piil_";
	// 	vtid |= ePersisBitOnVidFlag;
	// }
	// else
	// {
		prefix = "iil_";
	// }
	prefix << virtual_id;

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	// Chen Ding, 2013/01/10
	u32 siteid_back = rdata->setSiteid(siteid);
	AosDfmProcPtr dfm_proc(this, false);
	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
			virtual_id, AosModuleId::eIIL, dfm_proc, rdata);
	rdata->setSiteid(siteid_back);
	if (docFileMgr) goto check;

	// Chen Ding, 03/03/2012
	// If it failed retrieving the DocFileMgr, we will create it because
	// even if this is a read-only request, the doc file manager is needed
	// by other IILs for this virtual site.
	// if(!createFlag)	return 0;
	aos_assert_r(sgDevLocation, 0);
	siteid_back = rdata->setSiteid(siteid);
	docFileMgr = vfsMgr->createDocFileMgr(
			virtual_id, AosModuleId::eIIL, dfm_proc, rdata);
	if (!docFileMgr)
	{
		OmnAlarm << "Failed creating docfilemgr. This is a serious problem!" << enderr;
		return 0;
	}

	goto check;

check:
	// if (isPersis)
	// {
	// 	if (!sgPersisDocFileMgrs[virtual_id])
	// 	{
	// 		sgPersisDocFileMgrs[virtual_id] = docFileMgr;
	// 		return docFileMgr;
	// 	}
	// 	aos_assert_r(sgPersisDocFileMgrs[virtual_id] == docFileMgr, 0);
	// }
	// else
	// {
	// 	if (!sgNormDocFileMgrs[virtual_id])
	// 	{
	// 		sgNormDocFileMgrs[virtual_id] = docFileMgr;
	// 		return docFileMgr;
	// 	}
	// 	aos_assert_r(sgNormDocFileMgrs[virtual_id] == docFileMgr, 0);
	// }
	// docFileMgr->setFileNamePrefix(prefix);
	return docFileMgr;
}


// Ketty 2012/11/01
bool
AosIILMgr::finishTrans(
		const vector<u64> &trans,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(mTransServer, false);
	mTransServer->finishTrans(trans, rdata);
	return true;
}


u64
AosIILMgr::parseLocId(const u64 loc_id, const u32 virtual_id)
{
	aos_assert_r(sgVirServerNum > 0, 0);
	
	u64 iilid = loc_id * sgVirServerNum + virtual_id; 
	
	//localid = iilid / sgVirServerNum;
	//vid = iilid % sgVirServerNum;
	return iilid;	
}

	
bool
AosIILMgr::sendResp(
		const AosIILTransPtr &trans, 
		const AosBuffPtr &resp_buff)
{
	aos_assert_r(mTransServer, false);
	
	return mTransServer->sendResp(trans.getPtr(), resp_buff);
}


// Ketty 2012/12/18
bool
AosIILMgr::svrIsUp(const AosRundataPtr &rdata, const u32 svr_id)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	/*
	vector<u32> vids;
	AosGetVirtualIds(vids, svr_id);
	for(u32 i=0; i<vids.size(); i++)
	{
		u32 virtual_id = vids[i];
		if(AosGetBackupSvrId(virtual_id) != AosGetSelfServerId())   continue;

		bool rslt = mIDTransMap->cleanBkpVirtual(rdata, virtual_id);
		aos_assert_r(rslt, false);
		
		rslt = cleanBkpVirtual(rdata, virtual_id);
		aos_assert_r(rslt, false);

	}
	return true;
	*/
}
	

// Ketty 2012/12/18
bool
AosIILMgr::cleanBkpVirtual(const AosRundataPtr &rdata, const u32 virtual_id)
{
	vector<AosIILPtr> iils;

	u32 loc_iilid;
	u64 c_vid;

	// 1. clean from mIILIDHash.
	AOSLMTR_ENTER1(mLockMonitor, false);
	AOSLMTR_LOCK1(mLockMonitor, false);
	IILIDHashItr itr = mIILIDHash.begin();
	while(itr != mIILIDHash.end())
	{
		u64 combinedId = itr->first; 

		loc_iilid = (u32)combinedId;
		c_vid = loc_iilid % sgVirServerNum;
		if(c_vid != virtual_id)
		{
			itr++;
			continue;
		}

		AosIILPtr iil = itr->second;
		iils.push_back(iil);

		itr++;
		mIILIDHash.erase(combinedId);
		
		OmnScreen << "IILMgr; clean bkp virtuals:" << virtual_id
				<< "; combineedId:" << combinedId
				<< "; local_id:" << loc_iilid
				<< endl;
	}

	// 2. clean from mIILHead and mIILTail. 
	for(u32 i=0; i<eAosIILType_Total; i++)
	{
		AosIILPtr iil = mIILHead[i];
		if(!iil)	continue;
	
		loc_iilid = iil->getIILID() / sgVirServerNum;
		c_vid = loc_iilid % sgVirServerNum;
	
		if(c_vid == virtual_id)
		{
			iils.push_back(iil);
			//mIILHead[i] = 0;
			removeFromIILListPriv(iil, rdata);
		}
	}
	/*
	for(u32 i=0; i<eAosIILType_Total; i++)
	{
		AosIILPtr iil = mIILTail[i];
		if(!iil)	continue;
		
		loc_iilid = iil->getIILID() / sgVirServerNum;
		c_vid = loc_iilid % sgVirServerNum;

		if(c_vid == virtual_id)
		{
			iils.push_back(iil);
			mIILTail[i] = 0;
		}
	}
	*/

	AOSLMTR_UNLOCK1(mLockMonitor, false);
	AOSLMTR_FINISH1(mLockMonitor, false);

	// 3. clean from mIILsToSave
	mSavingLock->lock();
	list<AosIILPtr>::iterator d_itr = mIILsToSave.begin();
	while(d_itr != mIILsToSave.end())
	{
		AosIILPtr iil = *d_itr;
		loc_iilid = iil->getIILID() / sgVirServerNum;
		c_vid = loc_iilid % sgVirServerNum;
		
		if(c_vid != virtual_id)
		{
			d_itr++;
			continue;
		}
		
		iils.push_back(iil);
		list<AosIILPtr>::iterator d_itr_tmp = d_itr;
		d_itr++;
		mIILsToSave.erase(d_itr_tmp);
	}
	mSavingLock->unlock();

	/*
	// 4. clean from mCreatedIILs.
	mCreatedIILLock->lock();
	d_itr = mCreatedIILs.begin();
	while(d_itr != mCreatedIILs.end())
	{
		iil = *d_itr;
		loc_iilid = iil->getIILID() / sgVirServerNum;
		c_vid = loc_iilid % sgVirServerNum;
		
		if(c_vid != virtual_id)
		{
			d_itr++;
			continue;
		}
		
		iils.push_back(iil);
		list<AosIILPtr>::iterator d_itr_tmp = d_itr;
		d_itr++;
		mCreatedIILs.erase(d_itr_tmp);
	}
	mCreatedIILLock->unlock();
	*/

	for (u32 i=0; i<iils.size();i++)
	{
		iils[i]->resetSafe( 0, iils[i]->getIILID(), iils[i]->getSiteid(), false, rdata);
		iils[i]->setDirty(false);
		iils[i]->setNew(true);
		//aos_assert_r(iils[i]->getRefCount() == 0, false);
		//retrunIILPublic(ill);
	}
	return true;
}


bool
AosIILMgr::startTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILMgr::commitTrans(const AosRundataPtr &rdata)
{
	procAllTrans(rdata);
	saveAllIILs(true, rdata);
	AosDfmProcPtr dfm_proc(this, false);
	AosDocFileMgrObjPtr dfm;
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	vector<u32> total_vids;
	AosGetLocalVirtualIds(total_vids);
	for(u32 i=0; i < total_vids.size(); i++)
	{
		dfm = vfsMgr->retrieveDocFileMgr(total_vids[i], AosModuleId::eIIL, dfm_proc, rdata);	
		if(dfm)
		{
			dfm->commitTrans(rdata);
		}
	}
	
	return true;
}


bool
AosIILMgr::rollbackTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



bool
AosIILMgr::checkIsIILIDUsed(
		const u64 &iilid, 
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	//return AosIIL::staticIsIILIDUsed(iilid, siteid, rdata);
	return AosIIL::staticIsIILIDUsed(iilid, rdata);
}


// Chen Ding, 2013/03/03
bool
AosIILMgr::retrieveQueryBlock(
		const AosRundataPtr &rdata, 
		const OmnString &iilname,  
		const AosQueryReqObjPtr &query)
{
	u64 iilid = getIILID(iilname, false, rdata);
	if (!iilid)
	{
		AosSetErrorU(rdata, "invalid_iilname") << ": " << iilname << enderr;
		return false;
	}

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		AosSetErrorU(rdata, "missing_siteid") << ": " << iilname << enderr;
		return false;
	}

	AosIILType type;
	AosIILPtr iil = loadIILByIDPublic(iilid, siteid, type, false, false, rdata);
	if (!iil)
	{
		AosSetErrorU(rdata, "failed_retrieve_iil") << ": " << iilname << enderr;
		return false;
	}

	return iil->retrieveQueryBlock(rdata, query);
}


bool 
AosIILMgr::retrieveNodeList(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const AosQueryReqObjPtr &query)
{
	OmnNotImplementedYet;
	return false;
}

