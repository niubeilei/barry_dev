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
#include "IILMgr/IILMgr.h"

#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "IdGen/IdGenMgr.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILMgr/JimoTable.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Sleep.h"
#include "SearchEngine/SeError.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEModules/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "SEModules/LogMgr.h"
#include "SEUtil/Docid.h"
#include "SEUtil/Siteid.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SmartDoc/SmartDoc.h"
#include "StorageMgrUtil/DevLocation.h"
#include "IILUtil/IILSave.h"
#include "Thread/Thread.h"
#include "Thread/LockMonitor.h"
#include "Thread/LockMonitorMgr.h"
#include "TransServer/TransProc.h"
#include "TransUtil/IdRobin.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util1/Wait.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"

OmnSingletonImpl(AosIILMgrSingleton,
				 AosIILMgr,
				 AosIILMgrSelf,
				"AosIILMgr");

static int sgTotalIdleIIL[eAosIILType_Total];
static bool	sgStatus[AosIILMgr::eSavingThreadNum + 1];
static int sgMaxGuard = 1;

OmnThreadPoolPtr AosIILMgr::smThreadPool = OmnNew OmnThreadPool("iil_mgr", __FILE__, __LINE__);
bool AosIILMgr::smShowLog = true;
bool AosIILMgr::smCopyDataWithThrd = true;
i64	AosIILMgr::smCopyDataWithThrdIILSize = 5000;
i64	AosIILMgr::smCopyDataWithThrdNum = 8;

// Ketty, 2013/04/10
//static AosDfmConfig sgDfmConf(AosDfmDoc::eIIL, AosDfmType::eSnapShotDfm, 
//						"IIL", AosModuleId::eIIL, 10, "gzip", false);

static AosDfmConfig sgDfmConf(AosDfmDocType::eIIL, 
						"IIL", AosModuleId::eIIL, true, 10, "gzip", false);

AosIILMgr::AosIILMgr()
:
mLock(OmnNew OmnMutex()),
mIsStopping(false),
mIsFreshing(false),
mCleanCache(false),
mSaveFreq(eDftSaveFreq),
mMaxIILs(0),
mSavingLock(OmnNew OmnMutex()),
mSavingCondVar(OmnNew OmnCondVar())
{
}


AosIILMgr::~AosIILMgr()
{
}


bool      	
AosIILMgr::start()
{
	return true;
}


bool        
AosIILMgr::stop()
{
	AOSLOCK(mLock);
	OmnScreen << "To stop IILMgr!" << endl;
	mIsStopping = true;
	AosRundataPtr rdata = OmnApp::getRundata();
	mIDTransMap->setIsStopping();
	AOSUNLOCK(mLock);

	// Ketty 2013/01/30
	procAllTrans(rdata);
	saveAllIILs(rdata);
	OmnScreen << "IILMgr stopped!" << endl;
	return true;
}


bool    
AosIILMgr::signal(const int threadLogicId)
{
	mSavingLock->lock();
	//mSavingCondVar->signal();
	mSavingCondVar->broadcastSignal();
	mSavingLock->unlock();
	return true;
}


// felicia, 2013/06/19
/*
bool    
AosIILMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
*/


bool    
AosIILMgr::checkThread111(OmnString &err, const int thrdLogicId)
{
	bool threadStatus;
	if(thrdLogicId >= 0 && thrdLogicId <= eSavingThreadNum)
	{
		mSavingLock->lock();
		threadStatus = sgStatus[thrdLogicId];
		sgStatus[thrdLogicId] = false;
		mSavingLock->unlock();
	}
	else
	{
		OmnAlarm << "Invalid thread id ! " << enderr;
	}
	return threadStatus;
}


bool
AosIILMgr::config(const AosXmlTagPtr &theconfig) 
{
	aos_assert_r(theconfig, false);
	AosIIL::staticInit(theconfig);
	AosIILStr::staticInit(theconfig);

	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;

	smShowLog = config->getAttrBool(AOSCONFIG_SHOWLOG, true);

	// IIL Configurations are defined in the subtag AOSCONFIG_IILMGR
	mMaxIILs = config->getAttrU64(AOSCONFIG_MAX_IILS, 0);
	aos_assert_r(mMaxIILs > 0, false);
	OmnScreen << "Max IILs: " << mMaxIILs << endl;

	for (int i=0; i<eAosIILType_Total; i++)
	{
		sgTotalIdleIIL[i] = 0;
	}

	AosXmlTagPtr iil_map = config->getFirstChild("iil_map");
	aos_assert_r(iil_map, false);
	AosIDTransMapCallerPtr thisptr(this, false);
	mIDTransMap = OmnNew AosIDTransMap(thisptr, iil_map);

	OmnThreadedObjPtr thisptr2(this, false);
	mThread = OmnNew OmnThread(thisptr2, "IILMgrMainThrd", 0, true, true, __FILE__, __LINE__);
	sgStatus[0] = false;
	mThread->start();

	OmnThreadPtr thrd;
	for (int i=0; i<eSavingThreadNum; i++)
	{
		thrd = OmnNew OmnThread(thisptr2, "IILMgrSavingThrd", i+1, true, true, __FILE__, __LINE__);
		sgStatus[i+1] = false;
		mSavingThreads.push_back(thrd);
	}

	for (u32 i=0; i<mSavingThreads.size(); i++)
	{
		mSavingThreads[i]->start();
	}

	mVirServerNum = AosGetNumCubes();
	mRobin = OmnNew AosIdRobin();
	return true;
}


bool
AosIILMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	int thread_id = thread->getLogicId();
	if (thread_id == 0)
	{
		return mainThread(state, thread);
	}

	if (thread_id > 0 && thread_id <= eSavingThreadNum)
	{
		return savingThread(state, thread, thread_id);
	}

	//felicia, 2013/06/19
	/*switch (thread->getLogicId())
	{
	case 0:
		 return mainThread(state, thread);

	case 1:
		 return savingThread(state, thread);

	default:
		 break;
	}
	*/
	OmnShouldNeverComeHere;
	return false;
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
	//Gavin 2015/09/23                 
	rdata->setJPID(AOSJPID_SYSTEM); 
	rdata->setJSID(AOSJSID_SYSTEM); 
	rdata->setAppname(AOSAPPNAME_SYSTEM);
	rdata->setUserid(AOSDOCID_IILMGRROOT);

	while (state == OmnThrdStatus::eActive)
	{
		//OmnSleep(mSaveFreq);
		int index = mSaveFreq;
		while(index--)
		{
			OmnSleep(1);
			mSavingLock->lock();
			sgStatus[0] = true;
			mSavingLock->unlock();
		}

		if (mIsStopping || !mIDTransMap)
		{
			break;
		}

		if (mIDTransMap->needProcQueue())
		{
			mIDTransMap->procQueues(rdata);
			continue;
		}

		if (mIDTransMap->needProcTrans())
		{
			mIDTransMap->procTrans(rdata);
			continue;
		}

		if (mIDTransMap->isFree())
		{
			mIDTransMap->postProc(rdata);
			continue;				
		}
	}

	return true;
}


bool
AosIILMgr::savingThread(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread,
		const u32 thread_id)
{
	// This thread listens on the condvar 'mSavingCondVar'. 
	while (state == OmnThrdStatus::eActive)
	{
		mSavingLock->lock();
		
		if (mSavingList.size() <= 0)
		{
			mSavingCondVar->wait(mSavingLock);
			sgStatus[thread_id] = true;
			mSavingLock->unlock();
			continue;
		}

		AosIILObjPtr iilobj = mSavingList.front();
		mSavingList.pop_front();
		mSavingLock->unlock();

		AosRundataPtr rdata = OmnApp::getRundata();
		//Gavin 2015/09/23                 
		rdata->setJPID(AOSJPID_SYSTEM); 
		rdata->setJSID(AOSJSID_SYSTEM); 
u64 start_time = OmnGetTimestamp();
		saveIIL(iilobj, rdata);
u64 cost = OmnGetTimestamp() - start_time;
if (cost > 1000000)
{
	OmnScreen << "save iilobj const time:" << iilobj->getIILID() << ":" << AosTimestampToHumanRead(cost) << endl;
}
	}
	return true;
}


