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
// A Log File is a sequence of files:
// 	dir/fname + "_" + <seqno>.
// 	A log file has a pre-specified max file size. When the current
// 	log file size is bigger than the max size, a new log file will
// 	be created. 
//
// Processing Finished Entries
//  Log entries must be manually finished. Finished log entries are not removed
//  immediately. They are batch processed at a given frequency. When it is the
//  time to process finished entries, the requester should call the member 
//  function:
//  	startRead();
//  It returns the last log file to be processed.
//
// 	Initially, all log files are saved in the format:
// 		dir/fname + "_" + <seqno>
//
//  These log files are processed one at a time. During the process, all 
//  unfinished logs are saved in a new log file whose name is in the format:
//  	dir/fname + "_" + <seqno> + ".stale"
//  where the seqno starts from 0.
//
//  After all log files are processed, it does the following:
//  	1. All ".stable" files are renamed to be ".merge". 
//  	2. All processed log files are removed. 
//  	3. All ".merge" files are renamed to normal log files whose seqnos are
//  	   determeined as:
//  	   If the first new log file seqno is N:
//  	   a. the last ".merge" log file seqno is N-1;
//  	   b. the second last ".merge" log file seqno is N-2;
//		   c. (it is the same for all other ".merge" files)
//  
//  How to recover:
//  When the system comes back, it checks:
//  1. If there are no ".stale" and ".merge" files, do nothing.
//  2. If there are ".stable" files but no ".merge" files, it means the system
//     crashed while it was processing the log files. It needs to remove all
//     ".stale" files. 
//  2. If there are ".stale" files and ".merge" files, it means the system 
//     crahsed when it was renaming ".stale" files to ".merge" files. In this
//     case, it continues the process.
//  3. If there are no ".stale" but ".merge" files, it means the system crashed
//     when it was deleting processed log files. In this case, continue the 
//     processing.
//
//  Note: 
//  This algorithm assumes that log file seqnos are continuous. When it is
//  the time to process log files, the current log file is closed. The next
//  seqno is not used. A new log file is created after the next seqno. 
//  During recovery, all log files whose seqnos are continuous are the
//  processed log files. 
//
// 	IMPORTANT: This class assumes the directory has enough space.
//
// Modification History:
// 10/12/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransServer/TransFile.h"

#include "Rundata/Rundata.h"
#include "TransBasic/AppMsg.h"
#include "Thread/Mutex.h"
#include "TransBasic/Trans.h"
#include "SEUtil/LogFile.h"
#include "Util/File.h"
#include "Util/Buff.h"

#include <dirent.h>

static OmnString	sgNormSuffix = ".norm";
static OmnString	sgStaleSuffix = ".stale";
static OmnString	sgMergeSuffix = ".merge";
static OmnString	sgNewerSuffix = ".newer";

AosTransFile::AosTransFile(
		const OmnString &dirname, 
		const OmnString &fname, 
		const u64 max_fsize,
		const bool show_log)
:
//mCrtStaleSuffixSeq(0)
mLock(OmnNew OmnMutex()),
mDirname(dirname),
mFilename(fname),
mMaxFileSize(max_fsize),
mShowLog(show_log),
mNextNormSeqno(0),
mStaleBoundSeq(-1)
{
}


AosTransFile::~AosTransFile()
{
}

bool
AosTransFile::start()
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
	
	startPriv();
	return true;
}


bool
AosTransFile::initFileMap(
		const u32 file_seq,
		const OmnString &suffix,
		const OmnString &full_fname)
{
	if(suffix == sgNormSuffix)
	{
		mNormFname.insert(make_pair(file_seq, full_fname));
		return true;
	}
	
	if(suffix == sgStaleSuffix)
	{
		mStaleFname.insert(make_pair(file_seq, full_fname));	
		return true;
	}

	if(suffix == sgMergeSuffix)
	{
		mMergeFname.insert(make_pair(file_seq, full_fname));	
		return true;
	}
	
	aos_assert_r(suffix == sgNewerSuffix, false);
	mNewerFname.insert(make_pair(file_seq, full_fname));
	return true;
}


