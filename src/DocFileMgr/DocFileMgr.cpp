///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
//	To prevent from concurrent operating on the same file, wherenever
//	to do something on a file, lock the file. After that, release the file.
//
// Each doc file has a header, which is in the following format:
// 	eOffsetBlocksize(4bytes):	the blocksize
// 	eOffsetFlags(4bytes):		the flags
// 	eOffsetBlockStart(4bytes):	the flags
//
// Modification History:
// 12/15/2009	Created by Chen Ding	Modified by Ketty.
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DocFileMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocHeader.h"
#include "DfmSnapshot/DfmSnapshotMgr.h"
#include "DocFileMgr/DfmLog.h"
#include "DocFileMgr/DfmBody.h"
#include "DocFileMgr/DfmHeader.h"
#include "DocFileMgr/DfmHeaderFile.h"
#include "DocFileMgr/DfmCaller.h"
#include "DocFileMgr/DfmUtil.h"
#include "DocFileMgr/FileGroupMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"


AosDocFileMgr::LockGen AosDocFileMgr::smLockGen;

AosDocFileMgr::AosDocFileMgr(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config AosMemoryCheckDecl)
:
mLock(OmnNew OmnMutex()),
mFgm(fgm),
mVirtualId(virtual_id),
mId(dfm_id),
mSiteid(rdata->getSiteid()),	// Ketty 2013/01/29
mConfig(config),
mShowLog(config.mShowLog)
{
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	{
		OmnShouldNeverComeHere;
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosDocFileMgr);
}

AosDocFileMgr::AosDocFileMgr(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm AosMemoryCheckDecl)
:
mLock(OmnNew OmnMutex()),
mFgm(fgm),
mSiteid(rdata->getSiteid()),	// Ketty 2013/01/29
mShowLog(false)
{
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	{
		OmnShouldNeverComeHere;
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosDocFileMgr);
}


AosDocFileMgr::~AosDocFileMgr()
{
	AosMemoryCheckerObjDeleted(AosClassName::eAosDocFileMgr);
	// when It's clean by FileGrougMgr. It's not need mSaveCacher's stop. 
}


AosDocFileMgrObjPtr
AosDocFileMgr::createDfmStatic(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm,
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmConfig &config AosMemoryCheckDecl)
{
	AosDocFileMgrPtr dfm = OmnNew AosDocFileMgr(rdata,
			fgm, virtual_id, dfm_id, config AosMemoryCheckerArgs);
	bool rslt = dfm->init();
	aos_assert_r(rslt, 0);

	// Ketty 2013/05/15
	rslt = dfm->saveToFile(rdata);
	aos_assert_r(rslt, 0);
	return dfm;
}


AosDocFileMgrObjPtr
AosDocFileMgr::serializeDfmStatic(
		const AosRundataPtr &rdata,
		const AosFileGroupMgrPtr &fgm,
		const AosBuffPtr &dfm_buff AosMemoryCheckDecl)
{
	aos_assert_r(dfm_buff, 0);
	
	AosDocFileMgrPtr dfm = OmnNew AosDocFileMgr(rdata, fgm AosMemoryCheckerArgs);  
	dfm->serializeFrom(dfm_buff);

	bool rslt = dfm->init();
	aos_assert_r(rslt, 0);
	return dfm;
}


