////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// 	IMPORTANT: This class assumes the directory has enough space.
//
// Modification History:
// 10/12/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "FmtMgr/FmtFile.h"

#include "FmtMgr/Fmt.h"
#include "Rundata/Rundata.h"
#include "SEUtil/LogFile.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/Buff.h"

#include <dirent.h>

OmnString	sgNormSuffix = ".norm";
OmnString	sgProcedSuffix = ".proced";
OmnString	sgMergeSuffix = ".merge";

AosFmtFile::AosFmtFile(
		const OmnString &dirname, 
		const OmnString &fname, 
		const u64 max_fsize,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mDirname(dirname),
mFilename(fname),
mMaxFileSize(max_fsize),
mShowLog(show_log),
mNextNormSeqno(0),
mNextMergeSeqno(0)
{
}


AosFmtFile::~AosFmtFile()
{
}


bool
AosFmtFile::init()
{
	// This class has a filename map. This function creates the map by 
	// reading in all the files. It is a u32 map. The key is the file
	// creation time (seconds). 
	//
	
	DIR *dir;
	struct dirent *file;
	if((dir = opendir(mDirname.data())) == NULL)
	{
		mkdir(mDirname.data(), 0755);
		return true;
	}
	
	while((file = readdir(dir)) != NULL )
	{
		OmnString fname = file->d_name;
		if(fname == "." || fname == "..")	continue;
		if(!fname.hasPrefix(mFilename))	continue;
		
		// find a fname.
		u32 file_seq = AosLogFile::getSeqnoFromFname(fname);
		OmnString suffix = AosLogFile::getFnameSuffix(fname);
		
		OmnString full_fname = mDirname;
		full_fname << "/" << fname;
		initFileMap(file_seq, suffix, full_fname);
	}

	initNextNormSeq();
	initNextMergeSeq();
	return true;
}


bool
AosFmtFile::initFileMap(
		const u32 file_seq,
		const OmnString &suffix,
		const OmnString &full_fname)
{
	if(suffix == sgNormSuffix || suffix == sgProcedSuffix)
	{
		mNormFname.insert(make_pair(file_seq, full_fname));
		return true;
	}
	
	aos_assert_r(suffix == sgMergeSuffix, false);	
	mMergeFname.insert(make_pair(file_seq, full_fname));
	return true;
}


void
AosFmtFile::initNextNormSeq()
{
	// init nextNormSeqno
	map<u32, OmnString>::reverse_iterator rit = mNormFname.rbegin();
	if(rit != mNormFname.rend())
	{
		mNextNormSeqno = rit->first + 1;
	}
}

void
AosFmtFile::initNextMergeSeq()
{
	map<u32, OmnString>::reverse_iterator rit = mMergeFname.rbegin();
	if(rit != mMergeFname.rend())
	{
		//mNextMergeSeqno = rit->first + 1;
		mNextMergeSeqno = rit->first + 2;	// set seq un continue;
	}
}


bool
AosFmtFile::addData(
		const char *data,
		const u32 data_len,
		const bool need_proc)
{
	// the norm data is need proc.
	mLock->lock();
	bool rslt = getActiveFile(data_len, need_proc);
	aos_assert_rl(rslt && mActiveFile, mLock, false);
	
	mActiveFile->append(data, data_len, true);
	
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; saveFmt:" << data_len << endl;
	}
	mLock->unlock();
	return true;
}

bool
AosFmtFile::addFmt(
		const AosBuffPtr &fmt_buff,
		const bool need_proc)
{
	// the norm data is need proc.
	mLock->lock();

	u32 data_len = fmt_buff->dataLen();
	bool rslt = getActiveFile(data_len, need_proc);
	aos_assert_rl(rslt && mActiveFile, mLock, false);
	
	//mActiveFile->append(data, data_len, true);
	mActiveFile->append((char *)&data_len, sizeof(u32), false);
	mActiveFile->append(fmt_buff->data(), data_len, true);
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; saveFmt:" << data_len << endl;
	}
	mLock->unlock();
	return true;
}



bool
AosFmtFile::getActiveFile(
		const u32 data_len,
		const bool need_proc) 
{
	OmnString suffix = need_proc ? sgNormSuffix : sgProcedSuffix;
	if(!mActiveFile)	return createActiveFile(suffix);

	OmnString fname = mActiveFile->getFileName();
	OmnString crt_suffix = AosLogFile::getFnameSuffix(fname);
	if(crt_suffix != suffix)
	{
		OmnShouldNeverComeHere;
		return createActiveFile(suffix);
	}

	if(mActiveFile->getLength() + data_len > mMaxFileSize)
	{
		return createActiveFile(suffix);
	}

	return true;
}


