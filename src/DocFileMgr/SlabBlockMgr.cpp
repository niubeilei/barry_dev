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
//
// Modification History:
// 02/07/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/SlabBlockMgr.h"

#include "DocFileMgr/DfmSlabFile.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/SlabBlockFile.h"
#include "ReliableFile/ReliableFile.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/SystemId.h"
#include "SEUtil/SeConfig.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"


AosSlabBlockMgr::AosSlabBlockMgr(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mVirtualId(v_id),
mDfmId(dfm_id),
mSlabPrefix(prefix),
mCrtMaxSeqno(-1),
mShowLog(show_log)
{
	for(u32 i=0; i<files.size(); i++)
	{
		if(files[i]->getType() != AosDfmFileType::eSlabFile) continue;
	
		AosDfmSlabFilePtr dfm_file = (AosDfmSlabFile *)files[i].getPtr();
		u32 seqno = dfm_file->getFileSeqno();
		mFiles.insert(make_pair(seqno, dfm_file));
		if((int)seqno > mCrtMaxSeqno) mCrtMaxSeqno = seqno;
	}
}


AosSlabBlockMgr::~AosSlabBlockMgr()
{
}


bool
AosSlabBlockMgr::saveBitmap(
		const u32 seqno, 
		const AosRundataPtr &rdata)
{
	AosSlabBlockFilePtr slabFile = openSlabFile(seqno, rdata);	
	aos_assert_r(slabFile, false);
	
	return slabFile->saveBitmap(rdata);		
}


bool
AosSlabBlockMgr::flushBitmap(const AosRundataPtr &rdata)
{
	// Ketty 2014/03/25
	mLock->lock();
	
	AosSlabBlockFilePtr slab_ff;
	FileMapItr itr = mFiles.begin();
	for(; itr != mFiles.end(); itr++)
	{
		AosDfmSlabFilePtr dfm_file = itr->second;
		slab_ff = dfm_file->getSlabFile(rdata);
		
		slab_ff->saveBitmap(rdata);
	}
	mLock->unlock();

	return true;	
}


bool
AosSlabBlockMgr::findBlock(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		u32 &blockId,
		const u32 docsize,
		const AosRundataPtr &rdata)
{
	// This function finds a new block for the size 'docsize'. 
	// It first converts 'docsize' to a doc size index. 
	bool find = false;
	u32 blocksize = AosDfmUtil::docsize2SlabBlockSize(docsize);
	AosSlabBlockFilePtr slabFile;
	for(int i=mCrtMaxSeqno; i>=0; i--)
	{
		seqno = (u32)i;
		slabFile = openSlabFile(seqno, rdata);
		//Ketty 2014/02/12
		//aos_assert_r(slabFile, false);
		if(!slabFile)                                              
		{                                                          
			// maybe this slabFile has error!                      
			OmnScreen << "maybe this slabFile has error: " << endl;
			continue;                                              
		}

		if(slabFile->getBlockSize() != blocksize) continue;
		
		slabFile->findEmptyBlock(find, blockId, rdata);
		if(find) return true;
	}
	
	// When come here. means need create a new SlabFile
	slabFile = createSlabFile(dfm, seqno, blocksize, rdata);
	aos_assert_r(slabFile, false);
	
	bool rslt = slabFile->findEmptyBlock(find, blockId, rdata);
	aos_assert_r(rslt && find, false);
	return true;	
}


bool
AosSlabBlockMgr::addDocs(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		AosDfmCompareFunType::set_fs &docs)
{
	AosDfmCompareFunType::setitr_fs itr = docs.begin();
	while(itr != docs.end())
	{
		u32 seqno = (*itr)->getBodySeqno();
		AosSlabBlockFilePtr slabFile = openSlabFile(seqno, rdata);	
		aos_assert_r(slabFile, false);
		
		bool rslt = addDocs(rdata, docs, itr, slabFile, seqno);
		aos_assert_r(rslt, false);

		slabFile->saveBitmap(rdata);		
	}
	return true;
}


bool
AosSlabBlockMgr::modifyDoc(
		const u32 &rseqno,
		const u64 &offset,
		const u32 orig_len,
		const u32 compress_len,
		const char *data,
		bool &need_kickout,
		const AosRundataPtr &rdata)
{
	//u32 seqno = rseqno & 0x7fffffff;
	AosSlabBlockFilePtr slabFile = openSlabFile(rseqno, rdata);	
	aos_assert_r(slabFile, false);

	bool rslt;
	u32 blockSize = slabFile->getBlockSize();
	u32 maxDocSize = slabFile->getMaxDocSize(blockSize);
	u32 data_len = compress_len ? compress_len : orig_len;
	
	if(data_len > maxDocSize)
	{
		need_kickout = true;
		//Ice Yu 2013/5/17
		//bool rslt = removeDoc(seqno, offset, rdata);
		//aos_assert_r(rslt, false);
		
		return true;
	}

	need_kickout = false;
	u32 blockId = (u32)offset;
	rslt = slabFile->saveDoc(blockId, orig_len, compress_len, data, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSlabBlockMgr::removeDoc(
		const u32 rseqno,
		const u64 offset,
		const AosRundataPtr &rdata)
{
	//u32 seqno = rseqno & 0x7fffffff;
	
	AosSlabBlockFilePtr slabFile = openSlabFile(rseqno, rdata);	
	aos_assert_r(slabFile, false);
	
	u32 blockId = (u32)offset;
	bool rslt = slabFile->removeDoc(blockId);
	aos_assert_r(rslt, false);

	//OmnScreen << "ktttt:"
	//	<< "; virtual_id:" << mVirtualId
	//	<< "; remove doc. rseqno:" << rseqno
	//	<< "; offset:" << offset
	//	<< endl;

	return true;
}


bool
AosSlabBlockMgr::readDoc(
		const u32 rseqno,
		const u64 offset,
		AosBuffPtr &data_buff,
		u32 &orig_len,
		u32 &compress_len,
		const AosRundataPtr &rdata)
{
	//u32 seqno = rseqno & 0x7fffffff;
	
	AosSlabBlockFilePtr slabFile = openSlabFile(rseqno, rdata);	
	aos_assert_r(slabFile, false);
	
	u32 blockId = (u32)offset;
	//bool rslt = slabFile->readDoc(blockId, docsize, data);
	bool rslt = slabFile->readDoc(blockId, data_buff, orig_len, compress_len, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosSlabBlockFilePtr
AosSlabBlockMgr::openSlabFile(
		const u32 seqno,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	
	AosSlabBlockFilePtr slab_ff;
	FileMapItr itr = mFiles.find(seqno);
	if(itr != mFiles.end())
	{
		AosDfmSlabFilePtr dfm_file = itr->second;
		slab_ff = dfm_file->getSlabFile(rdata);
	}
	mLock->unlock();

	return slab_ff;
}


AosSlabBlockFilePtr
AosSlabBlockMgr::createSlabFile(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		const u32 blocksize,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	// create a new file.
	if(mCrtMaxSeqno >= eMaxFiles - 1)
	{
		OmnAlarm << "too many files! " << mCrtMaxSeqno << enderr;
		mLock->unlock();
		return 0;	
	}

	seqno = (++mCrtMaxSeqno);
	AosDfmSlabFilePtr dfm_file = OmnNew AosDfmSlabFile(rdata, seqno,
			blocksize, mVirtualId, mSlabPrefix, eAosMaxFileSize);
	aos_assert_rl(dfm_file, mLock, 0);

	mFiles.insert(make_pair(seqno, dfm_file));
	mLock->unlock();
	
	dfm->addBodyFile(rdata, dfm_file);
	
	AosSlabBlockFilePtr slab_ff = dfm_file->getSlabFile(rdata);
	aos_assert_r(slab_ff, 0);
	return slab_ff;
}


bool
AosSlabBlockMgr::addDocs(
		const AosRundataPtr &rdata, 
		AosDfmCompareFunType::set_fs &docs,
		AosDfmCompareFunType::setitr_fs &itr,
		const AosSlabBlockFilePtr &slabFile,
		const u32 &seqno)
{
	bool rslt = false;
	u32 blocksize = slabFile->getBlockSize(); 
	while (itr != docs.end() && (*itr)->getBodySeqno() == seqno)
	{
		AosBuffPtr buff = OmnNew AosBuff(blocksize, 0 AosMemoryCheckerArgs);

		AosDfmDocPtr doc = *itr;
		u32 begin_blockid = doc->getBodyOffset();
		u32 crt_blockid = begin_blockid;

		for (; itr != docs.end(); ++itr)
		{
			doc = *itr;
			if (doc->getBodySeqno() != seqno) break;

			u32 blockid = doc->getBodyOffset();
			aos_assert_r(blockid >= begin_blockid && blockid >= crt_blockid, false);

			if (blockid > crt_blockid +1 || buff->dataLen() > eBatchBlockSize) break;

			u32 index = blockid - begin_blockid;
			AosBuffPtr body_buff = doc->getBodyBuff();
			aos_assert_r(body_buff && body_buff->dataLen() > 0, false);

			rslt = slabFile->saveDocToBuff(index, buff, doc->getOrigLen(), doc->getCompressLen(), body_buff->data());
			aos_assert_r(rslt, false);

			if(mShowLog)
			{
				OmnScreen << "DocFileMgr; DfmBody; save:"
					<< "; virtual_id:" << mVirtualId
					<< "; dfm_id:" << mDfmId
					<< "; docid:" << doc->getDocid() 
					<< "; bodySeqno:" << seqno
					<< "; bodyOffset:" << blockid
					<< "; orig_len:" << doc->getOrigLen() 
					<< "; compress_len:" << doc->getCompressLen() 
					<< endl;
			}
			crt_blockid = blockid;
		}

		aos_assert_r(buff->dataLen() > 0, false);
		rslt = slabFile->addDocs(rdata, begin_blockid, buff->data(), buff->dataLen());
		aos_assert_r(rslt, false);
//OmnScreen << "======================== num : " << buff->dataLen() / blocksize  << " blocksize: " << blocksize<< endl;
	}
	return true;
}


bool
AosSlabBlockMgr::addDoc(
		const AosDfmDocPtr doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);
	u32 seqno = doc->getBodySeqno();
	u64 block_id = doc->getBodyOffset();

	AosSlabBlockFilePtr slabFile = openSlabFile(doc->getBodySeqno(), rdata);  
	aos_assert_r(slabFile, false);

	char *data = (doc->getBodyBuff())->data();
	bool rslt = slabFile->saveDoc(block_id, doc->getOrigLen(), doc->getCompressLen(), data, rdata);
	aos_assert_r(rslt, false);

	// Ketty 2014/03/25
	slabFile->saveBitmap(rdata);

	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; slab. add finish:"
			<< "; seqno:" << seqno 
			<< "; offset:" << block_id 
			<< endl;
	}
	return true;
}




/* Linda, 2013/09/25
bool
AosSlabBlockMgr::addDocs(
		const AosRundataPtr &rdata, 
		AosDfmCompareFunType::set_fs &docs,
		AosDfmCompareFunType::setitr_fs &itr,
		const AosSlabBlockFilePtr &slabFile,
		const u32 &seqno)
{
	bool rslt = false;
	while (itr != docs.end() && (*itr)->getBodySeqno() == seqno)
	{
		AosDfmDocPtr doc = *itr;
		aos_assert_r(doc->getBodySeqno() == seqno, false);
		u32 blockId = doc->getBodyOffset();

		char *data = (doc->getBodyBuff())->data();
		rslt = slabFile->saveDoc(blockId, doc->getOrigLen(), doc->getCompressLen(), data, rdata);
		aos_assert_r(rslt, false);

		if(mShowLog)
		{
			OmnScreen << "DocFileMgr; DfmBody; save:"
				<< "; virtual_id:" << mVirtualId
				<< "; dfm_id:" << mDfmId
				<< "; docid:" << doc->getDocid() 
				<< "; bodySeqno:" << seqno
				<< "; bodyOffset:" << blockId
				<< "; orig_len:" << doc->getOrigLen() 
				<< "; compress_len:" << doc->getCompressLen() 
				<< endl;
		}
		++ itr;
	}
	return true;
}
*/


/*
 * Linda, 2013/08/16
bool
AosSlabBlockMgr::addDocs(
		const AosRundataPtr &rdata, 
		AosDfmCompareFunType::set_fs &docs,
		AosDfmCompareFunType::setitr_fs &itr,
		const AosSlabBlockFilePtr &slabFile,
		const u32 &seqno)
{
	bool rslt = false;
	u32 blocksize = slabFile->getBlockSize(); 
	u32 save_num_docs = eBatchBlockSize / blocksize;
	if (save_num_docs == 0) OmnScreen << " blocksize (" << blocksize << ") > eBatchBlockSize(" << eBatchBlockSize << ")" << endl;

	while (itr != docs.end() && (*itr)->getBodySeqno() == seqno)
	{
		u32 expect_num = save_num_docs;
		u32 begin_blockid = (*itr)->getBodyOffset();
		AosBuffPtr buff = slabFile->readDocs(rdata, begin_blockid, expect_num);
		aos_assert_r(buff && buff->dataLen() == expect_num * blocksize, false);

		for (; itr != docs.end(); ++itr)
		{
			AosDfmDocPtr doc = *itr;
			if (doc->getBodySeqno() != seqno) break;

			u32 blockid = doc->getBodyOffset();
			aos_assert_r(blockid >= begin_blockid, false);
			if (blockid > (begin_blockid + expect_num -1)) break;

			u32 idx = blockid - begin_blockid;
			char *data = (doc->getBodyBuff())->data();
			rslt = slabFile->saveDocToBuff(idx, buff, doc->getOrigLen(), doc->getCompressLen(), data);
			aos_assert_r(rslt, false);

			if(mShowLog)
			{
				OmnScreen << "DocFileMgr; DfmBody; save:"
					<< "; virtual_id:" << mVirtualId
					<< "; dfm_id:" << mDfmId
					<< "; docid:" << doc->getDocid() 
					<< "; bodySeqno:" << seqno
					<< "; bodyOffset:" << blockid
					<< "; orig_len:" << doc->getOrigLen() 
					<< "; compress_len:" << doc->getCompressLen() 
					<< endl;
			}
		}
		rslt = slabFile->addDocs(rdata, begin_blockid, buff->data(), buff->dataLen());
		aos_assert_r(rslt, false);
	}
	return true;
}
*/
		

