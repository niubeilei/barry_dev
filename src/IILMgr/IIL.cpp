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
#include "IILMgr/IIL.h"

#include "Debug/ExitHandler.h"
#include "DocUtil/DocUtil.h"
#include "TransUtil/IILTrans.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/IILHit.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "IILMgr/IILBigStr.h"
#include "IILMgr/IILBigU64.h"
#include "IILMgr/IILBigI64.h"
#include "IILMgr/IILBigD64.h"
#include "IILMgr/IILBigHit.h"
#include "IILMgr/JimoTable.h"
//#include "IILMgr/IILCompStr.h"
//#include "IILMgr/IILCompU64.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/Passwords.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/Siteid.h"
#include "StorageMgrUtil/DevLocation.h"
#include "StorageMgr/StorageMgr.h"
#include "IILUtil/IILSave.h"
#include "DfmUtil/DfmDocIIL.h"
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

i64	AosIIL::mExtraDocids = eExtraDocids;
i64	AosIIL::mExtraDocidsInit = eInitNumDocids;
i64	AosIIL::mMaxNormIILSize = 0;
i64	AosIIL::mMaxBigIILSize = 0;
i64	AosIIL::mMinIILSize = 0;
i64	AosIIL::mMaxSubIILs = 0;
i64	AosIIL::mMinSubIILs = 0;
i64	AosIIL::mAddBlockMaxSize = 20000;	// Ken Lee, 2013/01/03
i64	AosIIL::mAddBlockMinSize = 4000;	// Ken Lee, 2013/01/03
i64	AosIIL::mAddBlockFillSize = 16000;	// Ken Lee, 2013/01/03

bool	AosIIL::smNeedCompress = true;		// Chen Ding DDDDD, 2013/01/26
i64	AosIIL::smCompressSize = AosIIL::eCompressSize;		// Chen Ding DDDDD, 2013/01/26

bool 	AosIIL::mIILMergeFlag = false;
i64	AosIIL::mTotalIILs[eAosIILType_Total];
//AosDfmDocIILPtr AosIIL::smDfmDoc;

i64 AosIIL::sgNumIILCreated = 0;
i64 AosIIL::sgNumIILRefs = 0;
OmnMutexPtr AosIIL::sgLock = OmnNew OmnMutex();

