////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Header Format:
// 		IILType		u8
// 		IILID		u64
// 		WordID		u64
// 		Num Docs	u32
// 		Flag		char
// 		Hit Count	u32
// 		Version		u32
//		IIL Size	u32
//		Offset		u64
//		Seqno		u32
//		Comp Size	u32
//		Comp Offset u64
//		Comp Seqno	u32
//
// Modification History:
// 	Created: 10/26/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/IIL.h"

#include "Debug/ExitHandler.h"
#include "DocUtil/DocUtil.h"
#include "IILMgrBig/IILMgr.h"
#include "IILMgrBig/IILHit.h"
#include "IILMgrBig/IILStr.h"
#include "IILMgrBig/IILU64.h"
#include "IILMgrBig/IILCompStr.h"
#include "IILMgrBig/IILCompU64.h"
#include "Porting/TimeOfDay.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEUtil/Passwords.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/Siteid.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageMgrUtil/DevLocation.h"
#include "WordClient/WordClient.h"
#include "Porting/ThreadDef.h"
#include "IdGen/IdGen.h"
#include "Thread/Mutex.h"

// Chen Ding, 01/25/2012
// const u32 sgMaxDocFileMgrs = 100000;
// static AosDocFileMgrObjPtr sgNormDocFileMgrs[sgMaxDocFileMgrs];
// static AosDocFileMgrObjPtr sgPersisDocFileMgrs[sgMaxDocFileMgrs];

// Chen Ding, 01/23/2012, Testing only
// const u64 sgMaxIILID_Test = 300000;
// const u64 sgNumIILIDs_Test = 10;
// static u64 sgIILIDLog[sgMaxIILID_Test][sgNumIILIDs_Test];
// static void sgSetLog(const u64 &iilid, const u32 seqno, const u32 offset)
// {
// 	u64 uu = (((u64)seqno) << 32) + offset;
// 	if (iilid < sgMaxIILID_Test)
// 	{
// 		for (u64 i=0; i<sgNumIILIDs_Test; i++)
// 		{
// 			if (sgIILIDLog[iilid][i] == 0)
// 			{
// 				sgIILIDLog[iilid][i] = uu;
// 				break;
// 			}
// 		}
// 	}
// }

//static AosVirtualFileSysPtr sgVirtualFileSys;
//static AosDevLocationPtr sgDevLocation;

//static AosDocFileMgrPtr sgDocFileMgr;
static u32 sgMaxHeaderPerFile = 0;
const bool sgSaveSanityCheck = false;
const int  sgHeaderSize = 62;

u32		AosIIL::mExtraDocids = eExtraDocids;
u32		AosIIL::mExtraDocidsInit = eInitNumDocids;
u32		AosIIL::mMaxIILSize = 0;
u32		AosIIL::mMinIILSize = 0;
u32		AosIIL::mMaxSubIILs = 0;
u32		AosIIL::mMinSubIILs = 0;
u32		AosIIL::mAddBlockMaxSize = 20000;	// Ken Lee, 2013/01/03
u32		AosIIL::mAddBlockMinSize = 4000;	// Ken Lee, 2013/01/03
u32		AosIIL::mAddBlockFillSize = 16000;	// Ken Lee, 2013/01/03

bool	AosIIL::smNeedCompress = true;		// Chen Ding DDDDD, 2013/01/26
u32		AosIIL::smCompressSize = AosIIL::eCompressSize;		// Chen Ding DDDDD, 2013/01/26

bool 	AosIIL::mIILMergeFlag = false;
int 	AosIIL::mTotalIILs[eAosIILType_Total];
AosDfmDocIILPtr AosIIL::smDfmDoc;

static int sgNumIILCreated = 0;
int sgNumIILRefs = 0;
static OmnMutex sgLock;

AosIIL::AosIIL(const AosIILType type, const bool isPersis)
:
mRefcount(0),
mIILType(type),
mIILID(AOS_INVIILID),
mWordId(AOS_INVWID),
mNumDocs(0),
mFlag(0),
mHitcount(0),
mVersion(0),
mIsPersis(isPersis),
mCompressedSize(0),
mMemCap(0),
mDocids(0),
mLock(OmnNew OmnMutex()),
mIsDirty(true),
mIsNew(false),
mIsGood(true),
mNumSubiils(0),
mIILIdx(-1),
mNumEntries(0),
mIILIds(0),
mLastAddSec(0),
mCreationOnly(false),
mSiteid(0),
mDebugFlag(eGoodIILFlag)
{
	// OmnScreen << "IIL created: " << this << endl;
	sgNumIILCreated++;

	//ken 2011/08/19
	mTrans.clear();
//OmnScreen << "-------iil be created :" << sgNumIILCreated << endl;
}


AosIIL::AosIIL(
	const u64 &wordid,
	const u64 &iilid, 
	const AosIILType iiltype,
	const bool isPersis,
	const OmnString &iilname)
:
mRefcount(0),
mIILType(iiltype),
mIILID(iilid),
mWordId(wordid),
mNumDocs(0),
mFlag(AOS_ACTIVE_FLAG),
mHitcount(0),
mVersion(0),
mIsPersis(isPersis),
mCompressedSize(0),
mIILName(iilname),
mMemCap(0),
mDocids(0),
mLock(OmnNew OmnMutex()),
mIsDirty(true),
mIsNew(false),
mIsGood(true),
mNumSubiils(0),
mIILIdx(-1),
mNumEntries(0),
mIILIds(0),
mCreationOnly(false),
mSiteid(0),
mDebugFlag(eGoodIILFlag)
{
	// OmnScreen << "IIL created: " << this << endl;
	sgNumIILCreated++;
	
	//ken 2011/08/19
	mTrans.clear();
//OmnScreen << "-------iil be created :" << sgNumIILCreated << endl;
}


