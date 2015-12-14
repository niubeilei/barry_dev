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
#include "DocFileMgr/ReadOnlyBlock.h"

#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/DfmReadOnlyFile.h"
#include "ReliableFile/ReliableFile.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"


AosReadOnlyBlock::AosReadOnlyBlock(
		const u32 v_id,
		const u32 dfm_id,
		const OmnString &prefix,
		vector<AosDfmFilePtr> &files,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mCrtSeqno(eAosInvFseqno),
mCrtLastOffset(eAosInvFoffset),
mFile(0),
mReadFile(0),
mReadSeqno(eAosInvFseqno),
mVirtualId(v_id),
mDfmId(dfm_id),
mPrefix(prefix),
mShowLog(show_log)
{
	for(u32 i=0; i<files.size(); i++)
	{
		if(files[i]->getType() != AosDfmFileType::eReadOnlyFile) continue;
		
		AosDfmReadOnlyFilePtr dfm_file = (AosDfmReadOnlyFile *)files[i].getPtr();
		u32 seqno = dfm_file->getFileSeqno();
		mDfmFiles.insert(make_pair(seqno, dfm_file));
		if(seqno > mCrtSeqno) mCrtSeqno = seqno;
	}
}


AosReadOnlyBlock::~AosReadOnlyBlock()
{
}


bool
AosReadOnlyBlock::removeDoc()
{
	return false;
}


bool
AosReadOnlyBlock::addDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		u64 &offset,
		const char *data,
		const u32 orig_len,
		const u32 compress_len)
{
	u32 docsize = compress_len ? compress_len : orig_len;
	aos_assert_r(docsize <= eMaxFileSize, false);

	bool rslt;
	mLock->lock();
	if (!mFile || docsize > eMaxFileSize - mCrtLastOffset)
	{
		rslt = createFileLocked(dfm, seqno, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	aos_assert_rl(mFile, mLock, false);

	seqno = mCrtSeqno;
	offset = mCrtLastOffset;

	aos_assert_rl(offset < eMaxFileSize, mLock, false); 
	
	mFile->setU32(offset, orig_len, false, rdata.getPtr());
	mFile->setU32(offset + sizeof(u32), compress_len, false, rdata.getPtr());
	mFile->put(offset + sizeof(u32) + sizeof(u32), data, docsize, true, rdata.getPtr());

	mCrtLastOffset += (docsize + sizeof(u32) + sizeof(u32));
	mLock->unlock();
	
	seqno = seqno | 0x40000000;
	if(mShowLog)
	{
		OmnScreen << "DocFileMgr; DfmBody; readonly. add finish:"
			<< "; seqno:" << seqno 
			<< "; offset:" << offset 
			<< "; orig_len:" << orig_len
			<< "; compress_len:" << compress_len
			<< endl;
	}
	return true;
}


bool
AosReadOnlyBlock::readDoc(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u64 offset,
		AosBuffPtr &data_buff,
		u32 &orig_len,
		u32 &compress_len)
{
	// modified by ketty.	2013/01/26
	
	bool rslt;
	mLock->lock();
	if (!mReadFile || mReadSeqno != seqno)
	{
		rslt = openFileLocked(seqno, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	aos_assert_rl(mReadFile, mLock, false);

	data_buff = OmnNew AosBuff(eAosSizePerRead, 0 AosMemoryCheckerArgs);
	memset(data_buff->data(), 0, eAosSizePerRead);
	
	rslt = mReadFile->readToBuff(offset, eAosSizePerRead, data_buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	data_buff->setDataLen(eAosSizePerRead);

	u32 data_off = sizeof(u32) + sizeof(u32);

	orig_len = data_buff->getU32(0);
	compress_len = data_buff->getU32(0);
	//aos_assert_r(orig_len >= compress_len, false);	
	if(orig_len < compress_len)
	{
		OmnAlarm << "ReadOnly Error! orig_len" << orig_len
			<< "; compress_len:" << compress_len
			<< enderr;
		return false;
	}

	u32 data_len = compress_len ? compress_len : orig_len;
	if(data_len <= eAosSizePerRead - data_off)
	{
		// We need to remove 'orig_len' and 'compress_len'.
		char *data = data_buff->data();
		memmove(data, &data[data_off], data_len);
		data_buff->setDataLen(data_len);
		mLock->unlock();
		return true;
	}

	// means need continue read.
	u32 read_size = eAosSizePerRead - data_off;
	AosBuffPtr big_buff = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs);
	memset(big_buff->data(), 0, data_len);
	big_buff->setBuff(data_buff->data() + data_off, read_size);

	u32 remain_size = data_len - read_size; 
	char * remain_data = big_buff->data() + read_size; 
	rslt = mReadFile->readToBuff(offset + eAosSizePerRead, remain_size, remain_data, rdata.getPtr());
	aos_assert_r(rslt, false);
	big_buff->setDataLen(data_len);

	data_buff = big_buff;	
	mLock->unlock();

	return true;
}

/*
bool
AosReadOnlyBlock::createFileLocked(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		const AosRundataPtr &rdata)
{
	aos_assert_r(dfm, false);

	u64 file_id = 0;
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	AosReliableFilePtr ff = vfsMgr->createRlbFile(
			        rdata, mVirtualId, file_id, mPrefix, eAosMaxFileSize,
					AosRlbFileType::eLog, true);
	aos_assert_r(ff && file_id, false);
	seqno = (++mCrtSeqno);
	mFile = ff;
	mCrtLastOffset = mFile->getLength();

	mSeqToFIdMap.insert(make_pair(seqno, file_id));
	dfm->addNewFile(rdata, AosDfmFileType::eReadOnlyFile, seqno, file_id);
	
	
	aos_assert_r(mCrtLastOffset < eMaxFileSize, false); 
	return true;
}


bool
AosReadOnlyBlock::openFileLocked(
		const u32 &seqno,
		const AosRundataPtr &rdata)
{
	map<u32, u64>::iterator f_itr = mSeqToFIdMap.find(seqno);
	if(f_itr == mSeqToFIdMap.end())
	{
		return 0;
	}

	u64 file_id = f_itr->second;
	aos_assert_rl(file_id, mLock, 0);   

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rl(vfsMgr, mLock, 0);
	AosReliableFilePtr ff = vfsMgr->openRlbFile(
					file_id, AosRlbFileType::eLog, rdata);
	aos_assert_rl(ff && ff->isGood(), mLock, 0);
	
	mReadFile = ff;
	mReadSeqno = seqno;
	return true;
}
*/

bool
AosReadOnlyBlock::createFileLocked(
		const AosDocFileMgrObjPtr &dfm,
		u32 &seqno,
		const AosRundataPtr &rdata)
{
	aos_assert_r(dfm, false);

	seqno = (++mCrtSeqno);
	AosDfmReadOnlyFilePtr dfm_file = OmnNew AosDfmReadOnlyFile(rdata, 
			seqno, mVirtualId, mPrefix, eAosMaxFileSize);
	aos_assert_r(dfm_file, false);	
	
	mDfmFiles.insert(make_pair(seqno, dfm_file));
	dfm->addBodyFile(rdata, dfm_file);
	
	mFile = dfm_file->getFile(rdata);
	aos_assert_r(mFile, false);
	mCrtLastOffset = mFile->getLength();

	aos_assert_r(mCrtLastOffset < eMaxFileSize, false); 
	return true;
}


bool
AosReadOnlyBlock::openFileLocked(
		const u32 &seqno,
		const AosRundataPtr &rdata)
{
	FileMapItr itr = mDfmFiles.find(seqno);
	if(itr == mDfmFiles.end())	return 0;

	AosDfmReadOnlyFilePtr dfm_file = itr->second;
	mReadFile = dfm_file->getFile(rdata);		
	mReadSeqno = seqno;
	
	aos_assert_r(mReadFile && mReadFile->isGood(), false);
	return true;
}


bool
AosReadOnlyBlock::sanityCheck(const u32 blockId)
{
	return true;
}