AosIIL::AosIIL(const AosIILType type)
:
mRefcount(0),
mIILType(type),
mIILID(AOS_INVIILID),
mWordId(AOS_INVWID),
mNumDocs(0),
mFlag(0),
mHitcount(0),
mVersion(0),
mIsPersis(false),
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
//mCreationOnly(false),
mSiteid(0),
mDebugFlag(eGoodIILFlag),
mSnapShotId(0),
mBRLock(OmnNew OmnMutex())
{
	// OmnScreen << "IIL created: " << this << endl;
	sgLock->lock();
	sgNumIILCreated++;
	sgLock->unlock();

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
//mCreationOnly(false),
mSiteid(0),
mDebugFlag(eGoodIILFlag),
mSnapShotId(0)
{
	// OmnScreen << "IIL created: " << this << endl;
	sgLock->lock();
	sgNumIILCreated++;
	sgLock->unlock();
	
	//ken 2011/08/19
	mTrans.clear();
//OmnScreen << "-------iil be created :" << sgNumIILCreated << endl;
}


AosIIL::~AosIIL()
{
	// OmnAlarm << "IIL Should not be deleted!" << enderr;
	//AosIILMgrSelf->checkIIL(this);
	sgLock->lock();
	sgNumIILCreated--;
	// OmnScreen << "-------iil deleteed :" << mIILID << "," << sgNumIILCreated << ":" << this << endl;
	sgLock->unlock();

	// Chen Ding, 11/06/2012
	if (mDebugFlag != eGoodIILFlag)
	{
		OmnAlarm << "To delete an invalid IIL: " << mDebugFlag << enderr;
		return;
	}
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

	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
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
	//if (sgDocFileMgr) sgDocFileMgr->stop();
	return true;
}


i64 
AosIIL::staticNumIILRefs()
{
	return sgNumIILRefs;
}


bool			
AosIIL::staticInit(const AosXmlTagPtr &theconfig)
{
	// memset(sgIILIDLog, 0, sgMaxIILID_Test*sgNumIILIDs_Test*8);
	// Ketty 2013/01/14
	//smDfmDoc = OmnNew AosDfmDocIIL(0);

	for(int i=0; i<eAosIILType_Total; i++)
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

	mMaxNormIILSize = config->getAttrU64(AOSCONFIG_MAX_NORM_IILSIZE, eMaxIILSize);
	aos_assert_r(mMaxNormIILSize > 0, false);
	aos_assert_r(mMinIILSize < mMaxNormIILSize, false);

	mMaxBigIILSize = config->getAttrU64(AOSCONFIG_MAX_BIG_IILSIZE, eMaxIILSize);
	aos_assert_r(mMaxBigIILSize > 0, false);
	aos_assert_r(mMinIILSize < mMaxBigIILSize, false);

	mMinSubIILs = config->getAttrU64(AOSCONFIG_MIN_SUBIILSIZE, eMinSubIILs);
	aos_assert_r(mMinSubIILs > 0, false);

	mMaxSubIILs = config->getAttrU64(AOSCONFIG_MAX_SUBIILSIZE, eMaxSubIILs);
	aos_assert_r(mMaxSubIILs > 0, false);
	aos_assert_r(mMinSubIILs < mMaxSubIILs, false);

	// Chen Ding DDDDD, 2013/01/26
	smNeedCompress = config->getAttrBool("compress_iil", true);
	i64 nn = config->getAttrInt64("compress_size", eCompressSize);
	if (nn < 0) smCompressSize = eCompressSize;
	else smCompressSize = nn;
	
	// Ken Lee 2013/01/03
	mAddBlockMaxSize = mMaxBigIILSize;
	mAddBlockMinSize = mMaxBigIILSize * 0.2;
	mAddBlockFillSize = mMaxBigIILSize * 0.8;

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


AosIILObjPtr
AosIIL::staticLoadFromFile(
		const u64 &iilid,
		const u64 &snap_id,
		const AosRundataPtr &rdata) 
{
	// It creates a new instance of IIL and load it from file.
	// This function is thread safe. There is no need to lock
	// it. The actual work is done by 'staticReadHeaderBuff(...)'.
	// The header has already been read. It is in 'iil_header'.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, 0);

	// Ketty 2013/01/14
	/*
	AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	bool rslt = docfilemgr->readDoc(rdata, doc);
	aos_assert_r(rslt, 0);
	if (!doc->doesDocExist())	return 0;

	AosDfmDocIILPtr iildoc = (AosDfmDocIIL*)doc.getPtr();
	if (!iildoc->doesDocExist())	return 0;
	*/
	//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid);
	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, true);
	if (!doc) return 0;
	
	AosDfmDocIILPtr iildoc = (AosDfmDocIIL*)doc.getPtr();
	AosIILType iiltype = iildoc->getIILType();

	AosIILObjPtr iil;
	u32 siteid = rdata->getSiteid();

	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_Str:
		 iil = OmnNew AosIILStr(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigI64:
		 iil = OmnNew AosIILBigI64(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigD64:
		 iil = OmnNew AosIILBigD64(iilid, siteid, iildoc, rdata);
		 break;

	case eAosIILType_BigHit:
		 iil = OmnNew AosIILBigHit(iilid, siteid, iildoc, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL type: " << iiltype 
			 << ":" << iilid << enderr;
		 return 0;
	}
	
	aos_assert_r(iil && iil->isGood(), 0);

	iil->setSnapShotId(snap_id);

	return iil;
}


AosIILObjPtr
AosIIL::staticCreateIIL(
	const u64 &wordid,
	const u64 &iilid, 
	const u32 siteid, 
	const AosIILType iiltype,
	const bool isPersis,
	const OmnString &iilname, 
	const AosRundataPtr &rdata)
{
	// There is no need to lock it.
	AosIILObjPtr iil;
	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_Str:
		 {
		 	 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, rdata);
			 const char * name = iilname.data();
			 if (AosDocUtil::isNumAlpha(name, iilname.length()))
			 {
			 	AosIILStrPtr striil = dynamic_cast<AosIILStr *>(iil.getPtr());
				striil->setNumAlpha();
			 }
		 }
		 break;
	
	case eAosIILType_NumAlpha:
		 {
			 iil = OmnNew AosIILStr(wordid, iilid, isPersis, iilname, rdata);
			 AosIILStrPtr striil = dynamic_cast<AosIILStr *>(iil.getPtr());
			 striil->setNumAlpha();
		 }
		 break;
	case eAosIILType_U64:
		 iil = OmnNew AosIILU64(wordid, iilid, isPersis, iilname, rdata);
		 break;
	
	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_BigI64:
		 iil = OmnNew AosIILBigI64(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_BigD64:
		 iil = OmnNew AosIILBigD64(wordid, iilid, isPersis, iilname, rdata);
		 break;

	case eAosIILType_BigHit:
		 iil = OmnNew AosIILBigHit(wordid, iilid, isPersis, iilname, rdata);
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Type: " << iiltype << enderr;
		 return 0;
	}

	aos_assert_r(iil && iil->isGood(), 0);
	iil->setSiteid(siteid);
	return iil;
}


AosIILObjPtr
AosIIL::staticCreateIIL(
	const AosIILType iiltype,
	const AosRundataPtr &rdata)
{
	// There is no need to lock it.
	AosIILObjPtr iil;
	switch (iiltype)
	{
	case eAosIILType_Hit:
		 iil = OmnNew AosIILHit();
		 break;

	case eAosIILType_Str:
		 {
		 	 iil = OmnNew AosIILStr();
		 }
		 break;
	
	case eAosIILType_NumAlpha:
		 {
			 iil = OmnNew AosIILStr();
			 AosIILStrPtr striil = dynamic_cast<AosIILStr *>(iil.getPtr());
			 striil->setNumAlpha();
		 }
		 break;
	case eAosIILType_U64:
		 iil = OmnNew AosIILU64();
		 break;
	
	case eAosIILType_BigStr:
		 iil = OmnNew AosIILBigStr();
		 break;

	case eAosIILType_BigU64:
		 iil = OmnNew AosIILBigU64();
		 break;

	case eAosIILType_BigI64:
		 iil = OmnNew AosIILBigI64();
		 break;

	case eAosIILType_BigD64:
		 iil = OmnNew AosIILBigD64();
		 break;

	case eAosIILType_BigHit:
		 iil = OmnNew AosIILBigHit();
		 break;

	case eAosIILType_JimoTable:
		 iil = OmnNew AosJimoTable();
		 break;

	default:
		 OmnAlarm << "Unrecognized IIL Type: " << iiltype << enderr;
		 return 0;
	}

	aos_assert_r(iil && iil->isGood(), 0);
	return iil;
}


/*
bool
AosIIL::staticReadHeaderBuff(
		const u64 &iilid, 
		const bool create_flag,
		const AosRundataPtr &rdata)
{
	// This function reads the iil header from the file. The header is
	// saved in 'iil_header'. If the iil does not exist, it may create
	// the iil if 'create_flag' is true.
	//
	// !!!!!!!! This function does not need to lock.
	sgLock.lock();
	
	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	if (!docFileMgr) 
	{
		sgLock.unlock();
		return false;
	}

	AosDfmDocIILPtr doc = smDfmDoc;
	rslt = docFileMgr->readHeader(rdata, localid, doc);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_reading") << enderr;
		sgLock.unlock();
		return false;
	}

	if (!doc)
	{
		if (!create_flag)
		{
			sgLock.unlock();
			return false;
		}
		
		AosBuffPtr header_buff = OmnNew AosBuff(eIILHeaderSize, 0 AosMemoryCheckerArgs);
		memset(header_buff->data(), 0, eIILHeaderSize);
		header = AosHeaderEntry::staticCreateHeader(
				docFileMgr, header_buff, localid, AosHeaderEntry::eIIL);
	}
	sgLock.unlock();
	
	aos_assert_r(header, false);
	iil_header = (AosIILHeaderEntry *)header.getPtr();
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
AosIIL::loadFromFileSafe(
		const u64 &iilid, 
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = loadFromFilePriv(iilid, rdata);
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
AosIIL::saveToFileSafe(const AosRundataPtr &rdata)
{
	// This function saves the changes to files. An IIL may
	// contain multiple subiils. This function should be
	// called by outside functions. It normally needs to 
	// lock the instance. If 'forcesave' is true, it does not
	// lock the class.

	aos_assert_r((isRootIIL() || isSingleIIL()), false);
	AOSLOCK(mLock);
	if (!mIsDirty)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	// Ketty 2013/01/15
	// maybe this rdata is come from--IILMgr:savingThread, It's not't set the siteid.
	rdata->setSiteid(mSiteid);
	bool rslt = saveToFilePriv(rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosIIL::saveToLocalFileSafe(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	bool rslt = true;
	if (mIsDirty)
	{
		rslt = saveToLocalFilePriv(rdata);
		mIsDirty = false;
	}
	AOSUNLOCK(mLock);
	return rslt;
}	


// Ketty 2013/01/08
bool
AosIIL::saveToFilePriv(const AosRundataPtr &rdata) 
{
	//1. save subiils to local file
	//2. save compiil subiils to local file
	//3. save to local file
	
	//aos_assert_r(!isLeafIIL() && !isCompIIL(), false);
	if (!isBigIIL())
	{
		bool result = addId(rdata);
		aos_assert_r(result, false);
	}

	bool rslt = false;
	if (isParentIIL())
	{
		rslt = saveSubIILToLocalFileSafe(rdata);
		aos_assert_r(rslt, false);
	}

	// Ketty 2012/11/30. save sub first. then save to localfile.
	//4. save to local file
	rslt = saveToLocalFilePriv(rdata);
	aos_assert_r(rslt, false);

	if(!isBigIIL())
	{
		bool result = commitId(rdata);
		aos_assert_r(result, false);
	}

	return true;
}


void
AosIIL::resetPriv(
		const u64 &wordid, 
		const u64 &iilid,
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	mWordId = wordid;
	mIILID = iilid;
	mSiteid = siteid;
	resetSelf(rdata);
}


void
AosIIL::resetSafe(
		const u64 &wordid, 
		const u64 &iilid,
		const u32 siteid,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	resetPriv(wordid, iilid, siteid, rdata);
	AOSUNLOCK(mLock);
}


u64
AosIIL::getDocIdSafe1(
		i64 &idx,
		i64 &iilidx,
		const AosRundataPtr &rdata)
{
	return getDocIdSafe(idx, rdata);
}


u64
AosIIL::nextDocIdSafe2(
		i64 &idx,
		i64 &iilidx,
		const bool reverse)
{
	AOSLOCK(mLock);
	u64 did = nextDocIdPriv2(idx, iilidx, reverse);
	AOSUNLOCK(mLock);
	return did;
}


u64
AosIIL::nextDocIdPriv2(
		i64 &idx,
		i64 &iilidx,
		const bool reverse)
{
	// if the iil is empty
	if (mNumDocs <= 0)
	{
		idx = -5;
		iilidx = -5;	// Chen Ding, 12/23/2010
		return AOS_INVDID;
	}

	iilidx = mIILIdx;
	if (idx == -10)
	{
		// need a new one
		if (reverse)
		{
			idx = mNumDocs - 1;
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
		if (reverse)
		{
			idx --;
		}
		else
		{
			idx ++;
		}
	}

	if (idx >= 0 && idx <= mNumDocs)
	{
		return mDocids[idx];
	}
	
	// idx out of range
	idx = -5;
	iilidx = -5;
	return AOS_INVDID;
}


u64
AosIIL::getDocIdSafe(
		const i64 &idx,
		const AosRundataPtr &rdata) const
{
	// This function retrieves the 'idx' docID from the list. 
	// Each IIL is an array of DocIds. Each DocId is eight 
	// bytes long. There is a max size of which the list is 
	// loaded into memory. If this max size is exceeded, the
	// list needs to be loaded in segments. In the current
	// implementation, segmentation is not supported.
	AOSLOCK(mLock);
	u64 did = getDocIdPriv(idx, rdata);
	AOSUNLOCK(mLock);
	return did;
}

u64
AosIIL::getDocIdPriv(
		const i64 &idx,
		const AosRundataPtr &rdata) const
{
	if (!mDocids) 
	{
		return AOS_INVDID;
	}

	if (idx >= 0 && idx < mNumDocs)
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
	OmnNotImplementedYet;
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


///////////////////////////////////////////////////////////
// 			Private functions							 //
//  All private functions shall not call any 			 //
//  thread-safe functions and shall not lock the lock.	 //
///////////////////////////////////////////////////////////
bool
AosIIL::returnMemory(const char *mem, const i64 &size) 
{
	OmnDelete [] mem;
	return true;
}


char *
AosIIL::getMemory(
		const i64 &newsize, 
		const char *oldmem, 
		const i64 &oldsize)
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
	i64 newcap = mNumDocs + mExtraDocidsInit;
	u64 *newmem = OmnNew u64[newcap+10];
	aos_assert_r(newmem, 0);
	if (mDocids)
	{
		OmnDelete []mDocids;
		mDocids = 0;
	}
	mDocids = newmem;
	mMemCap = newcap;
	return true;
}


AosBuffPtr
AosIIL::getBodyBuffMem(const i64 &size) const
{
	AosBuffPtr buff = OmnNew AosBuff(size, 0 AosMemoryCheckerArgs);
	return buff;
}


void
AosIIL::resetSelf(const AosRundataPtr &rdata)
{
	// This function is called when it needs to create a new
	// IIL and it is reusing an existing IIL.
	// ModifyNumDocs
    bool rslt = false;
	rslt = resetSpec();
	aos_assert(rslt);

	rslt = returnSubIILsPriv(rdata);
	aos_assert(rslt);

	rslt = resetSubIILInfo(rdata);
	aos_assert(rslt);

	mNumDocs = 0;
	mFlag = AOS_ACTIVE_FLAG;
	mHitcount = 0;
	mCompressedSize = 0;

	mRootIIL = 0;
	
	mMemCap = 0;
	if (mDocids)
	{
		OmnDelete [] mDocids;
		mDocids = 0;
	}

	if (mNumEntries)
	{
		OmnDelete [] mNumEntries;
		mNumEntries = 0;
	}

	if (mIILIds)
	{
		OmnDelete [] mIILIds;
		mIILIds = 0;
	}
	mDfmDoc = 0;		// Ketty 2013/01/07
	mTrans.clear();		// Ketty 2013/01/11
	mSnapShotId = 0;   // Linda, 2014/03/20
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
		const AosRundataPtr &rdata)
{
	// This function loads the IIL from file.
	mIILID = iilid;
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	// Ketty 2013/01/14
	if (!mDfmDoc)
	{
		
		//AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		//bool rslt = docfilemgr->readDoc(rdata, doc);
		//aos_assert_r(rslt, false);
		//mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
		//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid);
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, true);
		if (!doc)
		{
			//failInLoadProtection();
			//AosSetErrorU(rdata, "iil_not_found") << ": " << mIILID << enderr;
			return false;
		}
		mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
	}

	//OmnScreen << "readIIL from file: "
	//		<< " mIILID:" << mIILID
	//		<< "; loc_id:" << mDfmDoc->getDocid()
	//		<< "; bodySeqno:" << mDfmDoc->getBodySeqno()
	//		<< "; bodyOffset:" << mDfmDoc->getBodyOffset()
	//		<< "; bodyLen:" << mDfmDoc->getBodyBuff()->dataLen()
	//		<< endl;

	bool rslt = readFromDfmDoc(mDfmDoc, rdata);
	aos_assert_r(rslt, false);

	// Ketty 2013/06/08 
	mDfmDoc->cleanBodyBuff();
	mIsDirty = false;
	mIsNew = false;
	return true;
}


bool
AosIIL::expandDocidMem()
{
	// This function expands the space for the IIL. The new memory
	// will be (mNumDocs + mExtraDocids)*8.
	
	i64 newcapacity = mNumDocs + mExtraDocids;
	if (newcapacity <= mMemCap) return true;
	i64 newsize = newcapacity << 3;

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
	for (i64 i=0; i<mNumDocs; i++)
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
	
	// Ken Lee, 2013/04/18
/*	i64 crtidx = buff->getCrtIdx();
	if (crtidx < 0)
	{
		OmnAlarm << "Serious problem: Data incorrect: " << crtidx << enderr;
		// ModifyNumDocs
		mNumDocs = 0;
		if (mDocids) memset(mDocids, 0, (mNumDocs << 3));
		return false;
	}

	i64 datalen = mNumDocs << 3;
	if (buff->buffLen() < crtidx + datalen)
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
	*/
	bool rslt = buff->getU64s(mDocids, mNumDocs);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIIL::saveSanityCheck(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata) 
{

	// It assumes it just wrote the contents into the files. 
	// This function reads all the contents back to check 
	// whether they are correct.
	AosIIL * iil = dynamic_cast<AosIIL *>(iilobj.getPtr());
	iil->mIILID = mIILID;
	if (!iil->loadFromFileSafe(mIILID, rdata))
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

	for (i64 i=0; i<mNumDocs; i++)
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
AosIIL::numEntriesIncrease(const i64 &index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && index < mNumSubiils, false);
	mNumEntries[index]++;
	return true;
}

bool
AosIIL::numEntriesDecrease(const i64 &index)
{
	aos_assert_r(mNumEntries,false);
	aos_assert_r(index>=0 && index < mNumSubiils, false);
	mNumEntries[index]--;
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
AosIIL::moveTo(const i64 &startIdx, i64 &idx, i64 &iilidx)
{
	if (!isParentIIL())
	{
		idx = startIdx;
		iilidx = -1;
		return true;
	}

	idx = startIdx;
	iilidx = 0;
	while(idx > mNumEntries[iilidx] && iilidx < mNumSubiils -1)
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
		const i64 &start,
		const i64 &end)
{
	// Chen Ding, 12/15/2010
	aos_assert_r(mDocids, false);
	aos_assert_r(end < 0 || end < mNumDocs, false);

	i64 s = start;
	if (s<0)s = 0;
	i64 e = end;
	if (e<0)e = mNumDocs -1;
	i64 size = e - s + 1;
	aos_assert_r(size>=0,false);
	
	if (!query_rslt && query_bitmap)
	{
		query_bitmap->appendDocids(&(mDocids[s]),size);
		return true;
	}
	
	bool rslt = true;	
	u64 docid = 0;
	if (query_rslt)
	{
		for(i64 i = s;i <= e;i++)
		{
			docid = mDocids[i];
			// filter by bitmap
			if (query_bitmap && (!query_bitmap->checkDoc(docid))) continue;
			query_rslt->appendDocid(docid);
			//rslt = query_rslt->appendBlock(&(mDocids[s]),size);
		}
	}

	return rslt;
}


bool
AosIIL::staticIsIILIDUsed(
		const u64 &iilid,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	// This function checks whether 'iilid' was used.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, true);

	// Ketty 2013/01/14
	//AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	//bool rslt = docfilemgr->readDoc(rdata, doc, false);
	//aos_assert_r(rslt, false);

	//AosDfmDocIILPtr iil_doc = (AosDfmDocIIL*)doc.getPtr();
	//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid, false);
	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, false);
	if (!doc)	return false;

	AosDfmDocIILPtr iil_doc = (AosDfmDocIIL*)doc.getPtr(); 
	AosIILType iiltype = iil_doc->getIILType();
	if (iiltype <= eAosIILType_Invalid || iiltype >= eAosIILType_Total)
	{
		return false;
	}
	if (iilid != iil_doc->getIILID())	return false;

	return true;
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
	OmnNotImplementedYet;
	return false;

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
	if (headLen == (eFullIILHeaderContentSize - eNormalIILHeaderContentSize))
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
		num_docs = headbuff->getInt32(0);
		flag = headbuff->getChar(0);
		hitcount = headbuff->getU32(0);
		ver = headbuff->getU32(0);
		disksize = headbuff->getU32(0);
		offset = headbuff->getU64(0);
		seqno = headbuff->getU32(0);
		compressed_size = headbuff->getU32(0);
		if (iiltype == eAosIILType_CompStr || iiltype == eAosIILType_CompU64)
		{
			isCompLeafIIL = true;
			comp_disksize = headbuff->getU32(0);
			comp_offset = headbuff->getU64(0);
			comp_seqno = headbuff->getU32(0);
			comp_compressed_size = headbuff->getU32(0);
		}
	}

	// Ketty 2012/10/30
	/*
	// Save the body buff
	int vid = 0;
	u64 localid = 0;
	bool rslt = AosIILMgr::getSelf()->parseIILID(iilid, localid, vid, rdata);
	aos_assert_r(rslt, false);

	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	aos_assert_r(docFileMgr, false);

	bool is_compress = false;
	if (isCompRootIIL || isCompLeafIIL)
	{
		is_compress = isCompress(comp_disksize);
		rslt = docFileMgr->saveDoc(comp_seqno, comp_offset, comp_disksize, bodybuff->data(), is_compress, comp_compressed_size, rdata);
	}
	else
	{
		is_compress = isCompress(disksize);
		rslt = docFileMgr->saveDoc(seqno, offset, disksize, bodybuff->data(), is_compress, compressed_size, rdata);
	}
	aos_assert_r(rslt, false);
	// rslt = savingSanityCheck(docFileMgr, seqno, testOffset, offset, bodybuff->data(), 
	// 		disksize, compressed_size, rdata);

	// Reconstruct the header buff
	u32 header_seqno = localid / sgMaxHeaderPerFile;
	u64 header_offset = (localid % sgMaxHeaderPerFile) * eIILHeaderSize;

	char bb[eIILHeaderMemsize+10];
	AosBuff buff(bb, eIILHeaderMemsize, 0, 0 AosMemoryCheckerArgs);
	if (isCompRootIIL)
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
		buff.setI64(num_docs);
		buff.setChar(flag);
		buff.setU32(hitcount);
		buff.setU32(ver);
		buff.setU32(disksize);
		buff.setU64(offset);
		buff.setU32(seqno);
		buff.setU32(compressed_size);
		
		if (isCompLeafIIL)
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
	*/
	// Ketty write at 2012/10/30
	/*
	int vid = 0;
	u64 localid = 0;
	bool rslt = AosIILMgr::getSelf()->parseIILID(iilid, localid, vid, rdata);
	aos_assert_r(rslt, false);

	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	aos_assert_r(docFileMgr, false);
	
	AosHeaderEntryPtr iil_header = OmnNew AosIILHeaderEntry(headbuff, isCompRootIIL || isCompLeafIIL, 
			isCompLeafIIL);
	vector<u64> trans;	// the trans is empty.
	rslt = docFileMgr->saveDoc(rdata, trans, localid, bodybuff, iil_header);
	aos_assert_r(rslt, false);
	return true;
	*/
	// Ketty end.
}
#endif


// Chen Ding, 09/30/2011
void 
AosIIL::setLastAddTimestamp(const bool add_to_head)
{
	mLastAddSec = OmnGetTimestamp();
	mLastAddToHead = add_to_head;
}

/*
void 
AosIIL::addTrans(const AosIILTransPtr &trans)
{
	// All transactions processed by an IIL are stored (temporarily) in mTrans.
	// When this queue becomes too long, it will save the contents into disks.

	//u64 gid = trans->getTransId();
	AosTransId gid = trans->getTransId();
	// u32 lid = trans->getLocalTransId();
	AOSLOCK(mLock);
	mTrans.push_back(gid);
	AOSUNLOCK(mLock);
}
*/

void 
AosIIL::addTrans(const AosTransId &id)
{
	AOSLOCK(mLock);
	mTrans.push_back(id);
	AOSUNLOCK(mLock);
}


void
AosIIL::finishTrans()
{
	AOSLOCK(mLock);
	//vector<u64> trans = mTrans;
	vector<AosTransId> trans = mTrans;
	mTrans.clear();
	AOSUNLOCK(mLock);

	if (trans.size() > 0)
	{
		AosFinishTrans(trans);
	}
}


// Chen Ding, 2015/05/25
/*
bool
AosIIL::staticCheckIILIDs(
		const u64 &iilid, 
		const u32 siteid,
		i64 &num,
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
	static i64 sgMaxIdsPerBatch = 1000;

	if (num <= 0)
	{
		OmnAlarm << "num is negative: " << num << enderr;
		num = sgMaxIdsPerBatch;
	}
	else if (num > sgMaxIdsPerBatch)
	{
		OmnAlarm << "num is too big: " << num << enderr;
	}

	const i64 buffsize = eIILHeaderSize * num;
	iilid_states.resize(num);
	char *bb = OmnNew char[buffsize+10];
	try
	{
		AosBuff buff(bb, buffsize, 0 AosMemoryCheckerArgs);
		
		// Ketty 2013/01/15
		OmnNotImplementedYet;
		//if (!staticReadHeaders(iilid, siteid, num, buff, rdata)) 
		{
			delete [] bb;
			return false;
		}

		for (i64 i=0; i<num; i++)
		{
			i64 crt_idx = i * eIILHeaderSize;
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
		//delete [] bb;		Ketty 2012/11/15
	}

	catch (...)
	{
		//delete [] bb;		Ketty 2012/11/15
		OmnAlarm << "Failed allocating memory: " << num << enderr;
		return false;
	}

	return true;
}
*/


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
	if (!rslt)
	{
		sgLock.unlock();
		return false;
	}

	// Chen Ding, 11/08/2011
	// AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, true, rdata);
	// bool isPersis = isPersisIIL(iilid);
	AosDocFileMgrObjPtr docFileMgr = getDocFileMgr(vid, siteid, rdata);
	if (!docFileMgr) 
	{
		sgLock.unlock();
		return false;
	}
	
	u32 seqno = localid / sgMaxHeaderPerFile;
	u32 offset = (localid % sgMaxHeaderPerFile) * eIILHeaderSize;
	int len = num * eIILHeaderSize;
	char *data = buff.data();
	
	int max = len;
	
	// Ketty 2012/10/30
	OmnNotImplementedYet;
	//if (!docFileMgr->readHeaders(seqno, offset, data, max, len, rdata))
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
		const i64 &docsize, 
		const i64 &compressed_size,
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
AosIIL::deleteFromLocalFilePriv(const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its 
	// subiil even if it is a root.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		//AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		//bool rslt = docfilemgr->readDoc(rdata, doc, false);
		//aos_assert_r(rslt, false);

		//mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
		//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid, false);
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, false);
		if (!doc) return false;
		mDfmDoc = (AosDfmDocIIL*)doc.getPtr();
	}

	aos_assert_r(mDfmDoc->getDocid() == local_iilid, false);

	//bool rslt = docFileMgr->deleteDoc(rdata, mTrans, mDfmDoc);
	bool rslt = AosIILSave::getSelf()->deleteDoc(
		rdata, docfilemgr, mTrans, mDfmDoc, mSnapShotId);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIIL::deleteFromLocalFileByID(
		const u64 iilid,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its 
	// subiil even if it is a root.
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid, false);
	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, false);
	aos_assert_r(doc, false);

	//bool rslt = docfilemgr->deleteDoc(rdata, mTrans, doc);
	bool rslt = AosIILSave::getSelf()->deleteDoc(
		rdata, docfilemgr, mTrans, doc, snap_id);
	aos_assert_r(rslt, false);

	return true;
}