bool
AosIILMgr::saveAllIILs(const AosRundataPtr &rdata)
{
	for (int i = 0;i < eMaxWaitForForceSave;i++)
	{
		if (mSavingList.size() <= 0) break;
		OmnSleep(1);
	}

	// IMPORTANT: This will erase all the IILs.
	AOSLOCK(mLock);
	AosIILObjPtr iilobj;
	IILIDHashItr itr = mIILIDHash.begin();
	while(itr!=mIILIDHash.end())
	{
		iilobj = itr->second;
		if (!iilobj->isChildIIL())
		{
			u32 refcount = iilobj->addRefCountByIILMgr();
			if (refcount == 1)
			{
				removeIILFromIdleListPriv(iilobj, rdata);
			}

			AOSUNLOCK(mLock);
			iilobj->lockIIL();
			saveIIL(iilobj, rdata);
			AOSLOCK(mLock);
		}
		itr++;
	}
	AOSUNLOCK(mLock);

	return true;
}


bool
AosIILMgr::saveIIL(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata)
{
	// This function is called when the IILMgr has too many unsaved
	// IILs. This function saves 'iilobj'. 'iilobj' is already removed from
	// the idle list, its reference count is at least 1. But it may
	// or may not be dirty. Note that at the time when one wants to
	// save 'iilobj', it was dirty. But when it comes to this point, 
	// it may have been saved by some other threads. 
	aos_assert_r(iilobj, false);

	rdata->setSiteid(iilobj->getSiteid());
	if (iilobj->isDirty())
	{
		bool rslt = iilobj->saveToFilePriv(rdata);
		if (!rslt)
		{
			rdata->setError() << "Failed to save IIL: " << iilobj->getIILID();
			OmnAlarm << rdata->getErrmsg() << enderr;
		}
	}

	bool returned = true;
	iilobj->unlockIIL();
	return returnIILPublic(iilobj, returned, false, rdata);
}


// Chen Ding, 2013/05/26
bool
AosIILMgr::removeIILFromIdleListPriv(
		const AosIILObjPtr &iilobj, 
		const AosRundataPtr &rdata)
{
	// IMPORTANT: In order to avoid locking, it is important
	// for this function not to call other functions that may
	// potentially lock the iilobj or AosIILMgrSelf.
	//
	// This function is a private function. It assumes 'mLock' 
	// is locked.
	// 
	// Chen Ding, 10/15/2011
	// Need to check whether the IIL is 'creation only', which happens
	// when an IIL is first needed but not created yet. In that case, 
	// the IIL may not be put in the idle list. This function needs
	// to check whether the iilobj is 'creation only'. If yes, it should not
	// remove from the idle list.

	AosIILType type = iilobj->getIILType();		// Safe
	AosIILObjPtr prev = iilobj->getPrevByIILMgr();		// Safe
	AosIILObjPtr next = iilobj->getNextByIILMgr();		// Safe
	if (next) next->setPrevByIILMgr(prev);			// Safe
	if (prev) prev->setNextByIILMgr(next);			// Safe
	iilobj->resetPtrsByIILMgr();
	if (iilobj == mIILHead[type])
	{
		mIILHead[type] = next;
	}

	// Chen Ding, 2013/01/16
	if (iilobj == mIILTail[type])
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
	if (sgTotalIdleIIL[type] < 0)
	{
		AosSetErrorU(rdata, "total_iil_incorrect")
			<< type << ":" << sgTotalIdleIIL[type] << enderr;
		return false;
	}
	return true;
}


bool
AosIILMgr::checkHeadTailLocked(const AosIILType &type) const
{
return true;
	if (!mIILHead[type] || !mIILTail[type]) 
	{
		aos_assert_r(!mIILHead[type] && !mIILTail[type], false);
		return true;
	}

	// Check from head to tail
	int num = 0;
	int c = 0;
	AosIILObjPtr iilobj = mIILHead[type];
	while (iilobj->getNextByIILMgr() && num < sgTotalIdleIIL[type])
	{
		num++;
		c = iilobj->getRefCountByIILMgr();
		aos_assert_r(c==0, false);

		iilobj = iilobj->getNextByIILMgr();
	}
	num++;
	aos_assert_r(sgTotalIdleIIL[type] == num, false);
	aos_assert_r(iilobj == mIILTail[type], false);

	int nn = 0;
	iilobj = mIILTail[type];
	while (iilobj->getPrevByIILMgr() && nn < sgTotalIdleIIL[type])
	{
		nn++;
		c = iilobj->getRefCountByIILMgr();
		aos_assert_r(c==0, false);
		iilobj = iilobj->getPrevByIILMgr();
	}
	nn++;
	aos_assert_r(iilobj == mIILHead[type], false);
	aos_assert_r(nn == num, false);
	return true;
}


bool
AosIILMgr::returnIILPublic(
		const AosIILObjPtr &iilobj,
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	if (iilobj->isParentIIL())
	{
		//return all the subiil
		bool subreturned = false;
		iilobj->returnSubIILsSafe(subreturned, returnHeader, rdata);
	}
	
	AOSLOCK(mLock);
	bool rslt = returnIILPriv(iilobj, returned, returnHeader, rdata);
	AOSUNLOCK(mLock);

	return rslt;
}


bool
AosIILMgr::returnIILPriv(
		const AosIILObjPtr &iilobj,
		bool &returned,
		const bool returnHeader,
		const AosRundataPtr &rdata)
{
	// Someone finished the use of 'iilobj'. This function puts
	// 'iilobj' in its corresponding iilobj queue. 
	//
	// IMPORTANT: In order to avoid locking, it is important
	// for this function not to call other functions that may
	// potentially lock the iilobj or AosIILMgrSelf.
	
	returned = false;
	aos_assert_r(iilobj, false);
	
	u32 cc = iilobj->removeRefCountByIILMgr();			// Safe
	if (cc > 0) 
	{
		return true;
	}
	returned = true;
	AosIILType type = iilobj->getIILType();
	sgTotalIdleIIL[type]++;

	if (returnHeader)
	{
		if (!mIILHead[type])
		{
			// The iilobj list is empty
			aos_assert_r(!mIILTail[type], false);
			aos_assert_r(sgTotalIdleIIL[type] == 1, false);

			iilobj->resetPtrsByIILMgr();							// Safe
			mIILHead[type] = iilobj;
			mIILTail[type] = iilobj;
		}
		else
		{
			// The iilobj list is not empty
			iilobj->setPrevByIILMgr(0);					// Safe
			iilobj->setNextByIILMgr(mIILHead[type]);	// Safe
			mIILHead[type]->setPrevByIILMgr(iilobj);	// Safe
			mIILHead[type] = iilobj;
		}
	}
	else
	{
		if (!mIILTail[type])
		{
			// The iilobj list is empty
			aos_assert_r(!mIILHead[type], false);
			aos_assert_r(sgTotalIdleIIL[type] == 1, false);

			iilobj->resetPtrsByIILMgr();							// Safe
			mIILHead[type] = iilobj;
			mIILTail[type] = iilobj;
		}
		else
		{
			// The iilobj list is not empty
			iilobj->setPrevByIILMgr(mIILTail[type]);	// Safe
			iilobj->setNextByIILMgr(0);				// Safe
			mIILTail[type]->setNextByIILMgr(iilobj);	// Safe
			mIILTail[type] = iilobj;
		}

	}

	if (!mIILHead[type] || !mIILTail[type]) 
	{
		if (sgTotalIdleIIL[type] != 0)
		{
			OmnAlarm << "Internal error: " << sgTotalIdleIIL[type] << enderr;
			return false;
		}
	}
	return true;
}