bool
AosDocFileMgr::serializeFrom(const AosBuffPtr &dfm_buff)
{
	aos_assert_r(dfm_buff, false);
	mId = dfm_buff->getU32(0);
	mVirtualId = dfm_buff->getU32(0);
	u32 f_siteid = dfm_buff->getU32(0);
	if(f_siteid != mSiteid)
	{
		mSiteid = f_siteid;
	}
	AosDfmConfig::serializeFrom(mConfig, dfm_buff); 
	mShowLog = mConfig.mShowLog;

	u32 file_num = dfm_buff->getU32(0);
	for(u32 i=0; i<file_num; i++)
	{
		AosDfmFilePtr file_info = AosDfmFile::serializeFromStatic(dfm_buff);    
		// Temp Ketty 2014/07/21.
		//aos_assert_r(file_info, false);
		if (!file_info) continue;
				    
		AosDfmFileType::E type = file_info->getType();
		if(type == AosDfmFileType::eHeaderFile)
		{
			AosDfmHeaderFilePtr h_file_info = (AosDfmHeaderFile *)file_info.getPtr();
			mHeaderFiles.push_back(h_file_info);
		}
		else
		{
			mBodyFiles.push_back(file_info);
		}
	}

	return true;
}


bool
AosDocFileMgr::serializeTo(const AosBuffPtr &dfm_buff)
{
	aos_assert_r(dfm_buff, false);
	dfm_buff->setU32(mId);
	dfm_buff->setU32(mVirtualId);
	dfm_buff->setU32(mSiteid);
	mConfig.serializeTo(dfm_buff);
	
	dfm_buff->setU32(mHeaderFiles.size() + mBodyFiles.size());
	list<AosDfmHeaderFilePtr>::iterator itr = mHeaderFiles.begin();
	for(; itr != mHeaderFiles.end(); itr++)
	{
		AosDfmHeaderFilePtr file_info = *itr;
		file_info->serializeTo(dfm_buff);		
	}

	for(u32 i=0; i<mBodyFiles.size(); i++)
	{
		mBodyFiles[i]->serializeTo(dfm_buff);	
	}
	return true;
}


bool
AosDocFileMgr::init()
{
	OmnString prefix = mConfig.mPrefix;
	OmnString h_prefix = prefix;
	h_prefix << "_idx";

	//mHeader = OmnNew AosDfmHeader(mVirtualId, mId, 0, mConfig.getHeaderSize(),
	//		h_prefix, mHeaderFiles, mConfig.mTempDoc, mConfig.mShowLog);
	mHeader = OmnNew AosDfmHeader(mVirtualId, mId, h_prefix,
			mHeaderFiles, mConfig.mDocType, mConfig.mShowLog);
	mBody = OmnNew AosDfmBody(mVirtualId, mId, prefix, mConfig.mCompressAlg, 
			mBodyFiles, mConfig.mShowLog);

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(mSiteid);
	//Linda, 2013/07/08
	AosDocFileMgrPtr thisPtr(this, false);
	mDfmLog = OmnNew AosDfmLog(rdata, mConfig.mDocType, thisPtr, 
		mVirtualId, mId, mConfig.mShowLog);
	
	// Ketty 2014/03/11
	if(mConfig.mNeedSnapshotMgr) 
	{
		mSnapshotMgr = AosDfmSnapshotMgr::createSnapshotMgrStatic(rdata, thisPtr);
		aos_assert_r(mSnapshotMgr, false);
	}
	return true;
}


bool
AosDocFileMgr::startStop()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(mSiteid);
	
	mDfmLog->startStop(rdata);
	return true;
}


bool
AosDocFileMgr::stop()
{
	//AosDocFileMgrPtr thisPtr(this, false);
	//return mSaveCacher->stop(thisPtr);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(mSiteid);
	
	mDfmLog->stop();
	mBody->stop(rdata);
	return true;
}


bool
AosDocFileMgr::saveDoc(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	vector<AosTransId> trans_ids;
	if(trans_id != AosTransId::Invalid)	trans_ids.push_back(trans_id);

	return saveDoc(rdata, trans_ids, doc, flushflag);
}


bool
AosDocFileMgr::deleteDoc(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const AosDfmDocPtr &doc,
		const bool flushflag) 
{
	vector<AosTransId> trans_ids;
	if(trans_id != AosTransId::Invalid)	trans_ids.push_back(trans_id);

	return deleteDoc(rdata, trans_ids, doc, flushflag);
}