bool
AosFmtFile::createActiveFile(const OmnString &suffix)
{
	u32 file_seq = mNextNormSeqno++;
	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << file_seq << suffix;

	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);

	mNormFname.insert(make_pair(file_seq, full_fname));
	mActiveFile = file;

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; createNormFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}
	return true;
}


int 
AosFmtFile::begSeq()
{
	mLock->lock();	
	int beg_seq = begSeqPriv();
	mLock->unlock();
	return beg_seq;	
}

int
AosFmtFile::endSeq()
{
	mLock->lock();
	int end_seq = endSeqPriv();
	mLock->unlock();
	return end_seq;
}

int
AosFmtFile::begSeqPriv()
{
	map<u32, OmnString>::iterator itr = mNormFname.begin();
	if(itr == mNormFname.end())
	{
		return -1;
	}

	return itr->first;
}

int
AosFmtFile::endSeqPriv()
{
	map<u32, OmnString>::reverse_iterator r_itr = mNormFname.rbegin();
	if(r_itr == mNormFname.rend()) return -1;	

	return r_itr->first;
}


int
AosFmtFile::activeSeq()
{
	// if no activeFile. maybe the caller call startNextActive()
	mLock->lock();
	int act_seq = activeSeqPriv();
	mLock->unlock();
	return act_seq;
}

int
AosFmtFile::activeSeqPriv()
{
	// if no activeFile. maybe the caller call startNextActive()
	int act_seq = -1;
	if(mActiveFile)
	{
		act_seq = AosLogFile::getSeqnoFromFname(mActiveFile->getFileName());
		int end_seq = endSeqPriv();
		aos_assert_r(act_seq == end_seq, -1);
	}
	return act_seq;
}

int
AosFmtFile::getNextSeqno(const int crt_seqno)
{
	map<u32, OmnString>::iterator itr;
	
	mLock->lock();
	if(crt_seqno == -1)
	{
		itr = mNormFname.begin();
	}
	else
	{
		itr = mNormFname.find(crt_seqno);
		aos_assert_rl(itr !=mNormFname.end(), mLock, -2);
		itr++;
	}
	
	if(itr == mNormFname.end())
	{
		mLock->unlock();
		return -1;
	}

	int next_seqno = itr->first;
	mLock->unlock();
	return next_seqno;
}


bool
AosFmtFile::startNextActive()
{
	mLock->lock();
	mActiveFile = 0;         
	mLock->unlock();         
	return true;
}

	
bool
AosFmtFile::readNormFile( const int file_seqno, AosBuffPtr &file_buff)
{
	aos_assert_r(file_seqno>=0 , false);

	mLock->lock();
	OmnFilePtr file = openNormFile((u32)file_seqno);
	mLock->unlock();
	
	bool rslt = AosLogFile::readFileStatic(file, file_buff);
	aos_assert_r(rslt, false);
	return true;
}

	
OmnFilePtr
AosFmtFile::openNormFile(const u32 seqno)
{
	map<u32, OmnString>::iterator itr = mNormFname.find(seqno); 
	aos_assert_r(itr !=mNormFname.end(), 0);

	OmnString full_fname = itr->second;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), 0);

	return file;
}


/*
bool
AosFmtFile::addMergeData(const char *data, const u32 data_len)
{
	mLock->lock();
	if(!mMergeFile || mMergeFile->getLength() + data_len > mMaxFileSize)
	{
		createMergeFile();
	}
	aos_assert_rl(mMergeFile, mLock, false);
	
	mMergeFile->append(data, data_len, true);
	mLock->unlock();
	return true;
}
*/

	
bool
AosFmtFile::addMergeFmt(const AosBuffPtr &fmt_buff)
{
	mLock->lock();
	
	u32 data_len = fmt_buff->dataLen();
	if(!mMergeFile || mMergeFile->getLength() + data_len > mMaxFileSize)
	{
		createMergeFile();
	}
	aos_assert_rl(mMergeFile, mLock, false);
	
	//mMergeFile->append(data, data_len, true);
	mMergeFile->append((char *)&data_len, sizeof(u32), false);
	mMergeFile->append(fmt_buff->data(), data_len, true);
	mLock->unlock();
	return true;
}