AosIILObjPtr
AosIILMgr::loadIILPublic(
		const u64 &iilid,
		const u32 siteid,
		const u64 &snap_id,
		AosIILType &iiltype,
		const AosRundataPtr &rdata) 
{
	AosIILObjPtr iilobj = getIILFromHashPublic(iilid, siteid, iiltype, rdata);
	if (iilobj)
	{
		switchSnapShot(iilobj, snap_id, rdata);
		return iilobj;
	}

	bool rslt = AosIIL::staticGetIILType(iiltype, iilid, snap_id, rdata);
	aos_assert_r(rslt, 0);

	if (iiltype == eAosIILType_Invalid)
	{
		// The IIL type is incorrect. Just return.
		return 0;
	}

	u64 combinedId = AosSiteid::combineSiteid(iilid, siteid);
	int guard = 0;
	bool force_create_new = false;

	while (true)
	{
		AOSLOCK(mLock);
		iilobj = getIILFromHashPriv(iilid, siteid, iiltype, rdata);
		if (iilobj)
		{
			AOSUNLOCK(mLock);
			switchSnapShot(iilobj, snap_id, rdata);
			return iilobj;
		}
		
		iilobj = getNoneUsedIILPriv(iiltype, force_create_new, rdata);
		if (iilobj)
		{
			// Ken Lee, 2014/03/18
			iilobj->addRefCountByIILMgr();
			mIILIDHash[combinedId] = iilobj;

			iilobj->lockIIL();
			AOSUNLOCK(mLock);

			iilobj->resetPriv(0, 0, 0, rdata);
			iilobj->setSnapShotIdPriv(snap_id);
			iilobj->setSiteid(siteid);
			rslt = iilobj->loadFromFilePriv(iilid, rdata);
			iilobj->unlockIIL();
			aos_assert_r(rslt, 0);

			return iilobj;
		}
		AOSUNLOCK(mLock);

		// For some reason, it failed loading the IIL. Give it another try
		// to see whether it can load the IIL.
		OmnMsSleep(20);
		guard++;
		if (guard % 50 == 0)
		{
			OmnScreen << "try load iilobj too much times" << endl;
		}
		if (guard >= sgMaxGuard)
		{
			force_create_new = true;
		}
	}

	OmnAlarm << "Failed retrieve the IIL: " << iilid
		<< ", type: " << iiltype << enderr;
	return 0;
}
		

AosIILObjPtr
AosIILMgr::getNoneUsedIILPriv(
		const AosIILType &iiltype,
		const bool force_create_new,
		const AosRundataPtr &rdata)
{
	AosIILObjPtr iilobj;
	if (!needPopIIL(iiltype) || force_create_new)
	{
		// We may create new IILs
		iilobj = createNewIIL(iiltype, rdata);
	}
	else
	{
		// Need to pop some existing IILs
		bool needSave = false; 
		iilobj = getAndRemoveFirstNonDirtyLocked(iiltype, needSave, rdata);
		if (!iilobj)
		{
			if (!needSave)
			{
				//save thread is too busy. need wait for a while.
				//OmnScreen << "ken add flag, ???????????????????" << endl;
				// create static
				//iilobj = createNewIIL(iiltype, rdata);
			}
			else
			{
				mSavingLock->lock();
				//mSavingCondVar->signal();
				mSavingCondVar->broadcastSignal();
				mSavingLock->unlock();
			}
		}
		else
		{
			// It is guaranteed that no one holds pointers pointing to 'iilobj' now.
			u64 combinedId = iilobj->getIILIDForHash();
			mIILIDHash.erase(combinedId);
			// Ken Lee, 2014/03/18
			//iilobj->resetSafe(0, 0, 0, rdata);
		}
	}
	return iilobj;
}


AosIILObjPtr
AosIILMgr::getIILFromHashPublic(
		const u64 &iilid,
		const u32 siteid,
		AosIILType &iiltype,
		const AosRundataPtr &rdata) 
{
	AOSLOCK(mLock);
	AosIILObjPtr iilobj = getIILFromHashPriv(iilid, siteid, iiltype, rdata);
	AOSUNLOCK(mLock);
	return iilobj;
}


AosIILObjPtr
AosIILMgr::getIILFromHashPriv(
		const u64 &iilid,
		const u32 siteid,
		AosIILType &iiltype,
		const AosRundataPtr &rdata) 
{
	AosIILObjPtr iilobj;
	u64 combinedId = AosSiteid::combineSiteid(iilid, siteid);
	IILIDHashItr itr = mIILIDHash.find(combinedId);
	if (itr != mIILIDHash.end())
	{
		iilobj = itr->second;
		u32 refcount = iilobj->addRefCountByIILMgr();
		if (refcount == 1)
		{
			removeIILFromIdleListPriv(iilobj, rdata);
		}
		iiltype = iilobj->getIILType();
	}
	return iilobj;
}


bool
AosIILMgr::removeIILFromHashPublic(
		const u64 &iilid,
		const u32 siteid,
		const AosRundataPtr &rdata)
{	
	u64 combinedId = AosSiteid::combineSiteid(iilid, siteid);
	AOSLOCK(mLock);
	mIILIDHash.erase(combinedId);
	AOSUNLOCK(mLock);
	return true;
}


bool
AosIILMgr::createIILID(
		u64 &new_iilid,
		const u32 virtual_id,
		const AosRundataPtr &rdata)
{
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
	aos_assert_r(mVirServerNum > 0, false);
	localid = iilid / mVirServerNum;
	vid = iilid % mVirServerNum;
	
	return true;
}


bool
AosIILMgr::needPopIIL(const AosIILType type)
{
	i64 total = AosIIL::getTotalIILs(type);
	if (type == eAosIILType_Hit)
	{
		return total >= (4 * (i64)mMaxIILs); 
	}
	if (type == eAosIILType_Str || type == eAosIILType_U64)
	{
		return total >= (2 * (i64)mMaxIILs); 
	}
	return total >= (i64)mMaxIILs;
}


bool
AosIILMgr::addTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIILTransPtr &trans,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 10/05/2011
	// As far as IILMgr is concerned, 'trans' is finished when it comes
	// to this point since IILMgr will be responsible for processing
	// the transactions, regardless of whether it requires response or
	// not, whenever appropriate. 
	if (mIsStopping)
	{
		OmnAlarm << "is Stopping! should never add trans!" << enderr;
		return false;
	}

	while (mIsFreshing)
	{
		OmnMsSleep(200);	
	}

	aos_assert_r(mIDTransMap, false);	
	return mIDTransMap->addTrans(iilid, siteid, trans, rdata);
}


AosIILObjPtr
AosIILMgr::getIILPublic(
		const u64 &iilid, 
		const u32 siteid,
		const u64 snap_id, 
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	AosIILType type = eAosIILType_Invalid;
	AosIILObjPtr iilobj = loadIILPublic(iilid, siteid, snap_id, type, rdata);
	if (iilobj) return iilobj;

	// This means the IIL has not been created yet. 
	//bool needCreate = trans->needCreateIIL();
	//if (!needCreate) return 0;

	//type = trans->getIILType();
	iilobj = createIILPublic(iilid, siteid, snap_id, iiltype, false, rdata);
	aos_assert_r(iilobj, 0);
	return iilobj;
}


AosIILObjPtr
AosIILMgr::getIILPublic(
		const u64 &iilid, 
		const u32 siteid,
		const AosIILTransPtr &trans, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the IIL 'iilid'. If the IIL has already been
	// created, it returns the iilobj. Otherwise, it will create it.
	aos_assert_r(trans, 0);

	u64 snap_id = trans->getSnapshotId();
	
//Linda
AosTransType::E type1 = trans->getType();
if (type1 == AosTransType::eStrBatchInc 
		|| type1 == AosTransType::eStrBatchAdd
		|| type1 == AosTransType::eU64BatchAdd)
{
//aos_assert_r(snap_id, 0);
}

	AosIILType type = eAosIILType_Invalid;
	AosIILObjPtr iilobj = loadIILPublic(iilid, siteid, snap_id, type, rdata);
	if (iilobj) return iilobj;

	// This means the IIL has not been created yet. 
	bool needCreate = trans->needCreateIIL();
	if (!needCreate) return 0;

	type = trans->getIILType();
	bool isPersis = trans->isPersis();
	iilobj = createIILPublic(iilid, siteid, snap_id, type, isPersis, rdata);
	aos_assert_r(iilobj, 0);
	return iilobj;
}
	