void
AosTransFile::initNextNormSeq()
{
	// init nextNormSeqno
	map<u32, OmnString>::reverse_iterator rit = mNormFname.rbegin();
	if(rit != mNormFname.rend())
	{
		mNextNormSeqno = rit->first + 1;
	}
}


bool
AosTransFile::startPriv()
{
	bool rslt;
	if(mMergeFname.size() != 0)
	{
		if(mStaleFname.size() != 0)
		{
			map<u32, OmnString>::iterator itr = mMergeFname.begin();
			aos_assert_r(itr != mMergeFname.end(), false);
			int seq = itr->first; 
			aos_assert_r(seq >0 && (u32)seq >= mStaleFname.size(), false);
			
			mStaleBoundSeq = seq-1; 
		}

		// continue merge files.
		rslt = mergeStaleFiles();
		aos_assert_r(rslt, false);
	}

	if(mStaleFname.size() != 0)
	{
		rslt = deleteStaleFiles();
		aos_assert_r(rslt, false);
	}

	if(mNewerFname.size() != 0)
	{
		rslt = deleteNewerFiles();
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosTransFile::deleteStaleFiles()
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mStaleFname.begin();	
	for(; itr != mStaleFname.end(); itr++)
	{
		OmnString fname = itr->second;
		int rslt = unlink(fname.data());
		aos_assert_r(rslt != -1, false);
		if(mShowLog)
		{
			OmnScreen << "TransServer; deleteStaleFile"
				<< "; fname:" << fname
				<< endl;
		}
	}
	mStaleFname.clear();
	mStaleFile = 0;
	mLock->unlock();
	return true;
}


bool
AosTransFile::deleteNewerFiles()
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mNewerFname.begin();	
	for(; itr != mNewerFname.end(); itr++)
	{
		OmnString fname = itr->second;
		int rslt = unlink(fname.data());
		aos_assert_r(rslt != -1, false);
		if(mShowLog)
		{
			OmnScreen << "TransServer; deleteNewerFile"
				<< "; fname:" << fname
				<< endl;
		}
	}
	mNewerFname.clear();
	mNewerFile = 0;
	mLock->unlock();
	return true;
}

/*
bool
AosTransFile::saveToNormFile(const AosTransPtr &trans)
{
	AosBuffPtr data = trans->getConnBuff();
	aos_assert_r(data, false);
	u32 data_len = data->dataLen();
	
	mLock->lock();
	if(!mActiveFile || mActiveFile->getLength() + data_len > mMaxFileSize)
	{
		createActiveFile();
	}
	aos_assert_rl(mActiveFile, mLock, false);

	mActiveFile->append((char *)&data_len, sizeof(u32), false);
	mActiveFile->append(data->data(), data_len, true);
	mLock->unlock();
	return true;	
}
*/

bool
AosTransFile::saveToNormFile(const AosBuffPtr &trans_buff)
{
	aos_assert_r(trans_buff, false);
	
	u32 data_len = trans_buff->dataLen();
	
	mLock->lock();
	if(!mActiveFile || mActiveFile->getLength() + data_len > mMaxFileSize)
	{
		createActiveFile();
	}
	aos_assert_rl(mActiveFile, mLock, false);
	
	if(mShowLog)
	{
		OmnScreen << "save Trans to norm file:" 
			<< "; data_len:" << data_len
			<< "; fname:" << mActiveFile->getFileName()
			<< "; file_len:" << mActiveFile->getFileCrtSize()
			<< endl;
	}

	mActiveFile->append(trans_buff->data(), data_len, true);
	mLock->unlock();
	return true;	
}


/*
AosBuffPtr
AosTransFile::generateBigBuff(vector<AosTransPtr> &v_trans)
{
	AosBuffPtr big_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	for(u32 i=0; i<v_trans.size(); i++)
	{
		AosTransPtr trans = v_trans[i];
		aos_assert_r(trans->isNeedSave(), 0);

		AosBuffPtr trans_buff = trans->getConnBuff();
		aos_assert_r(trans_buff, 0);
		big_buff->setU32(trans_buff->dataLen());
		big_buff->setBuff(trans_buff);
	}
	return big_buff;	
}
*/

