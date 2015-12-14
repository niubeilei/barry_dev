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
#include "SEUtil/LogFile.h"

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/Buff.h"

#include <dirent.h>

AosLogFile::AosLogFile(
		const OmnString &dirname, 
		const OmnString &fname, 
		const u64 max_fsize,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mNextActiveSeqno(0),
mDirname(dirname),
mFilename(fname),
mMaxFileSize(max_fsize),
mShowLog(show_log)
{
}


AosLogFile::~AosLogFile()
{
}


bool
AosLogFile::start()
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
		u32 file_seq = getSeqnoFromFname(fname);
		
		OmnString full_fname = mDirname;
		full_fname << "/" << fname;
		mNormFname.insert(make_pair(file_seq, full_fname));
	}
	
	// init nextActiveSeqno
	map<u32, OmnString>::reverse_iterator rit = mNormFname.rbegin();
	if(rit != mNormFname.rend())
	{
		mNextActiveSeqno = rit->first + 1;
	}

	return true;
}


int
AosLogFile::getNextSeqno(const u32 crt_seqno)
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

OmnFilePtr
AosLogFile::openFilePriv(const u32 seqno)
{
	//OmnFilePtr file = findFromOpenedFile(seqno);
	//if(file)	return file;

	map<u32, OmnString>::iterator itr = mNormFname.find(seqno); 
	aos_assert_r(itr !=mNormFname.end(), 0);

	OmnString full_fname = itr->second;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), 0);

	//addToOpenedFile(seqno, file);	
	return file;
}


bool
AosLogFile::readEachFile( const int file_seqno, AosBuffPtr &file_buff)
{
	aos_assert_r(file_seqno>=0 , false);

	mLock->lock();
	OmnFilePtr file = openFilePriv((u32)file_seqno);
	mLock->unlock();
	
	bool rslt = readFileStatic(file, file_buff);
	aos_assert_r(rslt, false);

	//mLock->lock();
	//deleteFromOpenedFile(file_seqno);
	//mLock->unlock();
	return true;
}


bool
AosLogFile::createActiveFilePriv()
{
	u32 file_seqno = mNextActiveSeqno++;
	OmnString full_fname = mDirname;
	full_fname << "/" << mFilename << "_" << file_seqno;

	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	aos_assert_r(file && file->isGood(), false);

	//if(mActiveFile)
	//{
	//	int act_seq = activeSeq();
	//	aos_assert_r(act_seq >=0, false);
	//	deleteFromOpenedFile(act_seq);
	//}
	mNormFname.insert(make_pair(file_seqno, full_fname));
	//addToOpenedFile(file_seqno, file);
	mActiveFile = file;

	if(mShowLog)
	{
		OmnScreen << "LogFile:: createActiveFile:"
			<< "; filname:" << file->getFileName()
			<< endl;
	}

	return false;
}


bool
AosLogFile::addData(const char *data, const u32 data_len)
{
	mLock->lock();
	if(!mActiveFile || mActiveFile->getLength() + data_len > mMaxFileSize)
	{
		createActiveFilePriv();
	}
	
	aos_assert_rl(mActiveFile, mLock, false);
	mActiveFile->lock();
	mActiveFile->append(data, data_len, true);
	mActiveFile->unlock();
	
	mLock->unlock();
	return true;	
}


int 
AosLogFile::begSeq()
{
	mLock->lock();	
	int beg_seq = begSeqPriv();
	mLock->unlock();
	return beg_seq;	
}

int
AosLogFile::endSeq()
{
	mLock->lock();
	int end_seq = endSeqPriv();
	mLock->unlock();
	return end_seq;
}

int
AosLogFile::begSeqPriv()
{
	map<u32, OmnString>::iterator itr = mNormFname.begin();
	if(itr == mNormFname.end())
	{
		return -1;
	}

	return itr->first;
}

int
AosLogFile::endSeqPriv()
{
	map<u32, OmnString>::reverse_iterator r_itr = mNormFname.rbegin();
	if(r_itr == mNormFname.rend()) return -1;	
	//OmnScreen << "tttt end file:" << r_itr->second << endl;
	return r_itr->first;
}

int
AosLogFile::activeSeq()
{
	mLock->lock();
	int act_seq = activeSeqPriv();
	mLock->unlock();
	return act_seq;	
}


int
AosLogFile::activeSeqPriv()
{
	int act_seq = -1;
	if(mActiveFile)
	{
		act_seq = getSeqnoFromFname(mActiveFile->getFileName());
		int end_seq = endSeqPriv();
		//OmnScreen << " not finish. eg. act_seq = 0, end_seq == -1" << endl;
		aos_assert_r(act_seq == end_seq, false);
	}
	return act_seq;	
}


bool
AosLogFile::startNextActive()
{
	mLock->lock();
	mActiveFile = 0;
	mLock->unlock();
	return true;
}


bool
AosLogFile::deleteFile(const u32 seqno)
{
	mLock->lock();
	map<u32, OmnString>::iterator itr = mNormFname.find(seqno);
	aos_assert_rl(itr !=mNormFname.end(), mLock, 0);
	
	OmnString fname = itr->second;
	int rslt = unlink(fname.data());
	aos_assert_rl(rslt>=0, mLock, false);
	
	mNormFname.erase(itr);
	
	if(mActiveFile)
	{
		int act_seq = activeSeqPriv();
		aos_assert_r(act_seq >=0, false);
		if(seqno == (u32)act_seq) 	mActiveFile = 0;
	}
	
	mLock->unlock();

	if(mShowLog)
	{
		OmnScreen << "LogFile:: delete file:"
			<< " fname:" << fname
			<< endl;
	}
	return true;
}