bool
AosIILMgr::checkNeedProc(
		const AosIILTransPtr &trans,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans, false);

	if (trans->isNeedResp() || trans->needProc())
	{
		return true;
	}

	OmnString iilname = trans->getIILName();
	u64 iilid = trans->getIILID(rdata);
	if (isSpecialIILName(iilname) || isSpecialIILID(iilid))
	{
		return true;
	}

	return false;
}


bool
AosIILMgr::isSpecialIILName(const OmnString &iilname)
{
	if (!mCleanCache) return false;
	if (iilname == "") return false;

	static bool sgSetInit = false;
	static set<OmnString> sgSet;
	static OmnMutexPtr sgLock = OmnNew OmnMutex(); 
	sgLock->lock();
	if (!sgSetInit)
	{
		sgSet.insert("_zt1p");
		sgSet.insert("_zt19_zky_ctime");
		sgSet.insert("_zt19_zky_ctmepo__d");
		sgSet.insert("_zt19_zky_docid");
		sgSet.insert("_zt19_zky_objid");
		sgSet.insert("_zt19_zky_otype");
		sgSet.insert("_zt19_zky_pctrs");
		sgSet.insert("_zt19_zky_uname");

		sgSetInit = true;
	}
	sgLock->unlock();

	if (sgSet.find(iilname) != sgSet.end())
	{
		return true;
	}

	return false;
}


bool
AosIILMgr::isSpecialIILID(const u64 &iilid)
{
	if (!mCleanCache) return false;
	if (iilid == 0) return false;

	static bool sgSetInit = false;
	static set<u64> sgSet;
	static OmnMutexPtr sgLock = OmnNew OmnMutex(); 
	sgLock->lock();
	if (!sgSetInit)
	{
		sgSet.insert(500);
		sgSet.insert(1600);
		sgSet.insert(1601);
		sgSet.insert(1602);
		sgSet.insert(1603);
		sgSet.insert(1604);
		sgSet.insert(1605);
		sgSet.insert(1606);
		sgSet.insert(1607);
		sgSet.insert(1608);
		sgSet.insert(1609);
		sgSet.insert(1610);
		sgSet.insert(1611);

		sgSetInit = true;
	}
	sgLock->unlock();

	if (sgSet.find(iilid) != sgSet.end())
	{
		return true;
	}

	return false;
}


bool
AosIILMgr::procTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIDTransVectorPtr &p,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);
	aos_assert_r(p->size() > 0, false);
	
	// Ketty 2013/02/22 temp.
	rdata->setSiteid(siteid);

	//Linda, 2014/03/28
    OmnMutexPtr lock = mLockPool.get(iilid, mLockGen);
	lock->lock();

	AosIDTransVector::iterator itr = p->begin();
	aos_assert_r(*itr, false);
	AosIILTransPtr iiltrans = *itr;
	AosIILObjPtr iilobj = getIILPublic(iilid, siteid, iiltrans, rdata);
	if (!iilobj) 
	{
		lock->unlock();
		return false;
	}

	OmnString iilname = iiltrans->getIILName();

	//AosIIL * iil = dynamic_cast<AosIIL *>(iilobj.getPtr());
	//for (int i=0; itr != p->end(); itr++, i++)
	for (; itr != p->end(); itr++)
	{
		iiltrans = *itr;
		aos_assert_rl(iiltrans, lock, false);

		iiltrans->procTrans(iilobj, rdata);
	
		//iilobj->addTrans(iiltrans);
		AosTransId gid = iiltrans->getTransId();
		iilobj->addTrans(gid);
		//if (i >= 200)
		//{
		//	iilobj->saveToFileSafe(rdata);
		//	i = 0;
		//}
	}

	if (mIDTransMap->checkNeedSave(iilobj->getTransNum()) 
		|| isSpecialIILName(iiltrans->getIILName())
		|| isSpecialIILID(iiltrans->getIILID(rdata)))
	{
		//OmnScreen << "iilobj need to save: " << iilobj->getIILID() << ":" 
		//	<< iilobj->getTransNum() << endl;
		iilobj->saveToFileSafe(rdata);
	}

	returnIILPublic(iilobj, rdata);
	lock->unlock();
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

	//Linda, 2014/03/28
    OmnMutexPtr lock = mLockPool.get(iilid, mLockGen);
	lock->lock();
	AosIILObjPtr iilobj = getIILPublic(iilid, siteid, trans, rdata);
	if (!iilobj) 
	{
		lock->unlock();
		return false;
	}

	trans->procTrans(iilobj, rdata);
	returnIILPublic(iilobj, rdata);
	lock->unlock();
	return true;
}


bool
AosIILMgr::procAllTrans(const AosRundataPtr &rdata)
{
	aos_assert_r(mIDTransMap, false);
	return mIDTransMap->procAllTrans(rdata);
}


AosIILObjPtr
AosIILMgr::getAndRemoveFirstNonDirtyLocked(
		const AosIILType type,
		bool &needSave,
		const AosRundataPtr &rdata)
{
checkHeadTailLocked(type);
	needSave = false;

	AosIILObjPtr iilobj = mIILTail[type];
	if (!iilobj)
	{
		aos_assert_r(!mIILHead[type], 0);
	}

	const static int sgTryGetNonDirty = 50;
	int guard = sgTryGetNonDirty;
	while (guard--)
	{
		if (!iilobj)
		{
			//OmnScreen << "!!!!!!!!!!!!! mIILTail is not long enough: " 
			//	<< sgTryGetNonDirty-guard << ":" 
			//	<< sgTotalIdleIIL[type] << endl;
			break;
		}

		int c = iilobj->getRefCountByIILMgr();
		if (c != 0)
		{
			OmnAlarm << "Internal error: " << c << enderr;
			removeIILFromIdleListPriv(iilobj, rdata);
checkHeadTailLocked(type);
			AosIILObjPtr iil_new = mIILTail[type];
			aos_assert_r(iilobj != iil_new, 0);
			iilobj = iil_new;
			continue;
		}

		if (!iilobj->isDirty())
		{
			removeIILFromIdleListPriv(iilobj, rdata);
checkHeadTailLocked(type);
			return iilobj;
		}
		iilobj = iilobj->getPrevByIILMgr();
	}
checkHeadTailLocked(type);

	mSavingLock->lock();
	int size = mSavingList.size();
	mSavingLock->unlock();
	if (size >= sgTryGetNonDirty)
	{
		return 0;
	}

	AosIILObjPtr rootiil;
	iilobj = mIILTail[type];
	guard = sgTryGetNonDirty - size;
	while (guard > 0)
	{
		if (!iilobj)
		{
			return 0;
		}

		//1. get the root iilobj of the iilobj
		//2. check if the rootiil is in the idle list
		//3. if in the idle list, lock the iilobj
		//4. push to save queue.

		//1. get the root iilobj of the iilobj
		if (iilobj->isChildIIL())
		{
			rootiil = iilobj->getRootIIL();
		}
		else
		{
			rootiil = iilobj;
		}

		if (!rootiil)
		{
			OmnAlarm << "error" << enderr;
			return 0;
		}
		
		iilobj = iilobj->getPrevByIILMgr();
		//2. check if the rootiil is in the idle list
		if (rootiil->getRefCountByIILMgr() > 0)
		{
			continue;
		}

		//3. if in the idle list, lock the iilobj
		rootiil->lockIIL();
		removeIILFromIdleListPriv(rootiil, rdata);
		//4. push to save queue.
		
		mSavingLock->lock();
		mSavingList.push_back(rootiil);
		guard--;
		mSavingLock->unlock();

		rootiil->addRefCountByIILMgr();
		needSave = true;
	}

checkHeadTailLocked(type);
	return 0;
}