// Ketty, 10/29/2012
bool
AosIIL::saveToLocalFilePriv(const AosRundataPtr &rdata)
{
	// This function saves the IIL to file. It does not save its
	// subiil even if it is a root.
	// 3. Save the body
	bool rslt = false;
	rslt = saveBitmapTree(rdata);
	aos_assert_r(rslt, false);

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	if (!mDfmDoc)
	{
		//AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
		//bool rslt = docfilemgr->readDoc(rdata, doc, false);
		//aos_assert_r(rslt, false);
		//mDfmDoc = (AosDfmDocIIL *)doc.getPtr();
		
		//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid, false);
		AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
			rdata, docfilemgr, mSnapShotId, local_iilid, false);
		if (!doc)
		{
			// means maybe it's a new iil.
			//mDfmDoc = newDfmDoc(local_iilid);
			mDfmDoc = OmnNew AosDfmDocIIL(local_iilid, mIILType, 
				mIILID, mWordId, mVersion, mIsPersis,
				mHitcount, mNumDocs, mFlag);
		}
		else
		{
			mDfmDoc = (AosDfmDocIIL *)doc.getPtr();
			//bool rslt = resetDfmDoc(mDfmDoc);
			//aos_assert_r(rslt, false);
			mDfmDoc->setHitCount(mHitcount);
			mDfmDoc->setNumDocs(mNumDocs);
			mDfmDoc->setFlag(mFlag);
		}
	}
	else
	{
		mDfmDoc->setHitCount(mHitcount);
		mDfmDoc->setNumDocs(mNumDocs);
		mDfmDoc->setFlag(mFlag);
	}

	AosBuffPtr bodyBuff = getBodyBuffProtected();
	aos_assert_r(bodyBuff, false);
	mDfmDoc->setBodyBuff(bodyBuff);
	mDfmDoc->setNeedCompress( isCompress(bodyBuff->dataLen()) );

	//bool rslt = docfilemgr->saveDoc(rdata, mTrans, mDfmDoc);
	rslt = AosIILSave::getSelf()->saveDoc(
		rdata, docfilemgr, mTrans, mDfmDoc, mSnapShotId);
	aos_assert_r(rslt, false);

	/*OmnScreen << "saveIIL: "
			<< " mIILID:" << mIILID
			<< "; mIILType:" << mIILType
			<< "; mIILIdx:" << mIILIdx
			<< "; thisPtr:" << this
			<< "; loc_id:" << mDfmDoc->getDocid()
			<< "; mNumDocs:" << mNumDocs
			<< "; snap_id:" << mSnapShotId 
			<< "; virtual_id:" <<  docfilemgr->getVirtualId() 
			<< "; bodySeqno:" << mDfmDoc->getBodySeqno()
			<< "; bodyOffset:" << mDfmDoc->getBodyOffset()
			<< "; bodyLen:" << mDfmDoc->getBodyBuff()->dataLen()
			<< endl;
			*/
	
	// Ketty 2013/06/08 
	mDfmDoc->cleanBodyBuff();
	mIsDirty = false;
	mIsNew = false;

	// Ketty 2012/11/30
	clearTrans();
	return true;
}