// Chen Ding, 2013/02/19
bool 
AosDocFileMgr::saveHeader(
		const AosRundataPtr &rdata, 
		const AosDfmDocPtr &doc)
{
	OmnNotImplementedYet;
	return false;
}


/*
bool
AosDocFileMgr::removeHeaderFiles(
		const AosRundataPtr &rdata,
		const u32 snap_id)
{
	// snap_id 0 is the normal header.
	aos_assert_r(snap_id != 0, false);	

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	mLock->lock();
	list<AosDfmHeaderFilePtr>::iterator itr = mHeaderFiles.begin();
	while(itr != mHeaderFiles.end())
	{
		AosDfmHeaderFilePtr file_info = *itr;
		if(file_info->getSnapId() != snap_id)
		{
			itr++;
			continue;
		}
		
		u64 file_id = file_info->getFileId();
		u32 seqno = file_info->getFileSeqno();
		u32 snap_id = file_info->getSnapId();
	
		// delete this file.
		list<AosDfmHeaderFilePtr>::iterator tmp_itr = itr;
		itr++;
		mHeaderFiles.erase(tmp_itr);
	
		if(mShowLog)
		{
			OmnScreen << "DocFileMgr; DfmHeader; removeHeader file:"
				<< "; virtual_id:" << mVirtualId
				<< "; dfm_id:" << mId
				<< "; file_id:" << file_id
				<< "; seqno:" << seqno 
				<< "; snap_id:" << snap_id 
				<< "; fname: " << vfsMgr->getFileName(file_id, rdata)
				<< endl;
		}
		vfsMgr->removeFile(file_id, rdata);
	}
	
	bool rslt = saveToFileLocked(rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}
*/


bool
AosDocFileMgr::addHeaderFile(
		const AosRundataPtr &rdata,
		const AosDfmHeaderFilePtr &file)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	
	// snap_id 0 is the normal header.
	u32 snap_id = file->getSnapId();

	mLock->lock();
	mHeaderFiles.push_back(file);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; addHeader file:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; file_id:" << file->getFileId() 
			<< "; seqno:" << file->getFileSeqno() 
			<< "; snap_id:" << snap_id 
			<< "; fname: " << vfsMgr->getFileName(file->getFileId(), rdata.getPtr())
			<< endl;
	}
		
	//OmnScreen << "DocFileMgr; addHeader file:"
	//	<< "; virtual_id:" << mVirtualId
	//	<< "; dfm_id:" << mId
	//	<< "; file_id:" << file->getFileId() 
	//	<< "; seqno:" << file->getFileSeqno() 
	//	<< "; snap_id:" << snap_id 
	//	<< "; fname: " << vfsMgr->getFileName(file->getFileId(), rdata)
	//	<< endl;
	
	bool rslt = saveToFileLocked(rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}


bool 
AosDocFileMgr::addBodyFile(
		const AosRundataPtr &rdata,
		const AosDfmFilePtr &file)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	
	mLock->lock();
	mBodyFiles.push_back(file);
	
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; addBody file:"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; file_id:" << file->getFileId()
			<< "; seqno:" << file->getFileSeqno()
			<< "; fname: " << vfsMgr->getFileName(file->getFileId(), rdata.getPtr())
			<< endl;
	}
		
	//OmnScreen << "DocFileMgr; addBody file:"
	//	<< "; virtual_id:" << mVirtualId
	//	<< "; dfm_id:" << mId
	//	<< "; file_id:" << file->getFileId()
	//	<< "; seqno:" << file->getFileSeqno() 
	//	<< "; fname: " << vfsMgr->getFileName(file->getFileId(), rdata)
	//	<< endl;
	
	bool rslt = saveToFileLocked(rdata);
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosDocFileMgr::saveToFile(const AosRundataPtr &rdata)
{
	mLock->lock();
	bool rslt = saveToFileLocked(rdata);
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}