AosIILObjPtr
AosIILMgr::createIILPublic(
		const u64 &iilid, 
		const u32 siteid, 
		const u64 &snap_id,
		const AosIILType iiltype, 
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	// This function creates an IIL and saves the IIL. If needed, it will 
	// create the comp iilobj, too. Note that this function will not reuse
	// the cached IILs. It creates the IILs and saves them immediately. 
	// The IILs are thrown away. 
	//
	// To prevent the same IILs being created multiple times (in a multi-thread
	// environment), it will lock the creation lock, check whether the 
	// iilobj has been created. 

	AosIILType type = iiltype;
	bool isNumAlpha = false; 
	if (type == eAosIILType_NumAlpha)
	{                               
		type = eAosIILType_Str;     
		isNumAlpha = true;          
	}
	
	AosIILObjPtr iilobj;
	u64 combinedId = AosSiteid::combineSiteid(iilid, siteid);
	int guard = 0;
	bool force_create_new = false; 
	while (true)
	{
		AOSLOCK(mLock);
		iilobj = getIILFromHashPriv(iilid, siteid, type, rdata);
		if (iilobj)
		{
			AOSUNLOCK(mLock);
			switchSnapShot(iilobj, snap_id, rdata);
			return iilobj;
		}
		
		iilobj = getNoneUsedIILPriv(type, force_create_new, rdata);
		if (iilobj)
		{
			// Ken Lee, 2014/03/18
			iilobj->addRefCountByIILMgr();
			mIILIDHash[combinedId] = iilobj;
			iilobj->lockIIL();
			AOSUNLOCK(mLock);

			iilobj->resetPriv(0, 0, 0, rdata);

			iilobj->setDirty(true);
			iilobj->setNew(true);
			iilobj->setIILID(iilid);
			iilobj->setSiteid(siteid);
			iilobj->setPersis(isPersis);
			iilobj->setSnapShotIdPriv(snap_id);
			if (isNumAlpha)
			{                                                 
				AosIILStr *striil = dynamic_cast<AosIILStr *>(iilobj.getPtr());
				striil->setNumAlpha();
			}
			iilobj->unlockIIL();
			return iilobj;
		}
		AOSUNLOCK(mLock);
		// For some reason, it failed loading the IIL. Give it another try
		// to see whether it can load the IIL.
		OmnMsSleep(20);
		guard++;
		if (guard % 50 == 0)
		{
			OmnScreen << "try load iilobj too much times" << endl;
		}
		if (guard >= sgMaxGuard)
		{
			force_create_new = true;
		}
	}

	OmnAlarm << "Failed retrieve the IIL: " << iilid
		<< ", type: " << iiltype << enderr;
	return 0;
}


AosIILObjPtr
AosIILMgr::createNewIIL(
		const AosIILType type, 
		const AosRundataPtr &rdata)
{
	// This function creates a brand new IIL. 
	AosIILObjPtr iilobj;
	try
	{
		iilobj = AosIIL::staticCreateIIL(type, rdata);
	}

	catch (...)
	{
		OmnAlarm << rdata->getErrmsg() << ". Type: " << type << enderr;
		return 0;
	}

	return iilobj;
}


u64
AosIILMgr::getIILID(
		const OmnString &word,
		const bool create,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 06/09/2012
	static OmnMutexPtr sgLocalLock = OmnNew OmnMutex();

	sgLocalLock->lock();
	u64 wordid = AosWordMgr1::getSelf()->getWordId(
		(u8 *)word.data(), word.length(), false);
	if (!wordid)
	{
		if (!create) 
		{
			OmnScreen << "Warning: IIL not found:" << word << endl;
			sgLocalLock->unlock();
			return 0;
		}

		// Ketty 2012/08/08
		int vid = AosGetCubeId(word);
		aos_assert_rl(vid >=0, sgLocalLock, 0);

		u64 iilid = 0;
		bool rslt = createIILID(iilid, vid, rdata);
		aos_assert_rl(rslt && iilid, sgLocalLock, 0);
		
		wordid = AosWordMgr1::getSelf()->addWord(
			rdata, word.data(), word.length(), iilid);
		aos_assert_rl(wordid, sgLocalLock, 0);

		sgLocalLock->unlock();
		return iilid;
	}

	sgLocalLock->unlock();

	u64 iilid;
	bool rslt = AosWordMgr1::getSelf()->getId(wordid, iilid);
	aos_assert_r(rslt, 0);
		
	return iilid;
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
	aos_assert_r(query_context, false);
	query_context->setIILType(iilobj->getIILType());
	AosBitmapObjPtr cur_bitmap = query_bitmap;
	AosBitmapObjPtr bitmap2 = 0;
	if (!query_rslt && query_bitmap && !query_bitmap->isEmpty())
	{
		bitmap2 = AosBitmapMgrObj::getBitmapStatic();
		cur_bitmap = bitmap2;
	}

	bool rslt = iilobj->queryNewSafe(query_rslt, cur_bitmap, query_context, rdata);
	if (bitmap2)
	{
		query_bitmap->andBitmap(bitmap2);
		AosBitmapMgrObj::returnBitmapStatic(bitmap2);
	}
	return rslt;
}


AosIILObjPtr
AosIILMgr::createSubIILSafe(
		const u64 &parentid,
		const u32 siteid,
		const u64 &snap_id,
		const AosIILType &iiltype, 
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	u64 iilid;
	int vid = parentid % mVirServerNum;
	aos_assert_r(vid >=0, 0);
	bool rslt = createIILID(iilid, vid, rdata);
	aos_assert_r(rslt, 0);
	aos_assert_r(parentid != iilid, 0);

	return createIILPublic(iilid, siteid, snap_id, iiltype, isPersis, rdata);
}


bool 
AosIILMgr::StrBatchAddMergeSafe(
		const OmnString &iilname,
		const i64 &size,
		const AosIILExecutorObjPtr &executor,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	OmnString parallel_iilname = AosIILName::composeParallelIILName(iilname);
	u64 iilid = getIILID(parallel_iilname, false, rdata);

	if (iilid == 0) return true;
		
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;

	//Linda, 2014/03/28
    OmnMutexPtr lock = mLockPool.get(iilid, mLockGen);
	lock->lock();

	AosIILType type;
	AosIILObjPtr parallel_iil = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_rl(parallel_iil, lock, false);
	aos_assert_rl(type == eAosIILType_BigStr, lock, false);

	type = eAosIILType_BigStr;
	AosIILObjPtr iilobj;
	iilid = getIILID(iilname, false, rdata);
	if (iilid == 0)
	{
		iilid = getIILID(iilname, true, rdata);
		iilobj = createIILPublic(iilid, rdata->getSiteid(), snap_id, type, false, rdata);
		aos_assert_rl(iilobj, lock, false);

		//snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
		snap_id = 0;
		iilobj->setSnapShotId(snap_id);
	}
	else
	{
		//snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
		snap_id = 0;
		iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
		aos_assert_rl(iilobj, lock, false);
	}
	aos_assert_rl(type == eAosIILType_BigStr, lock, false);

	AosIILBigStrPtr iilstr = dynamic_cast<AosIILBigStr *>(iilobj.getPtr());
	AosIILBigStrPtr parallel_iilstr = dynamic_cast<AosIILBigStr *>(parallel_iil.getPtr());
	 
	u64 start_time = OmnGetTimestamp();
	iilstr->addMergeIIL(parallel_iilstr, size, executor, rdata);
	u64 cost = OmnGetTimestamp() - start_time;
	OmnScreen << "\n\n================================="
			  << "\nBatch Add Merge Finish, time cost:" << AosTimestampToHumanRead(cost)
			  << "\n=================================\n" << endl;
		
	parallel_iilstr->deleteIILSafe(true_delete, rdata);
	returnIILPublic(iilobj, rdata);
	returnIILPublic(parallel_iil, rdata);
	lock->unlock();
	return true;
}