bool
AosTransFile::createActiveFile()
{
	u32 file_seq = mNextNormSeqno++;
	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << file_seq << sgNormSuffix;

	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);

	mNormFname.insert(make_pair(file_seq, full_fname));
	mActiveFile = file;

	if(mShowLog)
	{
		OmnScreen << "TransServer; createNormFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}
	return true;
}


bool
AosTransFile::saveToStaleFile(const AosBuffPtr &data)
{
	aos_assert_r(data, false);
	u32 data_len = data->dataLen();

	mLock->lock();
	if(!mStaleFile || mStaleFile->getLength() + data_len > mMaxFileSize)
	{
		createStaleFile();
	}
	aos_assert_rl(mStaleFile, mLock, false);
	
	mStaleFile->append(data->data(), data_len, true);
	mLock->unlock();
	return true;
}


bool
AosTransFile::createStaleFile()
{
	u32 stale_fseq = mStaleFname.size();
	
	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << stale_fseq << sgStaleSuffix;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);
	mStaleFile = file;
	
	mStaleFname.insert(make_pair(stale_fseq, full_fname));

	if(mShowLog)
	{
		OmnScreen << "TransServer; createStaleFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}

	return true;
}


bool
AosTransFile::saveToNewerFile(const AosTransPtr &trans)
{
	AosBuffPtr buff = trans->getConnBuff();
	u32 data_len = buff->dataLen();

	mLock->lock();
	if(!mNewerFile || mNewerFile->getLength() + data_len > mMaxFileSize)
	{
		createNewerFile();
	}
	aos_assert_rl(mNewerFile, mLock, false);
	
	if(mShowLog)
	{
		OmnScreen << "save Trans to newer file:" 
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; data_len:" << data_len
			<< "; fname:" << mNewerFile->getFileName()
			<< "; file_len:" << mNewerFile->getFileCrtSize()
			<< endl;
	}

	
	mNewerFile->append((char *)&data_len, sizeof(u32), false);
	mNewerFile->append(buff->data(), data_len, true);
	mLock->unlock();
	return true;
}


bool
AosTransFile::createNewerFile()
{
	u32 newer_fseq = mNewerFname.size();
	
	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << newer_fseq << sgNewerSuffix;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);
	mNewerFile = file;
	
	mNewerFname.insert(make_pair(newer_fseq, full_fname));

	if(mShowLog)
	{
		OmnScreen << "TransServer; createNewerFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}

	return true;
}

bool
AosTransFile::setStaleBoundSeq(const int seq)
{
	aos_assert_r(seq >=0, false);
	mStaleBoundSeq = seq; 
	aos_assert_r(seq >=0 && (u32)mStaleBoundSeq >= mStaleFname.size(), false);
	return true;
}