bool
AosFmtFile::createMergeFile()
{
	u32 merge_fseq = mNextMergeSeqno++;

	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << merge_fseq << sgMergeSuffix;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);
	
	mMergeFile = file;
	mMergeFname.insert(make_pair(merge_fseq, full_fname));

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; createFmtMergeFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}
	return true;
}


int 
AosFmtFile::mergeBegSeq()
{
	int beg_seq = -1;
	mLock->lock();	
	map<u32, OmnString>::iterator itr = mMergeFname.begin();
	if(itr != mMergeFname.end())
	{
		beg_seq = itr->first;
	}

	mLock->unlock();
	return beg_seq;	
}


bool
AosFmtFile::readMergeFile(const int file_seq, AosBuffPtr &file_buff)
{
	aos_assert_r(file_seq>=0 , false);

	mLock->lock();
	OmnFilePtr file = openMergeFile((u32)file_seq);
	mLock->unlock();

	bool rslt = AosLogFile::readFileStatic(file, file_buff);                           
	aos_assert_r(rslt, false);
	return true;
}


OmnFilePtr 
AosFmtFile::openMergeFile(const u32 seqno)
{
	map<u32, OmnString>::iterator itr = mMergeFname.find(seqno);
	aos_assert_r(itr != mMergeFname.end(), 0);

	OmnString full_fname = itr->second; 
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), 0);

	return file;
}


bool
AosFmtFile::mergeNextAreaFiles()
{
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; merge next area:"
			<< endl;
	}
	
	if(mMergeFname.size() == 0)	return true;
	
	mLock->lock();

	// find un continueous file.
	map<u32, OmnString> fnames;
	map<u32, OmnString>::iterator itr = mMergeFname.begin();
	aos_assert_rl(itr != mMergeFname.end(), mLock, false);
		
	fnames.insert(make_pair(itr->first, itr->second));
	u32 next_seq = itr->first + 1;
	for(itr++; itr != mMergeFname.end(); itr++, next_seq++)
	{
		int uncont_seq = itr->first;
		if((u32)uncont_seq != next_seq)	break;
		//fnames.insert(itr);
		fnames.insert(make_pair(itr->first, itr->second));
	}
	
	if(fnames.size())
	{
		bool rslt = addToNormEnd(fnames);
		aos_assert_rl(rslt, mLock, false);
		
		mMergeFname.erase(mMergeFname.begin(), itr);
	}
	if(mMergeFname.size() == 0) mMergeFile = 0;

	mLock->unlock();
	return true;
}


bool
AosFmtFile::addToNormEnd(map<u32, OmnString> &fnames)
{
	// rename the file_name to the normal fname.
	if(fnames.size() == 0)	return true;

	int new_seq = endSeqPriv() + 1;
	aos_assert_r(new_seq >=0, false);

	map<u32, OmnString>::iterator itr = fnames.begin();
	for(; itr != fnames.end(); itr++)
	{
		OmnString merge_fname = itr->second;
	
		u32 file_seq = new_seq++;
		OmnString new_fname = mDirname;
		new_fname << "/" << mFilename << "_" << file_seq << sgNormSuffix;
		
		int rslt = rename(merge_fname.data(), new_fname.data());
		aos_assert_r(rslt == 0, false);
		
		mNormFname.insert(make_pair(file_seq, new_fname));
		if(mShowLog)
		{
			OmnScreen << "FmtMgr; rename to normfile:"
				<< " old_fname:" << merge_fname
				<< "; new_fname:" << new_fname
				<< endl;
		}
	}

	// reset nextActiveSeqno
	mNextNormSeqno = new_seq;
	mActiveFile = 0;
	return true;
}


int
AosFmtFile::endProcedSeq()
{
	mLock->lock();
	map<u32, OmnString>::reverse_iterator r_itr = mNormFname.rbegin();

	int seqno = -1;
	for(; r_itr != mNormFname.rend(); r_itr++)
	{
		OmnString fname = r_itr->second;
		OmnString suffix = AosLogFile::getFnameSuffix(fname);
		if(suffix == sgProcedSuffix)
		{
			seqno = r_itr->first;
			break;
		}
	}
	mLock->unlock();

	return seqno;
}