bool 
AosIILMgr::StrBatchIncMergeSafe(
		const OmnString &iilname,
		const i64 &size,
		const u64 &dftvalue, 
		const AosIILUtil::AosIILIncType incType,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	OmnString parallel_iilname = AosIILName::composeParallelIILName(iilname);
	u64 iilid = getIILID(parallel_iilname, false, rdata);

	if (iilid == 0) return true;
		
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;

    OmnMutexPtr lock = mLockPool.get(iilid, mLockGen);
	lock->lock();

	AosIILType type;
	AosIILObjPtr parallel_iil = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_rl(parallel_iil, lock, false);
	aos_assert_rl(type == eAosIILType_BigStr, lock, false);

	type = eAosIILType_BigStr;
	AosIILObjPtr iilobj;
	iilid = getIILID(iilname, false, rdata);
	if (iilid == 0)
	{
		iilid = getIILID(iilname, true, rdata);
		iilobj = createIILPublic(iilid, rdata->getSiteid(), snap_id, type, false, rdata);
		aos_assert_rl(iilobj, lock, false);

		//snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
		snap_id = 0;
		iilobj->setSnapShotId(snap_id);
	}
	else
	{
		//snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
		snap_id = 0;
		iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
		aos_assert_rl(iilobj, lock, false);
	}
	aos_assert_rl(type == eAosIILType_BigStr, lock, false);

	AosIILBigStrPtr iilstr = dynamic_cast<AosIILBigStr *>(iilobj.getPtr());
	AosIILBigStrPtr parallel_iilstr = dynamic_cast<AosIILBigStr *>(parallel_iil.getPtr());

	u64 start_time = OmnGetTimestamp();
	iilstr->incMergeIIL(parallel_iilstr, size, dftvalue, incType, rdata);
	u64 cost = OmnGetTimestamp() - start_time;
	OmnScreen << "\n\n================================="
			  << "\nBatch Inc Merge Finish, time cost:" << AosTimestampToHumanRead(cost)
			  << "\n=================================\n" << endl;
		
	parallel_iilstr->deleteIILSafe(true_delete, rdata);
	returnIILPublic(iilobj, rdata);
	returnIILPublic(parallel_iil, rdata);
	lock->unlock();
	return true;
}


// moved from IIL.cpp
// by Ketty 2012/10/29
AosDocFileMgrObjPtr
AosIILMgr::getDocFileMgr(
		const u64 &iilid, 
		u64 &local_iilid,
		const AosRundataPtr &rdata)
{
	int virtual_id = 0;
	bool rslt = parseIILID(iilid, local_iilid, virtual_id, rdata);
	aos_assert_r(rslt, 0);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
		rdata.getPtr(), virtual_id, sgDfmConf);
	
	if (!docFileMgr)
	{
		docFileMgr = vfsMgr->createDocFileMgr(rdata.getPtr(), virtual_id, sgDfmConf);
		if (!docFileMgr)
		{
			OmnAlarm << "Failed creating docfilemgr. This is a serious problem!" << enderr;
			return 0;
		}
	}

	return docFileMgr;
}


// Ketty 2012/12/18
bool
AosIILMgr::svrIsUp(const AosRundataPtr &rdata, const u32 svr_id)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	/*
	if (!mIDTransMap)	
	{
		// means this svr is self is just up yet. not need clean
		return true;
	}

	bool rslt = true;
	vector<u32> vids;
	AosGetVirtualIds(vids, svr_id);
	for (u32 i=0; i<vids.size(); i++)
	{
		u32 virtual_id = vids[i];
		if (AosGetBackupSvrId(virtual_id) != AosGetSelfServerId())   continue;

		rslt = mIDTransMap->cleanBkpVirtual(rdata, virtual_id);
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
	vector<AosIILObjPtr> iils;
	u32 loc_iilid;
	u64 c_vid;

	// 1. clean from mIILIDHash.
	AOSLOCK(mLock);
	IILIDHashItr itr = mIILIDHash.begin();
	while (itr != mIILIDHash.end())
	{
		u64 combinedId = itr->first; 

		loc_iilid = (u32)combinedId;
		c_vid = loc_iilid % mVirServerNum;
		if (c_vid != virtual_id)
		{
			itr++;
			continue;
		}

		AosIILObjPtr iilobj = itr->second;
		iils.push_back(iilobj);
		if (iilobj->getRefCountByIILMgr() == 0)
		{
			removeIILFromIdleListPriv(iilobj, rdata);
		}

		itr++;
		mIILIDHash.erase(combinedId);
		
		OmnScreen << "IILMgr; clean bkp virtuals:" << virtual_id
				<< "; combineedId:" << combinedId
				<< "; local_id:" << loc_iilid
				<< endl;
	}
	AOSUNLOCK(mLock);

	// 2. clean from mSavingList 
	mSavingLock->lock();
	list<AosIILObjPtr>::iterator d_itr = mSavingList.begin();
	while (d_itr != mSavingList.end())
	{
		AosIILObjPtr iilobj = *d_itr;
		loc_iilid = iilobj->getIILID() / mVirServerNum;
		c_vid = loc_iilid % mVirServerNum;
		
		if (c_vid != virtual_id)
		{
			d_itr++;
			continue;
		}
		
		iils.push_back(iilobj);
		list<AosIILObjPtr>::iterator d_itr_tmp = d_itr;
		d_itr++;
		mSavingList.erase(d_itr_tmp);
	}
	mSavingLock->unlock();

	for (u32 i=0; i<iils.size();i++)
	{
		iils[i]->resetSafe(0, iils[i]->getIILID(), iils[i]->getSiteid(), rdata);
		iils[i]->setDirty(false);
		iils[i]->setNew(true);
	}
	return true;
}


bool
AosIILMgr::checkIsIILIDUsed(
		const u64 &iilid, 
		const u32 siteid, 
		const AosRundataPtr &rdata)
{
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;
	return AosIIL::staticIsIILIDUsed(iilid, snap_id, rdata);
}


// Chen Ding, 2013/04/19
AosIILObjPtr 
AosIILMgr::nextLeaf(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		AosIILIdx &idx)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, 0);

	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;
	
	AosIILType type;
	AosIILObjPtr iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_r(iilobj, 0);
	aos_assert_r(type == eAosIILType_BigStr, 0);

	return iilobj->nextLeaf(idx, rdata);
}


AosIILObjPtr 
AosIILMgr::firstLeaf(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		AosIILIdx &idx)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, 0);
	
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;

	AosIILType type;
	AosIILObjPtr iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_r(iilobj, 0);
	aos_assert_r(type == eAosIILType_BigStr, 0);

	return iilobj->firstLeaf(idx, rdata);
}


AosIILObjPtr 
AosIILMgr::nextLeaf(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		AosIILIdx &idx,
		AosBitmapTreeObjPtr &tree)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, 0);
	
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;

	AosIILType type;
	AosIILObjPtr iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_r(iilobj, 0);
	aos_assert_r(type == eAosIILType_BigStr, 0);
	
	tree = 0;
	return iilobj->nextLeaf(idx, tree, rdata);
}


AosIILObjPtr 
AosIILMgr::firstLeaf(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		AosIILIdx &idx,
		AosBitmapTreeObjPtr &tree)
{
	u64 iilid = getIILID(iilname, false, rdata);
	aos_assert_r(iilid > 0, 0);
	
	//u64 snap_id = AosIILSave::getSelf()->getSnapIdByIILID(iilid);
	u64 snap_id = 0;

	AosIILType type;
	AosIILObjPtr iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
	aos_assert_r(iilobj, 0);
	aos_assert_r(type == eAosIILType_BigStr, 0);

	tree = 0;
	return iilobj->firstLeaf(idx, tree, rdata);
}

bool 
AosIILMgr::mergeSnapshot(
		const u32 virtual_id,
		const u64 &target_snap_id,
		const u64 &merge_snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr docFileMgr;
	docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata.getPtr(), virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, 0);

	return docFileMgr->mergeSnapshot(rdata, target_snap_id, merge_snap_id, trans_id);
}          


u64
AosIILMgr::createSnapshot(
		const u32 virtual_id,
		const u64 snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr docFileMgr;
	docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata.getPtr(), virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, 0);

	docFileMgr->createSnapshot(rdata, snap_id, trans_id);
	return snap_id;
}


bool
AosIILMgr::commitSnapshot(
		const u32 virtual_id,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	flushContentsBySnapId(snap_id, rdata);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata.getPtr(), virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, false);

	bool rslt = docFileMgr->commitSnapshot(rdata, snap_id, trans_id);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILMgr::rollbackSnapshot(
		const u32 virtual_id,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	flushContentsBySnapId(snap_id, rdata);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata.getPtr(), virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, false);

	bool rslt = docFileMgr->rollbackSnapshot(rdata, snap_id, trans_id);
	aos_assert_r(rslt, false);
	return true;
}

/*
bool
AosIILMgr::createSnapShot(
		u32 &snap_id,
		const u32 virtual_id,
		const bool need_remove,
		const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	AosDocFileMgrObjPtr docFileMgr;
	docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata, virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, false);

	snap_id = 0;
	bool rslt = true;
	if (need_remove)
	{
		rslt = docFileMgr->addAndRemoveSnapShot(rdata, snap_id);
	}
	else
	{
		rslt = docFileMgr->addSnapShot(rdata, snap_id);
	}
	aos_assert_r(rslt, false);
	aos_assert_r(snap_id != 0, false);

	return true;
}


bool
AosIILMgr::commitSnapShot(
		const OmnString &iilname,
		const AosRundataPtr &rdata)
{
	u32 virtual_id = AosGetCubeId(iilname);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(
		rdata, virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, false);
	
	bool rslt = docFileMgr->commit(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILMgr::commitSnapShot(const AosRundataPtr &rdata)
{
	vector<u32> virtualIds;
	bool rslt = AosGetLocalVirtualIds(virtualIds);
	aos_assert_r(rslt, false);
	aos_assert_r(virtualIds.size() > 0, false);
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr docFileMgr;
	for (u32 i=0; i<virtualIds.size(); i++)
	{
		docFileMgr = vfsMgr->retrieveDocFileMgr(
			rdata, virtualIds[i], sgDfmConf);
		aos_assert_r(docFileMgr, false);
		
		rslt = docFileMgr->commit(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

	
bool 
AosIILMgr::rollBackSnapShot(
		const u32 snap_id,
		const u32 virtual_id,
		const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	
	AosDocFileMgrObjPtr docFileMgr;
	docFileMgr = vfsMgr->retrieveDocFileMgr(
		rdata, virtual_id, sgDfmConf);
	aos_assert_r(docFileMgr, false);

	AOSLOCK(mLock);
	mIsFreshing = true;	
	AOSUNLOCK(mLock);

	aos_assert_r(mIDTransMap, false);
	bool rslt = mIDTransMap->cleanCache(virtual_id, rdata);
	aos_assert_r(rslt, false);

	vector<AosIILObjPtr> iils;
	AosIILObjPtr iilobj;
	u64 combinedId;
	u32 siteid;
	u64 id;
	u32 vid;
	u32 c;

	int guard = 0;
	bool finish = false;
	while (!finish)
	{
		AOSLOCK(mLock);
		finish = true;

		IILIDHashItr itr = mIILIDHash.begin();
		while (itr != mIILIDHash.end())
		{
			combinedId = itr->first; 
			id = AosSiteid::separateSiteid(combinedId, siteid);
			vid = id % AosGetNumCubes();
			if (vid != virtual_id)
			{
				itr++;
				continue;
			}

			iilobj = itr->second;
			c = iilobj->getRefCountByIILMgr();
			if (c == 0)
			{
				removeIILFromIdleListPriv(iilobj, rdata);
				iils.push_back(iilobj);
			}
			else
			{
				finish = false;
			}
			itr++;
			mIILIDHash.erase(combinedId);
		}

		AOSUNLOCK(mLock);
	
		for (u32 i=0; i<iils.size(); i++)
		{
			iilobj = iils[i];
			iilobj->resetSafe(0, 0, 0, rdata);
			iilobj->finishTrans();
		}
		iils.clear();

		OmnMsSleep(200);
		guard++;

		if (guard > 20)
		{
			OmnAlarm << "to much time to clear" << enderr;
		}
	}

	rslt = docFileMgr->rollBack(rdata, snap_id);
	aos_assert_r(rslt, false);

	AOSLOCK(mLock);
	mIsFreshing = false;	
	AOSUNLOCK(mLock);

	return true;
}
*/

	
// Ken Lee, 2013/05/17
void
AosIILMgr::cleanCache()
{
	aos_assert(mIDTransMap);
	//mIDTransMap->cleanCache();
	mCleanCache = true;
}