bool
AosTransFile::mergeStaleFiles()
{
	// maybe TransSaver read all file. but don't create stale file.
	// maybe no stalefile and no mergefile. but has mStaleBoundSeq.
	// then need delete this mStaleBoundSeq files.
	//if(mStaleFname.size() == 0 && mMergeFname.size() == 0)		return true;
	if(mShowLog)
	{
		OmnScreen << "TransServer; merge stale files"
			<< "; stale_bound_seq:" << mStaleBoundSeq 
			<< "; stale_file_num:" << mStaleFname.size()
			<< endl;
	}
	if(mStaleFname.size() != 0)
	{
		aos_assert_r(mStaleBoundSeq >=0 && (u32)mStaleBoundSeq + 1 >= mStaleFname.size(), false);
	}
	
	mLock->lock();
	int merge_seq = mStaleBoundSeq;
	map<u32, OmnString>::reverse_iterator ritr = mStaleFname.rbegin();
	for(; ritr != mStaleFname.rend(); ritr++)
	{
		OmnString stale_fname = ritr->second;

		int new_seq = merge_seq--;
		aos_assert_r(new_seq >=0, false);
		OmnString merge_fname = mDirname;
		merge_fname << "/" << mFilename << "_" << new_seq << sgMergeSuffix; 
		
		int rslt = rename(stale_fname.data(), merge_fname.data());
		aos_assert_rl(rslt == 0, mLock, false);
		mMergeFname.insert(make_pair(new_seq, merge_fname));
		
		if(mShowLog)
		{
			OmnScreen << "TransServer; rename to mergefile:"
				<< " old_fname:" << stale_fname
				<< "; new_fname:" << merge_fname
				<< endl;
		}
	}
	mStaleFname.clear();
	mStaleFile = 0;		// Ketty 2013/01/12
	
	// when svr is up. maybe continue mergeFiles.
	int delete_end = mStaleBoundSeq;	// when no mMergeFnames. delete_end using mStaleBoundSeq
	ritr = mMergeFname.rbegin();
	if(ritr != mMergeFname.rend())
	{
		delete_end = ritr->first;
	}

	bool rslt = deleteFilesToSeq(delete_end);
	aos_assert_rl(rslt, mLock, false);

	for(ritr = mMergeFname.rbegin(); ritr != mMergeFname.rend(); ritr++)
	{
		OmnString merge_fname = ritr->second;
		
		u32 file_seq = AosLogFile::getSeqnoFromFname(merge_fname);
		OmnString new_fname = mDirname;
		new_fname << "/" << mFilename << "_" << file_seq << sgNormSuffix;

		int rslt = rename(merge_fname.data(), new_fname.data());
		aos_assert_rl(rslt == 0, mLock, false);
		
		mNormFname.insert(make_pair(file_seq, new_fname));	
		
		if(mShowLog)
		{
			OmnScreen << "TransServer; rename to normfile:"
				<< " old_fname:" << merge_fname
				<< "; new_fname:" << new_fname
				<< endl;
		}
	}
	mMergeFname.clear();

	mLock->unlock();
	return true;
}


bool
AosTransFile::deleteFilesToSeq(const u32 end_seq)
{
	// the end_seq will been deleted. 
	//
	map<u32, OmnString>::iterator itr = mNormFname.begin();
	for(; itr != mNormFname.end(); itr++)
	{
		u32 seqno = itr->first;
		if(seqno > end_seq)	break;

		OmnString fname = itr->second;
		int rslt = unlink(fname.data());
		aos_assert_r(rslt>=0, false);

		if(mShowLog)
		{
			OmnScreen << "TransServer; delete file:"
				<< " fname:" << fname
				<< endl;
		}
	}
	mNormFname.erase(mNormFname.begin(), itr);

	if(mActiveFile)
	{
		int act_seq = AosLogFile::getSeqnoFromFname(mActiveFile->getFileName());
		aos_assert_r(act_seq >=0, false);

		if((u32)act_seq <= end_seq)
		{
			mActiveFile = 0;	
		}
	}
	
	return true;
}


bool
AosTransFile::mergeNewerFiles()
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; merge newer files." << endl;
	}
	
	if(mNewerFname.size() == 0)	return true;
	
	mLock->lock();
	int new_seq = endSeqPriv() + 1;
	aos_assert_rl(new_seq >=0, mLock, false);

	map<u32, OmnString>::iterator itr = mNewerFname.begin();
	for(; itr != mNewerFname.end(); itr++)
	{
		OmnString merge_fname = itr->second;
	
		u32 file_seq = new_seq++;
		OmnString new_fname = mDirname;
		new_fname << "/" << mFilename << "_" << file_seq << sgNormSuffix; 
		
		int rslt = rename(merge_fname.data(), new_fname.data());
		aos_assert_rl(rslt == 0, mLock, false);
		
		mNormFname.insert(make_pair(file_seq, new_fname));	

		if(mShowLog)
		{
			OmnScreen << "TransServer; rename to normfile:"
				<< " old_fname:" << merge_fname
				<< "; new_fname:" << new_fname
				<< endl;
		}
	}
	
	mNewerFname.clear();
	mNewerFile = 0;

	// reset nextActiveSeqno
	mNextNormSeqno = new_seq;
	mActiveFile = 0;
	mLock->unlock();
	return true;
}