bool
AosFmtFile::renameToProced(const int seq)
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mNormFname.find(seq);
	aos_assert_rl(itr != mNormFname.end(), mLock, false);
	
	OmnString old_fname = itr->second;
	OmnString new_fname = mDirname;
	new_fname << "/" << mFilename << "_" << seq << sgProcedSuffix;

	int rslt = rename(old_fname.data(), new_fname.data());
	aos_assert_rl(rslt == 0, mLock, false);
	
	itr->second = new_fname;
	//mNormFname.insert(make_pair(seq, new_fname));
	mLock->unlock();

	return true;	
}


int
AosFmtFile::getNextProcedSeq(const int crt_seqno)
{
	map<u32, OmnString>::iterator itr;
	
	mLock->lock();
	if(crt_seqno == -1)
	{
		itr = mNormFname.begin();
	}
	else
	{
		itr = mNormFname.find(crt_seqno);
		aos_assert_rl(itr !=mNormFname.end(), mLock, -2);
		itr++;
	}
	
	if(itr == mNormFname.end())
	{
		mLock->unlock();
		return -1;
	}
	
	OmnString fname = itr->second; 
	OmnString suffix = AosLogFile::getFnameSuffix(fname);
	if(suffix == sgProcedSuffix)
	{
		int next_seqno = itr->first;
		mLock->unlock();
		return next_seqno;
	}

	mLock->unlock();
	return -1;
}
	

u64
AosFmtFile::getMaxFmtIdByFileSeq(const u32 seqno)
{
	AosBuffPtr cont;
	bool rslt = readNormFile(seqno, cont);	
	aos_assert_r(rslt && cont, 0);

	int crt_idx;
	u32 fmt_len;
	u64 max_fmt_id = 0, crt_fmt_id;
	while(1)
	{
		fmt_len = cont->getU32(0);
		if(fmt_len == 0)	break;
		
		crt_idx = cont->getCrtIdx();
		
		crt_fmt_id = AosFmt::getFmtIdFromBuff(cont);
		aos_assert_r(crt_fmt_id && crt_fmt_id > max_fmt_id, false);
		max_fmt_id = crt_fmt_id;
		
		cont->setCrtIdx(crt_idx + fmt_len);
	}
	return max_fmt_id;
}



bool
AosFmtFile::deleteFile(const u32 file_seq)
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mNormFname.find(file_seq); 
	aos_assert_rl(itr !=mNormFname.end(), mLock, false);

	OmnString fname = itr->second;
	int rslt = unlink(fname.data());
	aos_assert_rl(rslt>=0, mLock, false);
	
	mNormFname.erase(itr);

	mLock->unlock();

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; delete file:"
			<< " fname:" << fname
			<< endl;
	}
	return true;	
}


bool
AosFmtFile::deleteAllFiles()
{
	OmnString fname;
	int rslt;

	mLock->lock();
	map<u32, OmnString>::iterator itr = mNormFname.begin(); 
	for(; itr != mNormFname.end(); itr++)
	{
		fname = itr->second;
		rslt = unlink(fname.data());
		aos_assert_rl(rslt>=0, mLock, false);
		
		if(mShowLog)
		{
			OmnScreen << "FmtMgr; sorFmt delete file:"
				<< " fname:" << fname
				<< endl;
		}
	}
	
	mNormFname.clear();
	mLock->unlock();
	return true;	
}


/*
bool
AosFmtFile::deleteFilesToSeq(const u32 end_seq)
{
	// the end_seq will been deleted. 
	//
	mLock->lock();
	
	map<u32, OmnString>::iterator itr = mNormFname.begin();
	for(; itr != mNormFname.end(); itr++)
	{
		u32 seqno = itr->first;
		if(seqno > end_seq)	break;

		OmnString fname = itr->second;
		int rslt = unlink(fname.data());
		aos_assert_rl(rslt>=0, mLock, false);

		if(mShowLog)
		{
			OmnScreen << "FmtMgr; delete file:"
				<< " fname:" << fname
				<< endl;
		}
	}
	mNormFname.erase(mNormFname.begin(), itr);

	if(mActiveFile)
	{
		int act_seq = AosLogFile::getSeqnoFromFname(mActiveFile->getFileName());
		aos_assert_r(act_seq >=0, false);
		
		if((u32)act_seq <= end_seq) mActiveFile = 0;
	}
	
	mLock->unlock();
	return true;
}
*/