/*
bool
AosIILMgr::createSnapShot(
		const AosRundataPtr &rdata,
		const AosIILObjPtr &iilobj)
{
	aos_assert_r(iilobj, false);

	u64 snap_id = iilobj->getSnapShotId();
	aos_assert_r(snap_id == 0, false);

	bool rslt = iilobj->saveToFileSafe(rdata);
	aos_assert_r(rslt, false);

	u64 iilid = iilobj->getIILID();
	aos_assert_r(iilid, false);

	AosIILSave::getSelf()->addSnapShot(rdata, iilid, snap_id);
	aos_assert_r(snap_id != 0, false);
	iilobj->setSnapShotId(snap_id);

	return true;
}


bool
AosIILMgr::commitSnapShot(
		const AosRundataPtr &rdata,
		const AosIILObjPtr &iilobj)
{
	aos_assert_r(iilobj, false);

	u64 snap_id = iilobj->getSnapShotId();
	if (snap_id == 0) return true;

	bool rslt = iilobj->saveToFileSafe(rdata);
	aos_assert_r(rslt, false);

	u64 iilid = iilobj->getIILID();
	aos_assert_r(iilid, false);

	u64 localid = 0;
	int vid = -1;
	rslt = parseIILID(iilid, localid, vid, rdata);
	aos_assert_r(rslt, false);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr docFileMgr = vfsMgr->retrieveDocFileMgr(rdata, vid, sgDfmConf);
	aos_assert_r(docFileMgr, false);
	
	iilobj->resetSnapShotId();

	rslt = AosIILSave::getSelf()->commit(rdata, iilid, docFileMgr, snap_id);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosIILMgr::rollBackSnapShot(
		const AosRundataPtr &rdata,
		const AosIILObjPtr &iilobj)
{
	aos_assert_r(iilobj, false);

	u64 snap_id = iilobj->getSnapShotId();
	if (snap_id == 0) return true;

	u64 iilid = iilobj->getIILID();
	aos_assert_r(iilid, false);

	u64 localid = 0;
	int vid = -1;
	bool rslt = parseIILID(iilid, localid, vid, rdata);
	aos_assert_r(rslt, false);

	// 1. clear Cache
	// 2. finished Trans
	AosDocFileMgrObjPtr docFileMgr;
	docFileMgr = AosVfsMgrObj::getVfsMgr()->retrieveDocFileMgr(
		rdata, vid, sgDfmConf);
	aos_assert_r(docFileMgr, false);

	u32 dfm_id = docFileMgr->getId();
	rslt = AosIILSave::getSelf()->rollBack(rdata, iilid, snap_id, vid, dfm_id);
	aos_assert_r(rslt, false);

	//iilobj->finishTrans();

	iilobj->resetIIL(rdata);
	return true;
}
*/

// Chen Ding, 2013/10/26
bool
AosIILMgr::flushContents(const AosRundataPtr &rdata)
{
	OmnScreen << "To flush IILMgr contents!" << endl;

	OmnScreen << "To proc all pending transactions ... " << endl;
	procAllTrans(rdata);
	OmnScreen << "To proc all pending transactions ... Finished" << endl;
	OmnScreen << "To save all IILs ... " << endl;
	saveAllIILs(rdata);
	OmnScreen << "To save all IILs ... Finished" << endl;
	OmnScreen << "To clear all opened IILs ..." << endl;
	clearOpenedIILs(rdata);
	OmnScreen << "To clear all opened IILs ... finished" << endl;
	return true;
}


bool
AosIILMgr::clearOpenedIILs(const AosRundataPtr &rdata)
{
	aos_assert_rr(mSavingList.size() == 0, rdata, false);

	// IMPORTANT: This will erase all the IILs.
	AOSLOCK(mLock);
	AosIILObjPtr iilobj;
	IILIDHashItr itr = mIILIDHash.begin();
	i64 count = 0;
	while (itr!=mIILIDHash.end())
	{
		if (++count % 100 == 0) OmnScreen << "iilobj count: " << count << endl;
		iilobj = itr->second;
		if (iilobj->isDirty())
		{
			OmnAlarm << "internal error" << enderr;
			AOSUNLOCK(mLock);
			return false;
		}

		itr++;
	}

	OmnMark;
	mIILIDHash.reset();
	OmnMark;

	for (int i=0; i<eAosIILType_Total; i++)
	{
		OmnMark;
		AosIILObjPtr iilobj = mIILHead[i];
		count = 0;
		while (iilobj)
		{
			if (++count % 100 == 0) OmnScreen << "iilobj count: " << count << endl;
			AosIILObjPtr next = iilobj->getNextByIILMgr();
			iilobj->resetPtrsByIILMgr();
			iilobj = next;
		}
		mIILHead[i] = 0;
		mIILTail[i] = 0;
		sgTotalIdleIIL[i] = 0;
	}

	OmnScreen << "IILs remaining: " << AosIIL::getNumIILsCreated() << endl;

	AOSUNLOCK(mLock);

	return true;
}