int
AosTransFile::staleBegSeq()
{
	mLock->lock();	
	int beg_seq = -1;
	map<u32, OmnString>::iterator itr = mStaleFname.begin();
	if(itr != mStaleFname.end())
	{
		beg_seq = itr->first;
	}

	mLock->unlock();
	return beg_seq;
}


int
AosTransFile::staleEndSeq()
{
	mLock->lock();
	int end_seq = -1;
	map<u32, OmnString>::reverse_iterator r_itr = mStaleFname.rbegin();
	if(r_itr != mStaleFname.rend())
	{
		end_seq = r_itr->first;
	}
	mLock->unlock();
	return end_seq;
}

int
AosTransFile::getNextStaleSeqno(const u32 crt_seqno)
{
	int next_seq= -1;

	mLock->lock();
	map<u32, OmnString>::iterator itr = mStaleFname.find(crt_seqno);
	aos_assert_rl(itr != mStaleFname.end(), mLock, -2);

	itr++;
	if(itr != mStaleFname.end())
	{
		next_seq= itr->first;
	}

	mLock->unlock();
	return next_seq;
}

bool
AosTransFile::startNextStale()
{
	mLock->lock();
	mStaleFile = 0;
	mLock->unlock();
	return true;             
}

bool
AosTransFile::readStaleFile( const int file_seqno, AosBuffPtr &file_buff)
{
	aos_assert_r(file_seqno>=0 , false);

	mLock->lock();
	OmnFilePtr file = openStaleFilePriv((u32)file_seqno);
	mLock->unlock();

	bool rslt = AosLogFile::readFileStatic(file, file_buff);
	aos_assert_r(rslt, false);
	return true;
}


OmnFilePtr
AosTransFile::openStaleFilePriv(const u32 seqno)
{
	map<u32, OmnString>::iterator itr = mStaleFname.find(seqno); 
	aos_assert_r(itr != mStaleFname.end(), 0);

	OmnString full_fname = itr->second;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), 0);

	return file;
}

int 
AosTransFile::begSeq()
{
	mLock->lock();	
	int beg_seq = begSeqPriv();
	mLock->unlock();
	return beg_seq;	
}

int
AosTransFile::endSeq()
{
	mLock->lock();
	int end_seq = endSeqPriv();
	mLock->unlock();
	return end_seq;
}

int
AosTransFile::begSeqPriv()
{
	map<u32, OmnString>::iterator itr = mNormFname.begin();
	if(itr == mNormFname.end())
	{
		return -1;
	}

	return itr->first;
}

int
AosTransFile::endSeqPriv()
{
	map<u32, OmnString>::reverse_iterator r_itr = mNormFname.rbegin();
	if(r_itr == mNormFname.rend()) return -1;	
	return r_itr->first;
}

int
AosTransFile::activeSeq()
{
	int act_seq = -1;
	mLock->lock();
	if(mActiveFile)
	{
		act_seq = AosLogFile::getSeqnoFromFname(mActiveFile->getFileName());
		int end_seq = endSeqPriv();
		aos_assert_r(act_seq == end_seq, -1);
	}
	mLock->unlock();
	return act_seq;	
}

	
int
AosTransFile::getNextSeqno(const u32 crt_seqno)
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mNormFname.find(crt_seqno);
	aos_assert_rl(itr !=mNormFname.end(), mLock, -2);

	itr++;
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
AosTransFile::startNextActive()
{
	mLock->lock();           
	mActiveFile = 0;         
	mLock->unlock();         
	return true;             
}


bool
AosTransFile::readNormFile( const int file_seqno, AosBuffPtr &file_buff)
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
AosTransFile::openNormFile(const u32 seqno)
{
	map<u32, OmnString>::iterator itr = mNormFname.find(seqno); 
	aos_assert_r(itr !=mNormFname.end(), 0);

	OmnString full_fname = itr->second;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), 0);

	return file;
}