bool 
AosDocFileMgr::saveToFileLocked(const AosRundataPtr &rdata)
{
	AosBuffPtr dfm_buff = OmnNew AosBuff(AosFileGroupMgr::eEntrySize,
			0 AosMemoryCheckerArgs);
	bool rslt = serializeTo(dfm_buff);
	aos_assert_r(rslt, false);

	mFgm->saveDfmBuffToFile(rdata, mId, dfm_buff);
	return true;
}
	

bool
AosDocFileMgr::removeAllFiles(const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	u64 file_id;
	bool rslt;
	
	list<AosDfmHeaderFilePtr>::iterator itr = mHeaderFiles.begin();
	for(; itr != mHeaderFiles.end(); itr++)
	{
		file_id = (*itr)->getFileId();
		aos_assert_rl(file_id, mLock, false);
		
		rslt = vfsMgr->removeFile(file_id, rdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
	}

	for(u32 i=0; i<mBodyFiles.size(); i++)
	{
		file_id = (mBodyFiles[i])->getFileId();
		aos_assert_rl(file_id, mLock, false);
		
		rslt = vfsMgr->removeFile(file_id, rdata.getPtr());
		aos_assert_rl(rslt, mLock, false);
	}

	AosBuffPtr dfm_buff;
	rslt = mFgm->removeDfmBuffFromFile(rdata, mId, dfm_buff);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	return true;
}


bool
AosDocFileMgr::removeFilesByDfmBuff(
		const AosRundataPtr &rdata,
		const u32 dfm_id,
		const AosBuffPtr &dfm_buff)
{
	aos_assert_r(dfm_buff, false);
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	u64 file_id;
	bool rslt;
	
	u32 dfm_id_2 = dfm_buff->getU32(0);
	aos_assert_r(dfm_id_2 == dfm_id, false);
	dfm_buff->getU32(0);	// mVirtualId
	dfm_buff->getU32(0);	// site_id.

	AosDfmConfig conf;
	AosDfmConfig::serializeFrom(conf, dfm_buff); 
	
	u32 file_num = dfm_buff->getU32(0); 
	for(u32 i=0; i<file_num; i++)
	{
		AosDfmFilePtr file_info = AosDfmFile::serializeFromStatic(dfm_buff);	
		aos_assert_r(file_info, false);
		
		file_id = (file_info)->getFileId();
		aos_assert_r(file_id, false);
		
		rslt = vfsMgr->removeFile(file_id, rdata.getPtr());
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDocFileMgr::saveDoc(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	AosBuffPtr buff = doc->getBodyBuff();
	aos_assert_r(buff->dataLen() < 512000000, false);
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; saveDoc;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid()
			<< "; "
			<< endl;
	}

    OmnMutexPtr lock = mLockPool.get(doc->getDocid(), smLockGen);
	lock->lock();

	if(mConfig.mNeedLog)
	{
		doc->setOpr(AosDfmDoc::eSave);
		mDfmLog->addEntry(rdata, doc, trans_ids);
	}
	else
	{
		bool rslt = saveDocToDfm(rdata, doc);
		aos_assert_rl(rslt, lock, false);
	}
	
	lock->unlock();

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; saveDoc finish."
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << doc->getBodySeqno()
			<< "; bodyOffset:" << doc->getBodyOffset()
			<< endl;
	}
	
	return true;
}


bool
AosDocFileMgr::deleteDoc(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const bool flushflag) 
{
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; deleteDoc;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << doc->getDocid()
			<< endl;
	}
	
	if(mConfig.mNeedLog)
	{
		doc->setOpr(AosDfmDoc::eDelete);
		mDfmLog->addEntry(rdata, doc, trans_ids);
		return true;
	}

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; deleteDoc; removeBody"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << doc->getBodySeqno()
			<< "; bodyOffset:" << doc->getBodyOffset()
			<< endl;
	}
	
	bool rslt = deleteDocFromDfm(rdata, doc);
	aos_assert_r(rslt, false);	
	return true;
}


