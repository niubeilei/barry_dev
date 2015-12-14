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
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/FileWithCache.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/FileOpenErr.h"
#include "Porting/GetErrno.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/FileWatcher.h"
#include "util_c/strutil.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// This constant is used to limit the maximum length of the buffer
// if one wants to read the entire file into memory. Currently, it is
// set to 100M. 
const int sgMaxBufLength = 100000000;

AosFileWithCache::AosFileWithCache()
:
mFile(0),
mMode(eInvalid),
mLock(OmnNew OmnMutex()),
mBlocksToCache(0),
mCachedBlockSize(0)
{
}


AosFileWithCache::AosFileWithCache(const OmnString &name, const Mode mode)
:
mName(name),
mFile(0),
mMode(mode),
mFileDesc(name),
mLock(OmnNew OmnMutex()),
mBlocksToCache(0),
mCachedBlockSize(0)
{

	mFile = openFile(mName, mode);
}


AosFileWithCache::~AosFileWithCache()
{
	if (mFile)
	{
		::fclose(mFile);
		mFile = 0;
	}
}


bool
AosFileWithCache::openFile(const Mode mode)
{
	mFile = openFile(mName, mode);
	return (mFile != 0);
}


FILE *
AosFileWithCache::openFile(const OmnString &filename, const Mode mode)
{
	FILE *ff = 0;
	switch (mode)
	{
	case eReadOnly:
		 ff = ::fopen64(filename, "rb");
		 break;
		 
	case eAppend:
		 ff = ::fopen64(filename, "ab");
		 break;

	case eCreate:
		 ff = ::fopen64(filename, "w+b");
		 break;
		 
	case eReadWrite:
		 ff = ::fopen64(filename, "r+b");
		 break;

	case eWriteCreate:
		 ff = ::fopen64(filename, "r+b");
		 if (!ff)
		 {
			 ff = ::fopen64(filename, "w+b");
		 }
		 break;

	default:
		 OmnWarnProgError << "Unrecognized file mode: " << mode 
			<< ". File name = " << filename << enderr;
		 return 0;
	}

	return ff;
}


bool
AosFileWithCache::put(
		const u64 &offset, 
		const char *str, 
		const int len, 
		const bool toFlush)
{
	if (!str || len <= 0) return true;

	if (offset != eNoSeek) aos_assert_r(seek(offset), false);
	if (mFile)
	{
		//
		// Chen Ding, 10/15/2004
		//
		// According to the manual, 'fputs()' shall return the number of bytes
		// written to the file. But it seems that Microsoft always returns 0.
		// For this reason, we have to change the following not to include 0.
		//
		//	if (::fputs(str, mFile) <= 0)
		// Chen Ding, 2009/10/08
		// Changed it to fwrite since we need to control how many bytes to 
		// write. 
		// if (::fputs(str, mFile) < 0)
		AOSFILE_COUNTING_WRITE;
		int nn = ::fwrite(str, 1, len, mFile);
		if (nn != len)
		{
			// 
			// The caller should pring a warning, not this
			// function in case this file
			// is used for logs, we don't want to print too
			// many alarms.
			//				Chen Ding, 12-07-2002
			return false;
		}
		if (toFlush)
		{
			AOSFILE_COUNTING_FLUSH;
			OmnFlushFile(mFile);
		}

		return true;
	}

	return false;
}


u64
AosFileWithCache::readU64(const u64 &offset, const u64 &dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	char *endptr;
	return ::strtoull(word, &endptr, 10);
}