bool
AosLogFile::deleteAllFiles()
{
	mLock->lock();
	u32 from_seq = begSeqPriv();
	bool rslt = deleteFilesPriv(from_seq, -1);
	mLock->unlock();
	
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLogFile::deleteFilesPriv(const u32 from_seq, const int end_seq)
{
	// if end_seq != -1, the end_seq will delete also.
	//
	
	map<u32, OmnString>::iterator beg_itr = mNormFname.find(from_seq);
	if(beg_itr == mNormFname.end())		return true;

	map<u32, OmnString>::iterator itr;
	u32 seqno = 0;
	for(itr = beg_itr; itr != mNormFname.end(); itr++)
	{
		seqno = itr->first;
		if( end_seq != -1 && seqno > (u32)end_seq)	break;

		OmnString fname = itr->second;
		int rslt = unlink(fname.data());
		aos_assert_rl(rslt>=0, mLock, false);

		if(mShowLog)
		{
			OmnScreen << "LogFile:: delete file:"
				<< " fname:" << fname
				<< endl;
		}
	}

	if(end_seq != -1 && seqno <= (u32)end_seq)
	{
		OmnScreen << "some files not deleted; "
			<< seqno << "--" << end_seq
			<< endl; 
	}
	mNormFname.erase(beg_itr, itr);
	
	if(mActiveFile)
	{
		int act_seq = activeSeqPriv();
		aos_assert_r(act_seq >=0, false);
		
		if(end_seq == -1 && (u32)act_seq >= from_seq)
		{
		}
		if(end_seq != -1 && (u32)act_seq >= from_seq && (u32)act_seq <= (u32)end_seq)
		{
			mActiveFile = 0;	
		}
	}
	
	return true;
}


//=--------------------------
bool
AosLogFile::readFileStatic(const OmnFilePtr &file, AosBuffPtr &file_buff)
{
	aos_assert_r(file, false);
	file->lock();
	u32 read_size = file->getLength();                                
	file_buff = OmnNew AosBuff(read_size, 0 AosMemoryCheckerArgs);
	int bytesread = file->readToBuff(0, read_size, file_buff->data());
	file->unlock();

	OmnScreen << "LogFile:: read log file:"
		<< "; filname:" << file->getFileName()
		<< "; read_size:" << read_size
		<< endl;

	//aos_assert_r(bytesread >0 && (u32)bytesread == read_size, false); 
	aos_assert_r((u32)bytesread == read_size, false); 
	file_buff->setDataLen(read_size);   
	return true;
}

u32
AosLogFile::getSeqnoFromFname(const OmnString &fname)
{
	int loc = fname.find('_', true);	
	u32 seqno = atoi(fname.substr(loc+1).data());
	return seqno;
}


OmnString
AosLogFile::getFnameSuffix(const OmnString &fname)
{
	int loc = fname.find('.', true);
	if(loc == -1)	return "";

	//OmnString suffix = fname.substr(loc+1);
	OmnString suffix = fname.substr(loc);
	return suffix;	
}

/*
void
AosLogFile::addToOpenedFile(const u32 seqno, const OmnFilePtr &file)
{
	OpenedFile o_file(file);
	mOpenedFile.insert(make_pair(seqno, o_file));
}


OmnFilePtr
AosLogFile::findFromOpenedFile(const u32 seqno)
{
	map<u32, OpenedFile>::iterator itr = mOpenedFile.find(seqno);
	if(itr != mOpenedFile.end())
	{
		return itr->second.mFile;
	}
	return 0;
}


void 
AosLogFile::deleteFromOpenedFile(const u32 seqno)
{
	map<u32, OpenedFile>::iterator itr = mOpenedFile.find(seqno);
	if(itr != mOpenedFile.end())
	{
		OpenedFile o_file = itr->second; 
		aos_assert(o_file.mRefCount !=0);   
		o_file.mRefCount--;
		if(o_file.mRefCount == 0)
		{
			mOpenedFile.erase(itr);
		}
	}
}

AosLogFile::iterator
AosLogFile::begin()
{
	iterator itr(begSeq(), 0);	
	return itr;
}


AosLogFile::iterator
AosLogFile::end()
{
	iterator itr;
	return itr;	
}

bool
AosLogFile::readData(
		const iterator &start,
		const u32 total_size,
		AosBuffPtr &buff)
{
	aos_assert_r(start.mSeqno >=0 , false);
	buff = OmnNew AosBuff(total_size, 0 AosMemoryCheckerArgs);
	
	u32 f_seq = (u32)start.mSeqno;
	u64 f_off = start.mOffset;
	u32 crt_read_size = 0;

	while(crt_read_size < total_size)
	{
		mLock->lock();
		OmnFilePtr file = openFilePriv(f_seq);
		mLock->unlock();
		aos_assert_r(file, false);

		u32 remain = total_size - crt_read_size;
		file->lock();
		char * data = buff->data() + buff->getCrtIdx(); 
		int bytesread = file->readToBuff(f_off, remain, data);
		file->unlock();

		if(bytesread <0 || (u32)bytesread < remain)
		{
			int next_seq = getNextSeqno(start.mSeqno);
			aos_assert_r(next_seq >=-1, false);
			if(next_seq == -1)	return true;		// means finish.
			
			aos_assert_r(next_seq >0, false);
			f_seq = (u32)next_seq;
			f_off = 0;
		}

		crt_read_size += bytesread;
	}
	return true;
}

*/