AosDfmDocPtr
AosDocFileMgr::readDoc(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body)
{
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; readDoc;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< "; docid:" << docid
			<< "; read_body:" << read_body
			<< endl;
	}

	AosDfmDocPtr doc;
	if(mConfig.mNeedLog)
	{
		doc = mDfmLog->readDoc(rdata, docid);
		if (doc) 
		{
			//Linda, 2013/09/23
			if (doc->getOpr() == AosDfmDoc::eDelete) return 0;
			if(doc && mShowLog)
			{
				OmnScreen << "DocFileMgr; readDoc; DfmLog; finish."
					<< "; virtual_id:" << mVirtualId
					<< "; dfm_id:" << mId
					<< "; docid:" << doc->getDocid()
					<< "; bodySeqno:" << doc->getBodySeqno()
					<< "; bodyOffset:" << doc->getBodyOffset()
					<< "; dataLen:" << doc->getBodyBuff()->dataLen()
					<< endl;
			}
			return doc;
		}
	}

	doc = readDocFromDfm(rdata, docid, read_body);
	if(!doc)	return 0;
	return doc;
}


AosDfmHeaderPtr 
AosDocFileMgr::getDfmHeaderObj() const
{ 
	return mHeader;
}


AosDfmBodyPtr   
AosDocFileMgr::getDfmBodyObj() const
{
	return mBody;
}


bool
AosDocFileMgr::saveDocAsync(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const AosDfmCallerPtr &caller,
		const bool flushflag)
{
	doc->setOpr(AosDfmDoc::eSave);
	mDfmLog->addEntry(rdata, doc, trans_ids);
	caller->saveCallBack(doc->getDocid());

	return true;
}


bool
AosDocFileMgr::deleteDocAsync(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const AosDfmCallerPtr &caller,
		const bool flushflag)
{
	doc->setOpr(AosDfmDoc::eDelete);
	mDfmLog->addEntry(rdata, doc, trans_ids);
	caller->deleteCallBack(doc->getDocid());

	return true;
}


bool
AosDocFileMgr::readDocAsync(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body,
		const AosDfmCallerPtr &caller)
{
	AosDfmDocPtr doc = mDfmLog->readDoc(rdata, docid);
	if (doc)
	{
		caller->readCallBack(docid, doc);
		return true;
	}
	return true;
}


bool 	
AosDocFileMgr::recoverDfmLog(const AosRundataPtr &rdata, const u64 &file_id)
{
	aos_assert_r(mDfmLog, false);
	return mDfmLog->recoverDfmLog(rdata, file_id);
}

//Linda, 2014/03/04
//bool
//AosDocFileMgr::commitDfmLog(
//		const AosRundataPtr &rdata, 
//		const AosDfmLogFilePtr &dfmlogfile)
//{
//	aos_assert_r(mDfmLog, false);
//	return mDfmLog->commit(rdata, dfmlogfile);
//}


bool
AosDocFileMgr::saveDocs(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff)
{
	//This function is used by IIL .
	//Linda, 2014/02/27 
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; saveDocs;"
			<< "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; prefix:" << mConfig.mPrefix
			<< endl;
	}

	aos_assert_r(mConfig.mNeedLog, false);
	mDfmLog->addEntrys(rdata, index_map, buff);
	return true;
}