/*
// Ketty 2013/01/15
AosDfmDocPtr 
AosIIL::newDfmDoc(const u32 local_iilid)
{
	dfm_doc = OmnNew AosDfmDocIIL(local_iilid, mIILType, 
			mIILID, mWordId, mVersion, mIsPersis,
			mHitcount, mNumDocs, mFlag);
	//doc->setIILID(mIILID);
	//doc->setWordId(mWordId);
	//doc->setIILType(mIILType);
	//doc->setVersion(mVersion);
	//doc->setIsPersis(mIsPersis);
	return dfm_doc;
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
*/

// Ketty 2013/01/15
bool
AosIIL::readFromDfmDoc(
		const AosDfmDocIILPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc->getIILType() == mIILType, false);
	if (doc->getIILID() != mIILID)
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
	bool rslt = setContentsProtected(bodybuff, rdata);
	if (!rslt)
	{
		OmnAlarm << "Can not read body buff" << enderr;
		failInLoadProtection();
		return false;
	}
	return true;
}


// Ketty 2012/10/30
/*
bool
AosIIL::setHeaderToBuff(const AosBuffPtr &buff)
{
	// Chen Ding, 2011/01/28
	// Format is:
	// 	type			char	
	// 	IILID			u64
	// 	WordID			u64
	// 	Num of Docs		u32
	// 	flag			char
	// 	hit count		u32
	// 	version			u32
	// 	disk size		u32
	// 	offset			u64
	// 	seqno			u32
	// 	comp size		u32
	// 	comp offset		u64
	// 	comp seqno		u32
	// 			 (Total: 62)
	//
	// Chen Ding, 2013/01/12
	// 'comp size, offset, and seqno are not used anymore.
	buff->reset();
	buff->setU8((u8)mIILType);
	buff->setU64(mIILID);
	buff->setU64(mWordId);
	buff->setI64(mNumDocs);
	buff->setChar(mFlag);
	buff->setU32(mHitcount);
	
	u32 ver = mVersion;
	if (mIsPersis)
	{
		// ver = ver || 0x80000000;
		// Chen Ding, 02/18/2012
		// ver = ver | eCompIILBitFlag;
		ver = ver | ePersisBitOnVersionFlag;
	}
	else
	{
		// ver = ver && 0x7fffffff;
		// Chen Ding, 02/18/2012
		// ver = ver & ~eCompIILBitFlag;
	}
	buff->setU32(ver);
	return true;
}

bool
AosIIL::setHeaderFromBuff(const AosBuffPtr &buff)
{
	// Format is:
	// 	type			char	
	// 	IILID			u64
	// 	WordID			u64
	// 	Num of Docs		u32
	// 	flag			char
	// 	hit count		u32
	// 	version			u32
	// 	disk size		u32
	// 	offset			u64
	// 	seqno			u32
	// 	comp size		u32
	// 	comp offset		u64
	// 	comp seqno		u32
	// 			 (Total: 62)
	
	buff->reset();
	aos_assert_r((u8)mIILType == (u8)buff->getU8(0), false);

	u64 iid = buff->getU64(AOS_INVIILID);
	if (iid != mIILID || iid == 0)
	{
		OmnAlarm << "IILID mismatch: " << iid << ":" << mIILID << enderr;
		failInLoadProtection();
		return false;
	}

    mWordId = buff->getU64(AOS_INVWID);
    mNumDocs = buff->getI64(0);
    mFlag = buff->getChar(0);
    mHitcount = buff->getU32(0);
    u32 ver = buff->getU32(0);
	mVersion = ver & ~ePersisBitOnVersionFlag; 
	mIsPersis = ver & ePersisBitOnVersionFlag;

	mRootIIL = 0;
	mNumSubiils = 0;
	mIILIdx = -1;

	return true;
}
*/