bool	
AosIILMgr::rebuildBitmap(
				const AosIILObjPtr &iilobj,
				const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	bool rslt = iilobj->rebuildBitmapSafe(rdata);
	return rslt;
}


bool
AosIILMgr::switchSnapShot(
		const AosIILObjPtr &iilobj, 
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	//Linda, 2014/03/28
	u64 snapid = iilobj->getSnapShotId();
	if (snapid != snap_id)
	{
		if (iilobj->isRootIIL() || iilobj->isSingleIIL())
		{
			iilobj->saveToFileSafe(rdata);
		}

		aos_assert_r(!iilobj->isDirty(), false);
		u32 siteid = iilobj->getSiteid();
		u64 iilid = iilobj->getIILID();

		iilobj->lockIIL();
		iilobj->resetPriv(0, 0, 0, rdata);
		iilobj->setSnapShotIdPriv(snap_id);
		iilobj->setSiteid(siteid);
		bool rslt = iilobj->loadFromFilePriv(iilid, rdata);
		iilobj->unlockIIL();
		if (!rslt)
		{
			aos_assert_r(!iilobj->isDirty(), false);
		}
	}
	return true;
}


bool
AosIILMgr::flushContentsBySnapId(
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	//Linda, 2014/03/28
	procAllTrans(rdata);

	vector<AosIILObjPtr> iils;
	AOSLOCK(mLock);
	AosIILObjPtr iilobj;
	IILIDHashItr itr = mIILIDHash.begin();
	while(itr != mIILIDHash.end())
	{
		iilobj = itr->second;
		u64 snapid = iilobj->getSnapShotId();
		if (snapid == snap_id && (iilobj->isRootIIL() || iilobj->isSingleIIL()))
		{
			//Jozhi 2014-07-31 dead lock
			//iilobj->saveToFileSafe(rdata);
			//aos_assert_r(!iilobj->isDirty(), false);
			iils.push_back(iilobj);
		}
		itr++;
	}
	AOSUNLOCK(mLock);

	//Jozhi 2014-07-31 dead lock
	for(u32 i=0; i<iils.size(); i++)
	{
		iils[i]->saveToFileSafe(rdata);
		aos_assert_r(!iils[i]->isDirty(), false);
	}

/*
 * check
AOSLOCK(mLock);
itr = mIILIDHash.begin();
while(itr != mIILIDHash.end())
{
	iilobj = itr->second;
	u64 snapid = iilobj->getSnapShotId();
	if (snapid == snap_id)
	{
		aos_assert_rl(!iilobj->isDirty(), mLock, false);
	}
	itr++;
}
AOSUNLOCK(mLock);
*/
	return true;
}


AosIILObjPtr
AosIILMgr::createJimoTable(
		const OmnString &iilname,
		const AosXmlTagPtr &cmp_tag,
		const AosRundataPtr &rdata)
{
	u64 snap_id = 0;
	AosIILObjPtr iilobj;
	AosIILType type = eAosIILType_JimoTable;

	u64 iilid = getIILID(iilname, false, rdata);
	if (iilid == 0)
	{
		iilid = getIILID(iilname, true, rdata);
		iilobj = createIILPublic(iilid, rdata->getSiteid(), snap_id, type, false, rdata);
		aos_assert_r(iilobj, 0);

		snap_id = 0;
		iilobj->setSnapShotId(snap_id);
	}
	else
	{
		snap_id = 0;
		iilobj = loadIILPublic(iilid, rdata->getSiteid(), snap_id, type, rdata);
		aos_assert_r(iilobj, 0);
	}

	if (iilobj->getNumDocs() == 0)
	{
		AosJimoTablePtr jimo = dynamic_cast<AosJimoTable *>(iilobj.getPtr());
		bool rslt = jimo->setCmpTag(cmp_tag, rdata);
		aos_assert_r(rslt, 0);
	}
	else
	{
		OmnAlarm << "iilobj is already exist and docnum is:" << iilobj->getNumDocs() << enderr;
	}

	return iilobj;
}


u64
AosIILMgr::getDocidByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid != "", 0);

	int cid = AosGetCubeId(objid);
	u32 siteid = rdata->getSiteid();
	OmnString tmpname = AosIILName::composeObjidListingName(siteid);

	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		iilname = AosIILName::composeCubeIILName(cid, tmpname);
	}

	u64 iilid = getIILID(iilname, false, rdata);
	if (iilid == 0)
	{
		return 0;
	}
		
	u64 snap_id = 0;
	AosIILType type = eAosIILType_Str;
	AosIILObjPtr iilobj = loadIILPublic(iilid, siteid, snap_id, type, rdata);
	aos_assert_r(iilobj, 0);

	AosIILStrPtr striil = dynamic_cast<AosIILStr *>(iilobj.getPtr());
	aos_assert_r(striil, 0);

	u64 docid = 0;
	bool isunique = false;
	int64_t idx = -10;
	int64_t iilidx = -10;
	bool rslt = striil->nextDocidSafe(idx, iilidx, false,
		eAosOpr_eq, objid, docid, isunique, rdata); 
	aos_assert_r(rslt, 0);

	return docid;
}


bool
AosIILMgr::unbindObjid(
		const OmnString &objid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid != "", false);

	int cid = AosGetCubeId(objid);
	u32 siteid = rdata->getSiteid();
	OmnString tmpname = AosIILName::composeObjidListingName(siteid);

	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		iilname = AosIILName::composeCubeIILName(cid, tmpname);
	}

	u64 iilid = getIILID(iilname, false, rdata);
	if (iilid == 0)
	{
		return true;
	}

	u64 snap_id = 0;
	AosIILType type = eAosIILType_Str;
	AosIILObjPtr iilobj = loadIILPublic(iilid, siteid, snap_id, type, rdata);
	aos_assert_r(iilobj, false);

	AosIILStrPtr striil = dynamic_cast<AosIILStr *>(iilobj.getPtr());
	aos_assert_r(striil, false);

	bool rslt = striil->removeDocSafe(objid, docid, rdata);
	aos_assert_r(rslt, false);

	return true;
}


OmnString
AosIILMgr::getMemoryStatus()
{
	AOSLOCK(mLock);
	map<AosIILType, i64> mp;
	for (int i=0; i<eAosIILType_Total; i++)
	{
		mp[(AosIILType)i] = 0;
	}

	AosIILObjPtr iilobj;
	AosIILType type;
	i64 num;
	IILIDHashItr itr = mIILIDHash.begin();
	while (itr != mIILIDHash.end())
	{
		iilobj = itr->second;
		type = iilobj->getIILType();
		if (iilobj->isSingleIIL() || (!iilobj->isSingleIIL() && !iilobj->isParentIIL()))
		{
			num = iilobj->getNumDocs();
		}
		else
		{
			num = iilobj->getNumSubiils() * 2;
		}

		num += mp[type];
		mp[type] = num;

		itr++;
	}

	OmnString ss1 = "IIL Num\n";
	OmnString ss2 = "IIL Entry Num\n";
	for (int i=0; i<eAosIILType_Total; i++)
	{
		if (AosIIL::mTotalIILs[i] > 0)
		{
			ss1 << "IILType:" << AosIILType_toStr((AosIILType)i) << ":" << AosIIL::mTotalIILs[i] << "\n"; 
		}

		if (mp[(AosIILType)i] > 0)
		{
			ss2 << "IILType:" << AosIILType_toStr((AosIILType)i) << ":" << mp[(AosIILType)i] << "\n"; 
		}
	}
	OmnString ss;
	ss << ss1 << ss2;
	AOSUNLOCK(mLock);
	return ss;
}