AosIIL::~AosIIL()
{
	// OmnAlarm << "IIL Should not be deleted!" << enderr;
	//AosIILMgrSelf->checkIIL(this);
	OmnAlarm << "-------iil be deleteed :" << mIILID << "," << sgNumIILCreated << ":" << this << enderr;
	// Chen Ding, 11/06/2012
	if (mDebugFlag != eGoodIILFlag)
	{
		OmnAlarm << "To delete an invalid IIL: " << mDebugFlag << enderr;
		return;
	}
	sgNumIILCreated--;
	if (mDebugFlag != eGoodIILFlag)
	{
		OmnAlarm << "To delete an invalid IIL: " << mDebugFlag << ":" << this << enderr;
		return;
	}

	mDebugFlag = eDeletedIILFlag;
	if (mDocids)
	{
		aos_assert(mMemCap > 0);
		returnMemory((char *)mDocids, mMemCap);
		mDocids = 0;
		mMemCap = 0;
	}

	if(mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if(mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	
	//ken 2011/08/19
	mTrans.clear();
}


bool
AosIIL::staticStop()
{
	//if(sgDocFileMgr) sgDocFileMgr->stop();
	return true;
}


int 
AosIIL::staticNumIILRefs()
{
	return sgNumIILRefs;
}


bool			
AosIIL::staticInit(const AosXmlTagPtr &theconfig)
{
	// memset(sgIILIDLog, 0, sgMaxIILID_Test*sgNumIILIDs_Test*8);
	// Ketty 2013/01/14
	smDfmDoc = OmnNew AosDfmDocIIL(0);

	for(int i=0;i < eAosIILType_Total;i++)
	{
		mTotalIILs[i] = 0;
	}
	
	// Its configuration is in the subtag "AOSCONFIG_IILMGR"
	aos_assert_r(theconfig, false);
	AosXmlTagPtr config = theconfig->getFirstChild(AOSCONFIG_IILMGR);
	if (!config) return true;

	//OmnString dirname = theconfig->getAttrStr(AOSCONFIG_DIRNAME);
	//aos_assert_r(dirname != "", false);

	//OmnString fname = config->getAttrStr(AOSCONFIG_IIL_FILENAME);
	//aos_assert_r(fname != "", false);

	sgMaxHeaderPerFile = eAosMaxFileSize / eIILHeaderSize;

	//u32 maxdocfiles = config->getAttrU64(AOSCONFIG_IIL_MAX_DOCFILES, 0);
	//aos_assert_r(maxdocfiles > 0, false);
	
	mMinIILSize = config->getAttrU64(AOSCONFIG_MIN_IILSIZE, eMinIILSize);
	aos_assert_r(mMinIILSize > 0, false);

	mMaxIILSize = config->getAttrU64(AOSCONFIG_MAX_IILSIZE, eMaxIILSize);
	aos_assert_r(mMaxIILSize > 0, false);
	aos_assert_r(mMinIILSize < mMaxIILSize, false);

	mMinSubIILs = config->getAttrU64(AOSCONFIG_MIN_SUBIILSIZE, eMinSubIILs);
	aos_assert_r(mMinSubIILs > 0, false);

	mMaxSubIILs = config->getAttrU64(AOSCONFIG_MAX_SUBIILSIZE, eMaxSubIILs);
	aos_assert_r(mMaxSubIILs > 0, false);
	aos_assert_r(mMinSubIILs < mMaxSubIILs, false);

	// Chen Ding DDDDD, 2013/01/26
	smNeedCompress = config->getAttrBool("compress_iil", true);
	int nn = config->getAttrInt("compress_size", eCompressSize);
	if (nn < 0) smCompressSize = eCompressSize;
	else smCompressSize = (u32)nn;
	
	// Ken Lee 2013/01/03
	mAddBlockMaxSize = mMaxIILSize;
	mAddBlockMinSize = mMaxIILSize * 0.2;
	mAddBlockFillSize = mMaxIILSize * 0.8;

	//ken 2011/07/13
	//AosRundataPtr rdata = OmnApp::getRundata();

	try
	{
		//sgDevLocation = OmnNew AosDevLocation1();
		// sgVirtualFileSys = OmnNew AosVirtualFileSys(AOSFILEKEY_IIL_STORAGE_APP, rdata);
		/*
		sgVirtualFileSys = AosVirtualFileSys::getIILVirtualFileSys(
			AOSPASSWD_SYSTEM_CALL, rdata);
		if (!sgVirtualFileSys->isGood())
		{
			OmnAlarm << "Failed creating VirtualFileSys for IILMgr. "
				<< "Check the configuration!" << enderr;
			OmnExitApp("Failed creating storageApp for IILMgr.");
		}
		*/
	}

	catch (...)
	{
		OmnExitApp("Failed creating storageApp for IILMgr.");
	}
	return true;
}


AosIILPtr
AosIIL::staticLoadFromFile1(
		const u64 &iilid, 
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata) 
{
	// It creates a new instance of IIL and load it from file.
	// This function is thread safe. There is no need to lock
	// it. The actual work is done by 'staticReadHeaderBuff(...)'.
	// The header has already been read. It is in 'iil_header'.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_rr(docfilemgr, rdata, 0);

	// Ketty 2013/01/14
	AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	bool rslt = docfilemgr->readDoc(rdata, local_iilid, doc);
	aos_assert_rr(rslt, rdata, 0);
	if (!doc->doesDocExist())	return 0;

	AosDfmDocIILPtr iildoc = (AosDfmDocIIL*)doc.getPtr();
	AosIILType iiltype = iildoc->getIILType();

	AosIILPtr iil;
	u32 siteid = rdata->getSiteid();

	switch (iiltype)
	{
	case eAosIILType_Str:
		 iil = OmnNew AosIILStr(iilid, siteid, iildoc, iilmgrLocked, rdata);
		 break;

	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(iilid, siteid, iildoc, iilmgrLocked, rdata);
		 break;

	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(iilid, siteid, iildoc, iilmgrLocked, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL type: " << iiltype 
			 << ":" << iilid << enderr;
		 return 0;
	}
	
	aos_assert_r(iil && iil->isGood(), 0);
	return iil;
}


AosIILPtr
AosIIL::staticCreateIIL(
	const u64 &wordid,
	const u64 &iilid, 
	const u32 siteid, 
	const AosIILType iiltype,
	const bool isPersis,
	const OmnString &iilname, 
	const bool iilmgrLocked,
	const AosRundataPtr &rdata)
{
	// There is no need to lock it.
	AosIILPtr iil;
	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(wordid, iilid, isPersis, iilname, iilmgrLocked, rdata);
		 break;

	case eAosIILType_Str:
		 {
		 	 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, iilmgrLocked, rdata);
			 int len = iilname.length();
			 const char * name = iilname.data();
			 if(AosDocUtil::isNumAlpha(name, len))
			 {
			 	AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
				striil->setNumAlpha();
			 }
		 }
		 break;
	
	case eAosIILType_NumAlpha:
		 {
			 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, iilmgrLocked, rdata);
			 AosIILStrPtr striil = (AosIILStr*)iil.getPtr();
			 striil->setNumAlpha();
		 }
		 break;
	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(wordid, iilid, isPersis, iilname, iilmgrLocked, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Type: " << iiltype << enderr;
		 return 0;
	}

	aos_assert_r(iil && iil->isGood(), 0);
	iil->setSiteid(siteid);
	return iil;
}


/*
bool
AosIIL::staticReadHeaderBuff(
		const u64 &iilid, 
		const u32 &siteid, 
		AosBuff &buff, 
		const AosRundataPtr &rdata)
{
	// This function reads the iil from the file. The data is
	// set to 'buff'. 
	sgLock.lock();
	
	int vid = 0;
	u64 localid = 0;
	bool rslt = AosIILMgr::getSelf()->parseIILID(iilid, localid, vid, rdata);
	if(!rslt)
	{
		sgLock.unlock();
		return false;
	}

	// bool isPersis = isPersisIIL(iilid);
	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	if(!docFileMgr) 
	{
		sgLock.unlock();
		return false;
	}
	
	u32 seqno = localid / sgMaxHeaderPerFile;
	u32 offset = (localid % sgMaxHeaderPerFile) * eIILHeaderSize;
	int len;
	char *data = buff.data();
	
	if (!docFileMgr->readHeader(seqno, offset, data, eIILHeaderSize-sizeof(len), len, rdata))
	{
		sgLock.unlock();
		return false;
	}

	if (len <= 0 || len > buff.buffLen())
	{
		OmnAlarm << "Expect reading: " << buff.buffLen()
			<< ", but read only: " << len << enderr;
		sgLock.unlock();
		return false;
	}
	buff.setDataLen(len);

	sgLock.unlock();
	return true;
}
*/


// Ketty 2012/10/29
// moved to IILMgr.
AosDocFileMgrObjPtr
AosIIL::getDocFileMgr(
		const u64 &iilid,
		u64 &local_iilid,
		const AosRundataPtr &rdata)
{
	return AosIILMgr::getSelf()->getDocFileMgr(iilid, local_iilid, rdata);
}


///////////////////////////////////////////////////////////
// 			Thread-Safe Functions						 //
///////////////////////////////////////////////////////////

bool
AosIIL::loadFromFileSafe1(
		const u64 &iilid, 
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	//bool rslt = loadFromFileLocked(iilid, iilmgrLocked, rdata);
	bool rslt = loadFromFilePriv(iilid, iilmgrLocked, rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


// Ketty 2013/01/15
//bool
//AosIIL::finishTransSafe(const AosRundataPtr &rdata)
//{
//	OmnShouldNeverComeHere;
//	return false;
//	return AosIILMgr::getSelf()->finishTrans(mTrans, rdata);
//}


bool
AosIIL::saveToFileSafe(
		const bool iillocked,
		const bool iilmgrLocked,
		const bool forcesave, 
		const AosRundataPtr &rdata)
{
	// This function saves the changes to files. An IIL may
	// contain multiple subiils. This function should be
	// called by outside functions. It normally needs to 
	// lock the instance. If 'forcesave' is true, it does not
	// lock the class.

	bool rslt = false;

	//aos_assert_r((isRootIIL() || isSingleIIL())&& !isCompIIL(), false);
	//aos_assert_r(!isLeafIIL(), false);
	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	if (!forcesave && !iillocked) 
	{
		AOSLOCK(mLock);
	}

	if (!mIsDirty)
	{
		//if(isRootIIL())
		//{
		//	//return all the subiil
		//	bool subreturned = false;
		//	returnSubIILsPriv(true, subreturned, rdata);
		//	aos_assert_r(subreturned, false);
		//}
		if (!forcesave && !iillocked)
		{
			AOSUNLOCK(mLock);
		}
		// Ketty 2012/12/31
		//int ss = mTrans.size();
		//if(ss > 0)
		//{
		//	finishTransSafe(rdata);
		//}
		//clearTrans();
		return true;
	}

	// Ketty 2013/01/15
	// maybe this rdata is come from--IILMgr:savingThread, It's not't set the siteid.
	rdata->setSiteid(mSiteid);
	rslt = saveToFilePriv(iilmgrLocked,forcesave,rdata);

	// Ketty 2013/01/22
	//u64 t1 = OmnGetTimestamp();
	//AosIILMgr::getSelf()->returnDocTrans(docTrans);
	//u32 lllen = mTrans.size();
	//clearTrans();
	//u64 t2 = OmnGetTimestamp();
	//if (t2 - t1 > 1000000)
	//{
	//	OmnScreen << "Clear transactions too slow. IILID:" << mIILID 
	//		<< ", Time: " << t2 - t1 << ". Num Transactions: " << lllen << endl;
	//}
	
	if (!forcesave && !iillocked) 
	{
		AOSUNLOCK(mLock);
	}
	return rslt;
}


bool
AosIIL::saveToLocalFileSafe(
		const bool forcesave, 
		const AosRundataPtr &rdata)
{
	if (!forcesave) 
	{
		AOSLOCK(mLock);
	}
	bool rslt = saveToLocalFilePriv(forcesave, rdata);
	if (!forcesave) 
	{
		AOSUNLOCK(mLock);
	}	
	return rslt;
}	


// Ketty 2013/01/08
bool
AosIIL::saveToFilePriv(
		const bool iilmgrLocked,
		const bool forcesave,
		const AosRundataPtr &rdata) 
{
	//1. save subiils to local file
	//2. save compiil subiils to local file
	//3. save to local file

	aos_assert_r((isRootIIL()||isSingleIIL()) && !isCompIIL(), false);
	
	bool rslt = false;
	if (isRootIIL())
	{
		rslt = saveSubIILToLocalFileSafe(forcesave, rdata);
		aos_assert_r(rslt, false);
	}

	// Ketty 2012/11/30. save sub first. then save to localfile.
	//4. save to local file
	rslt = saveToLocalFilePriv(forcesave, rdata);
	aos_assert_r(rslt, false);
	return true;
}


void
AosIIL::resetSafe(
		const u64 &wordid, 
		const u64 &iilid,
		const u32 siteid,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	mWordId = wordid;
	mIILID = iilid;
	mSiteid = siteid;
	resetSelf(iilmgrLocked, rdata);
	AOSUNLOCK(mLock);
}


u64
AosIIL::getDocIdSafe1(int &idx,
					 int &iilidx,
					 const AosRundataPtr &rdata)
{
	return getDocIdSafe(idx,rdata);
}

u64
AosIIL::nextDocIdSafe2(int &idx,int &iilidx,const bool reverse)
{
	AOSLOCK(mLock);
	u64 did = nextDocIdPriv2(idx,iilidx,reverse);
	AOSUNLOCK(mLock);
	return did;
}


u64
AosIIL::nextDocIdPriv2(int &idx,int &iilidx,const bool reverse)
{
	// if the iil is empty
	if(mNumDocs <= 0)
	{
		idx = -5;
		iilidx = -5;	// Chen Ding, 12/23/2010
		return AOS_INVDID;
	}

	iilidx = mIILIdx;
	if(idx == -10)
	{
		// need a new one
		if(reverse)
		{
			idx = mNumDocs -1;
			return mDocids[idx];
		}
		else
		{
			idx = 0;
			return mDocids[0];
		}
	}
	else
	{
		aos_assert_r(idx >= 0,AOS_INVDID)
		if(reverse)
		{
			idx --;
		}
		else
		{
			idx ++;
		}
	}

	if(idx >= 0 && idx <= (int)mNumDocs)
	{
		return mDocids[idx];
	}
	
	// idx out of range
	idx = -5;
	iilidx = -5;
	return AOS_INVDID;
}


u64
AosIIL::getDocIdSafe(const int idx,const AosRundataPtr &rdata) const
{
	// This function retrieves the 'idx' docID from the list. 
	// Each IIL is an array of DocIds. Each DocId is eight 
	// bytes long. There is a max size of which the list is 
	// loaded into memory. If this max size is exceeded, the
	// list needs to be loaded in segments. In the current
	// implementation, segmentation is not supported.
	AOSLOCK(mLock);
	u64 did = getDocIdPriv(idx,rdata);
	AOSUNLOCK(mLock);
	return did;
}

u64
AosIIL::getDocIdPriv(const int idx,const AosRundataPtr &rdata) const
{
	if (!mDocids) 
	{
		return AOS_INVDID;
	}

	if (idx >= 0 && (u32)idx < mNumDocs)
	{
		u64 did = mDocids[idx];
		return did;
	}
	return AOS_INVDID;
}


bool
AosIIL::docExistSafe(const u64 &docid, const AosRundataPtr &rdata) const
{
	AOSLOCK(mLock);
//	bool rslt = docExistSafe(docid, rdata);
	AOSUNLOCK(mLock);
	OmnNotImplementedYet
	return false;
}


///////////////////////////////////////////////////////////
// 			Functions called by IILMgr only				 //
// 	These functions should be considered thread-safe	 //
// 	as long as IILMgr ensures the mutual calling of them //
///////////////////////////////////////////////////////////
u32
AosIIL::addRefCountByIILMgr()
{
	// This function shall be called by IILMgr only. 
	// In addition, AosIILMgr::mLock should be locked
	// when this function is called.
	mRefcount++;
	sgNumIILRefs++;
	return mRefcount;
}

u32
AosIIL::removeRefCountByIILMgr()
{
	// Someone finished the use of the IIL. It decrements the
	// reference count. When the reference count is 0, it
	// returns the IIL to IILMgr.
	//
	// This function should be called by AosIILMgr only.
	// When this function is called, AosIILMgr::mLock should
	// have been locked.
	aos_assert_r(mRefcount > 0, 0);
	mRefcount--;
	sgNumIILRefs--;
	return mRefcount;
}


/*
bool
AosIIL::setPtrToWordHashByIILMgr()
{
	// !!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!
	// This function should be called by IILMgr only and its
	// mLock is locked whenever this function is called. This
	// means that it is safe not to lock this class to 
	// modify the pointer. 
	WordPtr pp;
	pp.id = mIILID;
	pp.pointer = (ptoi)this;
	return AosWordClient::getSelf()->setPtr1(mWordId, pp);
}


bool
AosIIL::resetWordPtrByIILMgr()
{
	// !!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!
	// This function should be called by IILMgr only and its
	// mLock is locked whenever this function is called. This
	// means that it is safe not to lock this class to 
	// modify the pointer. 
	//
	// It associates a wordID with an IIL.
	if (mWordId == AOS_INVWID) return true;
	if (mIILType == eAosIILType_Docid) return true;
	WordPtr pp;
	pp.id = mIILID;
	return AosWordClient::getSelf()->setPtr1(mWordId, pp);
}
*/



///////////////////////////////////////////////////////////
// 			Private functions							 //
//  All private functions shall not call any 			 //
//  thread-safe functions and shall not lock the lock.	 //
///////////////////////////////////////////////////////////
bool
AosIIL::returnMemory(const char *mem, const u32 size) 
{
	OmnDelete [] mem;
	return true;
}


char *
AosIIL::getMemory(const u32 newsize, 
		const char *oldmem, 
		const u32 oldsize)
{
	aos_assert_r(newsize > oldsize, 0);
	char *mem = OmnNew char[newsize+10];
	aos_assert_r(mem, 0);
	memset(mem, 0, newsize);

	if (oldmem)
	{
		// Copy the contents from the old buffer to 'mem'
		memcpy(mem, oldmem, oldsize);
		OmnDelete [] oldmem;
	}

	return mem;
}


bool
AosIIL::adjustMemoryProtected()
{
	if (mNumDocs < mMemCap) return true;
	u32 newcap = mNumDocs + mExtraDocidsInit;
	u64 *newmem = OmnNew u64[newcap+10];
	aos_assert_r(newmem, 0);
	if(mDocids)
	{
		OmnDelete []mDocids;
		mDocids = 0;
	}
	mDocids = newmem;
	mMemCap = newcap;
	return true;
}


AosBuffPtr
AosIIL::getBodyBuffMem(const u32 size) const
{
	AosBuffPtr buff = OmnNew AosBuff(size, 0 AosMemoryCheckerArgs);
	return buff;
}


void
AosIIL::resetSelf(const bool iilmgrLocked, const AosRundataPtr &rdata)
{
	// This function is called when it needs to create a new
	// IIL and it is reusing an existing IIL.
	// ModifyNumDocs
    bool rslt = false;
	rslt = resetSpec();
	aos_assert(rslt);

	rslt = resetSubIILInfo(rdata);
	aos_assert(rslt);

	mNumDocs = 0;
	mFlag = AOS_ACTIVE_FLAG;
	mHitcount = 0;
	mCompressedSize = 0;

	mRootIIL = 0;
	
	mMemCap = 0;
	if(mDocids)
	{
		OmnDelete [] mDocids;
		mDocids = 0;
	}

	if(mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if(mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	mDfmDoc = 0;		// Ketty 2013/01/07
	mTrans.clear();		// Ketty 2013/01/11
}


void 
AosIIL::failInLoadProtection()
{
	failInSetCtntProtection();
	mNumDocs = 0;
	mIILID = 0;
    mWordId = 0;
    mHitcount = 0;
    mVersion = 0;
	mIsPersis = false;
	
	mCompressedSize = 0;
	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = -1;

	// Set the contents
	mIsDirty = false;
	mIsNew = false;
}


void
AosIIL::failInSetCtntProtection()
{
}


bool
AosIIL::loadFromFilePriv(
		const u64 &iilid,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
{
	// This function loads the IIL from file.
	mIILID = iilid;
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_rr(docfilemgr, rdata, false);

	// Ketty 2013/01/14
	if(!mDfmDoc)
	{
		AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		bool rslt = docfilemgr->readDoc(rdata, local_iilid, doc);
		aos_assert_rr(rslt, rdata, false);
		mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
	}

	if (!mDfmDoc->doesDocExist())
	{
		failInLoadProtection();
		AosSetErrorU(rdata, "iil_not_found") << ": " << mIILID << enderr;
		return false;
	}

	bool rslt = readFromDfmDoc(mDfmDoc, iilmgrLocked, rdata);
	aos_assert_r(rslt, false);

	mIsDirty = false;
	mIsNew = false;
	return true;
}


bool
AosIIL::expandDocidMem()
{
	// This function expands the space for the IIL. The new memory
	// will be (mNumDocs + mExtraDocids)*8.
	
	u32 newcapacity = mNumDocs + mExtraDocids;
	if (newcapacity <= mMemCap) return true;
	u32 newsize = newcapacity << 3;

	char *mem = getMemory(newsize, (char *)mDocids, (mMemCap << 3));
	aos_assert_r(mem, false);
	mDocids = (u64 *)mem;
	mMemCap = newcapacity;
	return true;
}


/*
bool
AosIIL::getDocidBody(const AosBuffPtr &buff) const
{
	for (u32 i=0; i<mNumDocs; i++)
	{
		*buff << mDocids[i];
	}
	return true;
}
*/


bool
AosIIL::setDocidContents(AosBuffPtr &buff)
{
	// This function sets its contents from 'buff', which 
	// was read from a file. 'buff' should be arranged
	// as:
	// 	[min_docid][max_docid][docid][docid]...[docid]
	// The number of docids is mNumDocs
	// Note that this class may be reused, which means it may
	// already have memory assigned.
	//
	// IMPORTANT: If it ever returns false, 'mNumDocs' will be set to 0.
	
	int crtidx = buff->getCrtIdx();
	if (crtidx < 0)
	{
		OmnAlarm << "Serious problem: Data incorrect: " << crtidx << enderr;
		// ModifyNumDocs
		mNumDocs = 0;
		if (mDocids) memset(mDocids, 0, (mNumDocs << 3));
		return false;
	}

	u32 datalen = mNumDocs << 3;
	if ((u32)buff->buffLen() < (u32)crtidx + datalen)
	{
		OmnAlarm << "Serious problem: Buff too short: " << buff->buffLen()
			<< ":" << crtidx << ":" << datalen << enderr;
	
		// ModifyNumDocs
		mNumDocs = 0;
		if (mDocids) memset(mDocids, 0, (mNumDocs << 3));
		return false;
	}

	if (mNumDocs > 0)
	{
		char *data = buff->data();
		memcpy(mDocids, &data[crtidx], (mNumDocs << 3));
		buff->incIndex(mNumDocs<<3);
	}
	return true;
}


bool
AosIIL::saveSanityCheck(const AosIILPtr &iil, const AosRundataPtr &rdata) 
{

	// It assumes it just wrote the contents into the files. 
	// This function reads all the contents back to check 
	// whether they are correct.
	iil->mIILID = mIILID;
	if (!iil->loadFromFileSafe1(mIILID, true, rdata))
	{
		OmnAlarm << "Failed to read from file" << enderr;
		return false;
	}
	aos_assert_r(mIILType == iil->mIILType, false);
	if (mIILID != iil->mIILID)
	{
		OmnAlarm << "IILID mismatch: " << mIILID << ":" << iil->mIILID << enderr;
		return false;
	}
	aos_assert_r(mWordId == iil->mWordId, false);

	aos_assert_r(mNumDocs == iil->mNumDocs, false);
	aos_assert_r(mFlag == iil->mFlag, false);
	aos_assert_r(mHitcount == iil->mHitcount, false);
	aos_assert_r(mCompressedSize == iil->mCompressedSize, false);

	if (mNumDocs == 0) return true;

	for (u32 i=0; i<mNumDocs; i++)
	{
		aos_assert_r(mDocids[i] == iil->mDocids[i], false);
	}
	return true;
}


bool
AosIIL::prepareMemoryForReloading()
{
	return adjustMemoryProtected();
}


bool
AosIIL::numEntriesIncrease(const int index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && (u32)index < mNumSubiils, false);
	mNumEntries[index] ++;
	return true;
}

bool
AosIIL::numEntriesDecrease(const int index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && index < (int)mNumSubiils, false);
	mNumEntries[index] --;
	return true;
}


bool
AosIIL::resetSubIILInfo(const AosRundataPtr &rdata)
{
	return true;
}

bool
AosIIL::resetSpec()
{
	return true;
}

bool
AosIIL::moveTo(const int startIdx,int &idx,int &iilidx)
{
	if(isLeafIIL()||isSingleIIL())
	{
		idx = startIdx;
		iilidx = -1;
		return true;
	}

	idx = startIdx;
	iilidx = 0;
	while(idx > mNumEntries[iilidx] && (u32)iilidx < mNumSubiils -1)
	{
		idx -= mNumEntries[iilidx];
		iilidx ++;
	}
	return true;
}


bool
AosIIL::copyDocidsPriv(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const int start,
		const int end)
{
	// Chen Ding, 12/15/2010
	aos_assert_r(mDocids, false);
	aos_assert_r(end < 0 || (u32)end < mNumDocs, false);

	int s = start;
	if(s<0)s = 0;
	int e = end;
	if(e<0)e = mNumDocs -1;
	int size = e - s + 1;
	aos_assert_r(size>=0,false);
	
	if(!query_rslt && query_bitmap)
	{
		query_bitmap->appendBlock(&(mDocids[s]),size);
		return true;
	}
	
	bool rslt = true;	
	u64 docid = 0;
	if(query_rslt)
	{
		for(int i = s;i <= e;i++)
		{
			docid = mDocids[i];
			// filter by bitmap
			if(query_bitmap && (!query_bitmap->checkDoc(docid))) continue;
			query_rslt->appendDocid(docid);
			//rslt = query_rslt->appendBlock(&(mDocids[s]),size);
		}
	}

	return rslt;
}


bool
AosIIL::staticIsIILIDUsed(
		const u64 &iilid, 
		const AosRundataPtr &rdata)
{
	// This function checks whether 'iilid' was used.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_rr(docfilemgr, rdata, true);

	// Ketty 2013/01/14
	AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	bool rslt = docfilemgr->readDoc(rdata, local_iilid, doc, false);
	aos_assert_rr(rslt, rdata, false);

	AosDfmDocIILPtr iil_doc = (AosDfmDocIIL*)doc.getPtr();
	if (!iil_doc->doesDocExist()) return false;

	AosIILType iiltype = iil_doc->getIILType();
	if (iiltype <= eAosIILType_Invalid || iiltype >= eAosIILType_Total)
	{
		return false;
	}
	if(iilid != iil_doc->getIILID())	return false;

	return true;
}


bool
AosIIL::staticIsIILCreated(
		const u64 &iilid,
		const u32 siteid,
		const AosIILType type,
		const AosRundataPtr &rdata)
{
	// This func is almost the same as staticIsIILIDUsed
	// Ketty 2013/01/15
	return staticIsIILIDUsed(iilid, rdata);
}


#if 0
bool
AosIIL::saveToFileSafeStatic(
		const u64 &root_iilid,
		const u32 siteid, 
		const AosBuffPtr &headbuff,
		const AosBuffPtr &bodybuff,
		const AosRundataPtr &rdata) 
{

OmnScreen << "recover iil, " << root_iilid << endl;

	aos_assert_r(headbuff && bodybuff, false);

	AosIILType iiltype = eAosIILType_Invalid;

	u64 iilid = 0;
	u64 wordid = 0;
	u32 num_docs = 0;
	char flag = 0;
	u32 hitcount = 0;
	u32 ver = 0;
	u32 disksize = 0;
	u64 offset = 0;
	u32 seqno = 0;
	u32 compressed_size = 0;
	u32 comp_disksize = 0;
	u64 comp_offset = 0;
	u32 comp_seqno = 0;
	u32 comp_compressed_size = 0;
		
	bool isCompRootIIL = false;
	bool isCompLeafIIL = false;
	int headLen = headbuff->dataLen();
	if(headLen == (eFullIILHeaderContentSize - eNormalIILHeaderContentSize))
	{
		isCompRootIIL = true;
		iilid = root_iilid;
		comp_disksize = headbuff->getU32(0);
		comp_offset = headbuff->getU64(0);
		comp_seqno = headbuff->getU32(0);
		comp_compressed_size = headbuff->getU32(0);
	}
	else
	{
		iiltype = (AosIILType)headbuff->getChar(eAosIILType_Invalid);
		iilid = headbuff->getU64(0);
		wordid = headbuff->getU64(0);
		num_docs = headbuff->getU32(0);
		flag = headbuff->getChar(0);
		hitcount = headbuff->getU32(0);
		ver = headbuff->getU32(0);
		disksize = headbuff->getU32(0);
		offset = headbuff->getU64(0);
		seqno = headbuff->getU32(0);
		compressed_size = headbuff->getU32(0);
		if(iiltype == eAosIILType_CompStr || iiltype == eAosIILType_CompU64)
		{
			isCompLeafIIL = true;
			comp_disksize = headbuff->getU32(0);
			comp_offset = headbuff->getU64(0);
			comp_seqno = headbuff->getU32(0);
			comp_compressed_size = headbuff->getU32(0);
		}
	}

	// Save the body buff
	int vid = 0;
	u64 localid = 0;
	bool rslt = AosIILMgr::getSelf()->parseIILID(iilid, localid, vid, rdata);
	aos_assert_r(rslt, false);

	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	aos_assert_r(docFileMgr, false);

	bool is_compress = false;
	if(isCompRootIIL || isCompLeafIIL)
	{
		is_compress = isCompress(comp_disksize);
		rslt = docFileMgr->saveDoc(comp_seqno, comp_offset, comp_disksize, bodybuff->data(), is_compress, comp_compressed_size, rdata);
		//OmnScreen << "save comp iil body, iilid:" << iilid << ", seqno:" << comp_seqno << ", offset:" << comp_offset << ", disksize:" << comp_disksize << endl;
	}
	else
	{
		is_compress = isCompress(disksize);
		rslt = docFileMgr->saveDoc(seqno, offset, disksize, bodybuff->data(), is_compress, compressed_size, rdata);
		//OmnScreen << "save iil body, iilid:" << iilid << ", seqno:" << seqno << ", offset:" << offset << ", disksize:" << disksize << endl;
	}
	aos_assert_r(rslt, false);
	// rslt = savingSanityCheck(docFileMgr, seqno, testOffset, offset, bodybuff->data(), 
	// 		disksize, compressed_size, rdata);

	// Reconstruct the header buff
	u32 header_seqno = localid / sgMaxHeaderPerFile;
	u64 header_offset = (localid % sgMaxHeaderPerFile) * eIILHeaderSize;

	char bb[eIILHeaderMemsize+10];
	AosBuff buff(bb, eIILHeaderMemsize, 0, 0 AosMemoryCheckerArgs);
	if(isCompRootIIL)
	{
		buff.setU32(comp_disksize);
		buff.setU64(comp_offset);
		buff.setU32(comp_seqno);
		buff.setU32(comp_compressed_size);
			
		int offset2 = header_offset + sizeof(int) + eNormalIILHeaderContentSize;
		rslt = docFileMgr->saveHeaderSegment(
						header_seqno, header_offset, eFullIILHeaderContentSize, offset2,
						buff.data(), buff.dataLen(), true, rdata);
	}
	else
	{
		buff.setU8((u8)iiltype);
		buff.setU64(iilid);
		buff.setU64(wordid);
		buff.setU32(num_docs);
		buff.setChar(flag);
		buff.setU32(hitcount);
		buff.setU32(ver);
		buff.setU32(disksize);
		buff.setU64(offset);
		buff.setU32(seqno);
		buff.setU32(compressed_size);
		
		if(isCompLeafIIL)
		{
			buff.setU32(comp_disksize);
			buff.setU64(comp_offset);
			buff.setU32(comp_seqno);
			buff.setU32(comp_compressed_size);
		}
		rslt = docFileMgr->saveHeaderSegment(
					header_seqno, header_offset, eFullIILHeaderContentSize, header_offset + sizeof(int),
					buff.data(), buff.dataLen(), true, rdata);
	}
	aos_assert_r(rslt, false);
	return true;
}
#endif


// Chen Ding, 09/30/2011
void 
AosIIL::setLastAddTimestamp(const bool add_to_head)
{
	mLastAddSec = OmnGetTimestamp();
	mLastAddToHead = add_to_head;
}


void 
AosIIL::addTrans(const AosIILTransPtr &trans)
{
	// All transactions processed by an IIL are stored (temporarily) in mTrans.
	// When this queue becomes too long, it will save the contents into disks.

	u64 gid = trans->getTransId();
	// u32 lid = trans->getLocalTransId();
	AOSLOCK(mLock);
	mTrans.push_back(gid);
	AOSUNLOCK(mLock);
}


bool
AosIIL::staticCheckIILIDs(
		const u64 &iilid, 
		const u32 siteid,
		int &num,
		vector<bool> &iilid_states, 
		const AosRundataPtr &rdata)
{
	// This function checks whether [iilid, iilid + len] are used. 
	// It updates the states in 'iilid_states':
	// 		iilid_states[0] for iilid
	// 		iilid_states[1] for iilid + 1
	// 		...
	// 		iilid_states[1] for iilid + num-1 
	// 'num' should not be bigger than 'sgMaxIdsPerBatch', otherwise, it is
	// reset to 'sgMaxIdsPerBatch'.
	static int sgMaxIdsPerBatch = 1000;

	if (num <= 0)
	{
		OmnAlarm << "num is negative: " << num << enderr;
		num = sgMaxIdsPerBatch;
	}
	else if (num > sgMaxIdsPerBatch)
	{
		OmnAlarm << "num is too big: " << num << enderr;
	}

	const int buffsize = eIILHeaderSize*num;
	iilid_states.resize(num);
	try
	{
		char *bb = OmnNew char[buffsize+10];
		AosBuff buff(bb, buffsize, 0, 0 AosMemoryCheckerArgs);
		
		// Ketty 2013/01/15
		// OmnNotImplementedYet;
		//if (!staticReadHeaders(iilid, siteid, num, buff, rdata)) 
		{
			delete [] bb;
			return false;
		}

		for (int i=0; i<num; i++)
		{
			int crt_idx = i * eIILHeaderSize;
			if (crt_idx < buff.dataLen())
			{
				buff.setCrtIdx(crt_idx);
				buff.getInt(0);
				AosIILType iiltype = (AosIILType)buff.getChar(eAosIILType_Invalid);
				u64 id = buff.getU64(0);
				if (iiltype <= eAosIILType_Invalid || 
					iiltype >= eAosIILType_Total ||
					iilid != id)
				{
					iilid_states[i] = true;
				}
				else 
				{
					iilid_states[i] = false;
				}
			}
			else
			{
				iilid_states[i] = true;
			}
		}
	}

	catch (...)
	{
		OmnAlarm << "Failed allocating memory: " << num << enderr;
		return false;
	}

	return true;
}


/*
bool
AosIIL::staticReadHeaders(
		const u64 &iilid, 
		const u32 &siteid, 
		const int num, 
		AosBuff &buff, 
		const AosRundataPtr &rdata)
{
	// This function reads a number of iil headers from the file. The data is
	// set to 'buff'. 
	sgLock.lock();
	
	int vid = 0;
	u64 localid = 0;
	bool rslt = AosIILMgr::getSelf()->parseIILID(iilid, localid, vid, rdata);
	if(!rslt)
	{
		sgLock.unlock();
		return false;
	}

	// Chen Ding, 11/08/2011
	// AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, true, rdata);
	// bool isPersis = isPersisIIL(iilid);
	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	if(!docFileMgr) 
	{
		sgLock.unlock();
		return false;
	}
	
	u32 seqno = localid / sgMaxHeaderPerFile;
	u32 offset = (localid % sgMaxHeaderPerFile) * eIILHeaderSize;
	int len = num * eIILHeaderSize;
	char *data = buff.data();
	
	int max = len;
	if (!docFileMgr->readHeaders(seqno, offset, data, max, len, rdata))
	{
		sgLock.unlock();
		return false;
	}

	if (len <= 0)
	{
		buff.setDataLen(0);
	}
	else
	{
		buff.setDataLen(len);
	}
	sgLock.unlock();
	return true;
}
*/


bool
AosIIL::savingSanityCheck(
		const AosDocFileMgrObjPtr &docFileMgr,
		const u32 seqno, 
		const u64 &offset_old,
		const u64 &offset, 
		const char *data, 
		const u32 docsize, 
		const u32 compressed_size,
		const AosRundataPtr &rdata)
{
	// Ketty 2012/10/30
	return true;
	OmnNotImplementedYet;
	/*
	aos_assert_r(offset, false);
	if (offset_old == 0)
	{
		// It is creating a new. Need to make sure its seqno matches the
		// block size.
		aos_assert_r(docFileMgr->verifySeqno(seqno, docsize), false);
	}

	// Read the data
	char *buff = OmnNew char[docsize+10];
	bool rslt = docFileMgr->readDoc(seqno, offset, buff, docsize, compressed_size, rdata);
	if (!rslt)
	{
		OmnAlarm << "Failed the sanity checking" << enderr;
		delete [] buff;
		return false;
	}
	int nn = memcmp(buff, data, docsize);
	delete buff;
	aos_assert_r(nn == 0, false);
	return true;
	*/
}


u64		
AosIIL::getIILIDForHash()
{
	if (isCompIIL())
	{
		return AosSiteid::combineSiteid(mIILID | eCompIILBitFlag,mSiteid);
	}
	return AosSiteid::combineSiteid(mIILID,mSiteid);
}


u64		
AosIIL::getIILIDWithCompFlag() 
{
	if (isCompIIL())
	{
		return mIILID | eCompIILBitFlag;
	}
	return mIILID;
}


bool	
AosIIL::queryRangeSafe(
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &query_bitmap,
			const AosQueryContextObjPtr &query_context,
			const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosIIL::deleteFromLocalFilePriv(
		const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its 
	// subiil even if it is a root.
	u64 local_iilid;
	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docFileMgr, false);

	if(!mDfmDoc)
	{
		AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		bool rslt = docFileMgr->readDoc(rdata, local_iilid, doc, false);
		aos_assert_rr(rslt, rdata, false);

		mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
	}
	if (!mDfmDoc->doesDocExist()) return false;

	aos_assert_rr(mDfmDoc->getDocid() == local_iilid, rdata, false);

	bool rslt = docFileMgr->deleteDoc(rdata, mTrans, mDfmDoc);
	aos_assert_r(rslt, false);

	return true;
}


// Ketty, 10/29/2012
bool
AosIIL::saveToLocalFilePriv(
		const bool forcesave, 
		const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its
	// subiil even if it is a root.
	// 3. Save the body

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_rr(docfilemgr, rdata, false);

	if(!mDfmDoc)
	{
		AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		bool rslt = docfilemgr->readDoc(rdata, local_iilid, doc, false);
		aos_assert_rr(rslt, rdata, false);
		mDfmDoc = (AosDfmDocIIL *)doc.getPtr();

		if(!mDfmDoc->doesDocExist())
		{
			// means maybe it's a new iil.
			setNewDfmDoc(mDfmDoc);
		}
	}

	bool rslt = resetDfmDoc(mDfmDoc);
	aos_assert_rr(rslt, rdata, false);

	rslt = docfilemgr->saveDoc(rdata, mTrans, mDfmDoc);
	aos_assert_rr(rslt, rdata, false);

	mIsDirty = false;
	mIsNew = false;

	// Ketty 2012/11/30
	clearTrans();
	return true;
}


// Ketty 2013/01/15
void
AosIIL::setNewDfmDoc(const AosDfmDocIILPtr doc)
{
	doc->setIILID(mIILID);
	doc->setWordId(mWordId);
	doc->setIILType(mIILType);
	doc->setVersion(mVersion, mIsPersis);
}


// Ketty 2013/01/15
bool
AosIIL::resetDfmDoc(const AosDfmDocIILPtr &doc)
{
	aos_assert_r(doc, false);

	doc->setHitCount(mHitcount);
	doc->setNumDocs(mNumDocs);
	doc->setFlag(mFlag);

	AosBuffPtr bodyBuff = getBodyBuffProtected();
	aos_assert_r(bodyBuff, false);
	mDfmDoc->setBodyBuff(bodyBuff);

	// Chen Ding DDDDD, 2013/01/26
	mDfmDoc->setNeedCompress( isCompress(bodyBuff->dataLen()) );

	return true;
}


// Ketty 2013/01/15
bool
AosIIL::readFromDfmDoc(
		const AosDfmDocIILPtr &doc,
		const bool iilmgrLocked, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc->getIILType() == mIILType, false);
	if(doc->getIILID() != mIILID)
	{
		OmnAlarm << "IILID mismatch: " << ":" << mIILID << enderr;
		failInLoadProtection();
		return false;
	}

	mWordId = doc->getWordId();
	mNumDocs = doc->getNumDocs();
	mFlag = doc->getFlag();
	mHitcount = doc->getHitCount();
	mVersion = doc->getVersion();
	mIsPersis = doc->getPersis();

	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = -1;

	AosBuffPtr bodybuff = doc->getBodyBuff();
	aos_assert_r(bodybuff, false);

	// Set the contents
	bodybuff->reset();
	bool rslt = setContentsProtected(bodybuff, iilmgrLocked, rdata);
	if(!rslt)
	{
		OmnAlarm << "Can not read body buff" << enderr;
		failInLoadProtection();
		return false;
	}
	
	return true;
}


bool
AosIIL::isCompress(const u32 &size)
{
	// Chen Ding DDDDD, 2013/01/26
	// When IIL is not big enough, it will not compress. 
	//ken 2012/11/26
	// return false;
	return (smNeedCompress && size >= smCompressSize);
}


bool
AosIIL::staticGetIILType(
		AosIILType &iiltype,
		const u64 &iilid,
		const AosRundataPtr &rdata)
{
	iiltype = eAosIILType_Invalid;

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_rr(docfilemgr, rdata, false);

	// Ketty 2013/01/14
	AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	bool rslt = docfilemgr->readDoc(rdata, local_iilid, doc, false);
	aos_assert_rr(rslt, rdata, false);
	AosDfmDocIILPtr iil_doc = (AosDfmDocIIL *)doc.getPtr();
	if (!iil_doc->doesDocExist()) return true;

	iiltype = iil_doc->getIILType();
	return true;
}


// Chen Ding, 2013/03/01
bool 
AosIIL::computeQueryResults(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const AosQueryContextObjPtr &context, 
		const AosBitmapObjPtr &bitmap, 
		const u64 &query_id, 
		const int physical_id)
{
	OmnShouldNeverComeHere;
	return false;
}


// Chen Ding, 2013/03/03
bool 
AosIIL::retrieveQueryBlock(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosIIL::retrieveNodeList(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query)
{
	OmnShouldNeverComeHere;
	return false;
}