bool
AosIIL::isCompress(const i64 &size)
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
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	iiltype = eAosIILType_Invalid;

	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);

	// Ketty 2013/01/14
	//AosDfmDocPtr doc = smDfmDoc->clone(local_iilid);
	//bool rslt = docfilemgr->readDoc(rdata, doc, false);
	//aos_assert_r(rslt, false);
	//AosDfmDocIILPtr iil_doc = (AosDfmDocIIL *)doc.getPtr();
	
	//AosDfmDocPtr doc = docfilemgr->readDoc(rdata, local_iilid, false);
	AosDfmDocPtr doc = AosIILSave::getSelf()->readDoc(
		rdata, docfilemgr, snap_id, local_iilid, false);
	if (!doc) return true;
	
	AosDfmDocIILPtr iil_doc = (AosDfmDocIIL *)doc.getPtr();
	iiltype = iil_doc->getIILType();
	return true;
}

	
bool
AosIIL::staticIsIILCreated(
		const u64 &iilid, 
		const u32 siteid,
		const u64 &snap_id,
		const AosIILType type,
		const AosRundataPtr &rdata)
{
	// This func is almost the same as staticIsIILIDUsed
	// Ketty 2013/01/15
	return staticIsIILIDUsed(iilid, snap_id, rdata);

	/*
	u64 local_iilid;
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(iilid, local_iilid, rdata);
	aos_assert_r(docfilemgr, false);
	
	// Ketty 2013/01/14
	AosDfmDocIILPtr doc = (AosDfmDocIIL *)docfilemgr->readDoc(
			rdata, local_iilid, AosDfmDoc::eIIL, false).getPtr();
	if (!doc || !doc->doesDocExist()) return false;

	AosIILType iiltype = doc->getIILType();
	aos_assert_r(type == iiltype, false);
	if (iiltype <= eAosIILType_Invalid || iiltype >= eAosIILType_Total)
	{
		return false;
	}

	if (doc->getIILID() != id)	return false;
	return true;
	*/
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


int
AosIIL::getNumIILsCreated()
{
	sgLock->lock();
	int nn = sgNumIILCreated;
	sgLock->unlock();
	return nn;
}
bool		
AosIIL::rebuildBitmapSafe(const AosRundataPtr &rdata)
{
	// do nothing for other type than AosIILBigStr
	return true;
}


bool
AosIIL::addId(const AosRundataPtr &rdata)
{
	//Linda, 2014/03/03
	if (mSnapShotId != 0) 
	{
		//small IIL docFileMgr Snap Shot reliability
		aos_assert_r(!AosIILSave::getSelf()->isSmallIILTrans(mSnapShotId), false);
		return true;
	}
	//small IIL trans reliability
	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	u64 snap_id = 0;
	bool rslt = AosIILSave::getSelf()->add(rdata, mIILID, snap_id);
	aos_assert_r(rslt, false);

	aos_assert_r(snap_id != 0, false);
	setSnapShotIdPriv(snap_id);
	//OmnScreen << "addId:" <<  mIILID << ";" << snap_id<< endl;
	return true;
}

bool
AosIIL::commitId(const AosRundataPtr &rdata)
{
	//Linda, 2014/03/03
	aos_assert_r(mSnapShotId, false);
	if (!AosIILSave::getSelf()->isSmallIILTrans(mSnapShotId)) return true;

	//small IIL trans reliability
	aos_assert_r((isRootIIL() || isSingleIIL()), false);

	u64 snap_id = mSnapShotId;
	aos_assert_r(snap_id, false);

	u64 local_iilid = 0;
	AosDocFileMgrObjPtr doc_mgr = getDocFileMgr(mIILID, local_iilid, rdata);
	aos_assert_r(doc_mgr, 0);

	bool rslt = AosIILSave::getSelf()->commit(rdata, mIILID, doc_mgr, snap_id);
	aos_assert_r(rslt, false);

	resetSnapShotIdPriv();
	//OmnScreen << "commitId:" << mIILID << ";" << snap_id << endl;
	return true;
}