// Ketty 2014/02/28
bool
AosDocFileMgr::saveDocToDfm(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	// flush dfmDoc's mem data to mHeaderBuff. for IIL.
	doc->flushToHeaderBuff();

	// compress.
	bool rslt;
	AosBuffPtr body_buff = doc->getBodyBuff();
	u32 orig_len = body_buff->dataLen();
	u32 compress_len = 0;
	if(doc->needCompress())
	{	
		rslt = mBody->compressBody(body_buff, rdata AosMemoryCheckerArgs);
		aos_assert_r(rslt && body_buff, false);
		compress_len = body_buff->dataLen(); 
		doc->setBodyBuff(body_buff);    // reset the compressed body.
	}
	doc->setOrigLen(orig_len);
	doc->setCompressLen(compress_len);
	
	AosDfmDocHeaderPtr crt_header, new_header;
	rslt = mHeader->readHeader(rdata, doc->getDocid(), crt_header);
	aos_assert_r(rslt, false);

	doc->setBodySeqOff(crt_header->getBodySeqno(),
			crt_header->getBodyOffset());
	rslt = saveBody(rdata, doc);	
	aos_assert_r(rslt, false);
	
	new_header = doc->getHeader();
	// iil will always save the header.
	//if(new_header->isSameBodySeqOff(crt_header)) return true;
	
	AosDocFileMgrPtr thisptr(this, false);
	rslt = mHeader->saveHeader(rdata, thisptr, new_header);
	aos_assert_r(rslt, false);
	return true;
}


// Ketty 2014/02/28
bool
AosDocFileMgr::saveBody(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	bool rslt;
	AosDocFileMgrObjPtr thisptr(this, false);
	
	if(doc->isHeaderEmpty())
	{
		rslt = mBody->addBody(rdata, thisptr, doc);
		aos_assert_r(rslt, false);
		return true;
	}

	bool kickout;
	rslt = mBody->modifyBody(rdata, thisptr, kickout, doc);
	aos_assert_r(rslt, false);
	if(!kickout)	return true;

	rslt = mBody->removeBody(rdata, doc);
	aos_assert_r(rslt, false);
	
	doc->setBodySeqOff(0, 0);
	rslt = mBody->addBody(rdata, thisptr, doc);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocFileMgr::deleteDocFromDfm(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	AosDocFileMgrObjPtr thisptr(this, false);
	
	AosDfmDocHeaderPtr crt_header, new_header;
	bool rslt = mHeader->readHeader(rdata, doc->getDocid(), crt_header);
	aos_assert_r(rslt, false);
	
	if(crt_header->isEmpty())
	{
		OmnAlarm << "this doc not exist in file."
			    << "docid:" << doc->getDocid() << enderr;
		return false;	
	}

	doc->setBodySeqOff(crt_header->getBodySeqno(),
		crt_header->getBodyOffset());
	rslt = mBody->removeBody(rdata, doc);
	aos_assert_r(rslt, false);

	// this new_header is empty.
	new_header = doc->cloneDfmHeader(doc->getDocid());
	rslt = mHeader->saveHeader(rdata, thisptr, new_header);
	return true;
}


AosDfmDocPtr
AosDocFileMgr::readDocFromDfm(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body)
{
	bool rslt, exist = false;
	AosDfmDocHeaderPtr crt_header;
	
	if(mSnapshotMgr)
	{
		rslt = mSnapshotMgr->readFromCommitingSnapshot(
				rdata, docid, exist, crt_header);
		aos_assert_r(rslt, 0);
	}
	
	if(!exist)
	{
		rslt = mHeader->readHeader(rdata, docid, crt_header);
		aos_assert_r(rslt, 0);
	}

	aos_assert_r(crt_header, 0);
	if(crt_header->isEmpty())	return 0;	// this doc not exist

	AosDfmDocPtr doc = AosDfmDoc::cloneDoc(mConfig.mDocType, crt_header);
	if(read_body)
	{
		rslt = mBody->readBody(rdata, doc);
		aos_assert_r(rslt, 0);
	}
	
	if(mShowLog)
	{
		OmnString str = "DocFileMgr; readDoc. finish";
		str << "; virtual_id:" << mVirtualId
			<< "; dfm_id:" << mId
			<< "; docid:" << doc->getDocid()
			<< "; bodySeqno:" << doc->getBodySeqno()
			<< "; bodyOffset:" << doc->getBodyOffset();
		if(read_body) str << "; dataLen:" << doc->getBodyBuff()->dataLen();
		OmnScreen << str << endl;
	}
	return doc;
}