int 
AosFileWithCache::readBinaryInt(const u64 &offset, const int dft)
{
	aos_assert_r(seek(offset), dft);
	int value;
	AOSFILE_COUNTING_READ;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


int64_t 
AosFileWithCache::readBinaryInt64(const u64 &offset, const int64_t &dft)
{
	aos_assert_r(seek(offset), dft);
	int64_t value;
	AOSFILE_COUNTING_READ;
	int bytesread = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (bytesread != sizeof(value)) return dft;
	return value;
}


u32
AosFileWithCache::readBinaryU32(const u64 &offset, const u32 dft)
{
	aos_assert_r(seek(offset), dft);
	u32 value;
	AOSFILE_COUNTING_READ;
	int rr = ::fread((void *)&value, 1, sizeof(value), mFile);
	if (rr != sizeof(value)) return dft;
	return value;
}


u64
AosFileWithCache::readBinaryU64(const u64 &offset, const u64 &dft)
{
	aos_assert_r(seek(offset), dft);
	u64 value;
	AOSFILE_COUNTING_READ;
	aos_assert_r(::fread((void *)&value, 1, 
		sizeof(value), mFile) == sizeof(value), false);
	return value;
}


int 
AosFileWithCache::readInt(const u64 &offset, const int dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	return ::atoi(word);
}


u32
AosFileWithCache::readU32(const u64 &offset, const u32 dft)
{
	aos_assert_r(seek(offset), dft);
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return dft;
	char *endptr;
	u32 vv = strtoul(word, &endptr, 10);
	return vv;
}


bool
AosFileWithCache::readInt(int &v)
{
	char word[100];
	int length = getWord(word, 100);
	if (length <= 0) return false;
	v = ::atoi(word);
	return true;
}


bool
AosFileWithCache::closeFile()
{
	aos_assert_r(mFile, false);
	bool rt = closeFile(mFile);
	mFile = 0;
	return rt;
}


bool
AosFileWithCache::closeFile(FILE *f)
{
	if (!f)
	{
		return true;
	}

	::fclose(f);
	f = 0;
	return true;
}


bool
AosFileWithCache::resetFile()
{
	// 
	// This function erases all the contents. It assumes the 
	// file must be valid. Otherwise, it does nothing.
	//
	if (!closeFile())
	{
		return false;
	}

	mFile = openFile(mName, eCreate);

	return true;
}


int			
AosFileWithCache::readToBuff(
		const u64 &startPos,
		const u32 size,
		char *buff)
{
	// 
	// This function reads the contents from the file starting from the position: 
	// 'startPos' and reads maximum 'size' number of bytes. If the file has more than
	// 'size' bytes from 'startPos', only 'size' number of bytes are read. The caller
	// is responsible for allocating sufficient memory in 'buff'.
	//
	// If successful, it returns the number of bytes read. Otherwise, it returns
	// -1. 
	// 
	if (!mFile)
	{
		OmnAlarm << "File is null: " << mName << enderr;
		return -1;
	}

	//
	// Get the file size and check the size parameters
	//
	AOSFILE_COUNTING_SEEK;
	::fseeko64(mFile,0,SEEK_END);
	int64_t fsize = ::ftello64(mFile);
	if ((u64)fsize <= startPos) 
	{
		return -1;
	}

	int64_t asize = size;
	if ((u64)fsize < startPos + size)
	{
		asize = fsize - startPos;
	}

	// ::fseek(mFile, startPos.toInt(),SEEK_SET);
	int rr;
	AOSFILE_COUNTING_SEEK;
	rr = ::fseeko64(mFile, startPos, SEEK_SET);
	AOSFILE_COUNTING_READ;
	rr = ::fread(buff, 1, asize, mFile);
	return rr;
}


bool
AosFileWithCache::append(const OmnString &buff, const bool toFlush)
{
	// 
	// It appends the contents 'buff' to the file. If failed, it returns false.
	// Otherwise, it returns true.
	//
	// 
	// Write to the file. 
	//
	aos_assert_r(mFile, false);
	if (buff.length() <= 0)
	{
		return false;
	}

	// int asize = ::fwrite(buff, 1, buff.length(), mFile) ;
	AOSFILE_COUNTING_WRITE;
	int asize = ::fwrite(buff.data(), 1, buff.length(), mFile) ;
	if (asize < 0) 
	{
		return false;
	}

	if (toFlush)
	{
		AOSFILE_COUNTING_FLUSH;
		OmnFlushFile(mFile);
	}

	return true;
}


bool
AosFileWithCache::flushFileContents()
{
	OmnFlushFile(mFile);
	return true;
}


bool
AosFileWithCache::seek(const u64 &pos, AosFileCacheBlockPtr &block)
{
	block = 0;
	if (!mFile)
	{
		OmnAlarm << "File null: " << mName << enderr;
		return false;
	}
	AOSFILE_COUNTING_REWIND;

	if (mBlocksToCache <= 0)
	{
		return (::fseeko64(mFile, pos, SEEK_SET) == 0);
	}

	// Check whether it is cached.
	u64 idx = (pos / mCachedBlockSize) * mCachedBlockSize;
	CacheMapItr_t itr = mCache.find(idx);
	if (itr == mCache.end()) 
	{
		return (::fseeko64(mFile, pos, SEEK_SET) == 0);
	}

	block = itr->second;
	return true;
}


bool
AosFileWithCache::openFile1(const OmnString &fname, const Mode mode)
{
	mName = fname;
	mMode = mode;
	mFile = openFile(mName, mMode);
	return isGood();
}
	

u64
AosFileWithCache::getFileCrtSize()
{
	aos_assert_r(mFile, 0);
	AOSFILE_COUNTING_SEEK;
	::fseeko64(mFile, 0, SEEK_END);
	return ::ftello64(mFile);
}


bool		
AosFileWithCache::append(const char *buff, const int len, const bool toFlush)
{
	aos_assert_r(mFile, false);
	if (len <= 0) return false;

	AOSFILE_COUNTING_WRITE;
	int asize = ::fwrite(buff, 1, len, mFile) ;
	if (asize < 0) return false;
	if (toFlush) 
	{
		AOSFILE_COUNTING_FLUSH;
		OmnFlushFile(mFile);
	}
	return true;
}


bool	
AosFileWithCache::writeToFile(
		const OmnString &fname, 
		const u64 &offset, 
		const u32 size, 
		char *data)
{
	AosFileWithCache ff(fname, eReadWrite);
	aos_assert_r(ff.isGood(), false);
	// aos_assert_r(::fseek(ff.mFile, offset, SEEK_SET) == 0, false);
	aos_assert_r(ff.put(offset, data, (int)size, true), false);
	ff.closeFile();
	return true;
}


bool
AosFileWithCache::setInt(const u64 &offset, 
		const int value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}



bool
AosFileWithCache::setU32(const u64 &offset, 
		const u32 value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}



bool
AosFileWithCache::setU64(const u64 &offset, 
		const u64 &value, 
		const bool flushflag)
{
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, (char *)&value, sizeof(value), flushflag), false);
	return true;
}


bool
AosFileWithCache::setStr(const u64 &offset, 
		const char *value, 
		const int len,
		const bool flushflag)
{
	if (len <= 0) return true;
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, value, len, flushflag), false);
	return true;
}


bool
AosFileWithCache::setStr(const u64 &offset, 
		const OmnString &value, 
		const bool flushflag)
{
	if (value.length() == 0) return true;
	// aos_assert_r(seek(offset), false);
	aos_assert_r(put(offset, value.data(), value.length(), flushflag), false);
	return true;
}


OmnString	
AosFileWithCache::readStr(
		const u64 &offset, 
		const u32 len, 
		const OmnString &dft)
{
	OmnString str(len+1, ' ', true);
	char *data = (char *)str.data();
	u32 bytesRead = readToBuff(offset, len, data);
	if (bytesRead != len) return dft;
	return str;
}


bool
AosFileWithCache::forceFlush()
{
	AOSFILE_COUNTING_FORCEFLUSH;
	aos_assert_r(mFile, false);
	::fclose(mFile);
	mFile = ::fopen64(mName, "r+b");
	return true;
}

#endif
