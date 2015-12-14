////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_File_h
#define Snt_Util_File_h

#define _FILE_OFFSET_BITS 64


#include "aosUtil/Types.h"
#include "Porting/File.h"
#include "Porting/LongTypes.h"
#include "Rundata/Rundata.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/FileDesc.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/KeyedNode.h"
#include "Util/LocalFile.h"
#include "UtilComm/Ptrs.h"
#include <list>

#define AOSFILEMODE_READWRITE			"readwrite"
#define AOSFILEMODE_CREATE				"create"
#define AOSFILEMODE_READONLY			"readonly"
#define AOSFILEMODE_APPEND				"append"

class OmnFile : public OmnLocalFile, public AosMemoryCheckerObj  
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftFileReadSize = 50000000,		// 50M
		eNoSeek = 0xffffffff
	};

	enum Mode
	{
		eInvalid,
		eReadOnly,
		eAppend,
		eCreate,
		eReadWrite,
		eWriteCreate,
		eReadWriteCreate
	};

private:
	OmnString		mName;
	FILE *			mFile;
	Mode			mMode;
	OmnFileDesc		mFileDesc;
	OmnString		mFileContents;
	OmnMutexPtr		mLock;
	int				mBlocksToCache;
	int				mCachedBlockSize;
	int64_t         mDeviceId;
	OmnString 		mError;
	
	static int64_t 	smDftReadSize;

public:
	OmnFile(AosMemoryCheckDeclBegin);
	OmnFile(const OmnString &name, const Mode mode AosMemoryCheckDecl);
	virtual ~OmnFile();

	static FILE * 	openFile(const OmnString &filename, const Mode mode);
	static bool		closeFile(FILE *f);

	// Ketty 2014/02/21
	bool		closeFile();

// Ken Lee, 2013/05/29
private:
	void		exceptionThrow(
					const OmnString &file_name, 
					const int line,
					const int t_errno);

public:
	bool		openFile(const Mode mode);
	void 		resetFile();
	void		lock() {mLock->lock();}
	void		unlock() {mLock->unlock();}

	// Chen Ding, 02/19/2007
	static bool	fileExist(const OmnString &filename);
	static Mode	convertMode(const OmnString &str);

	bool		isGood() const {return mFile != 0;}
	OmnString	getFileName() const {return mName;}

	bool		put(const u64 &, const char *str, const int len, const bool flush);
	OmnFile &	operator << (const char *str);

	int64_t		readBinaryInt64(const u64 &offset, const int64_t &dft);
	int			readBinaryInt(const u64 &offset, const int dft);
	u32			readBinaryU32(const u64 &offset, const u32 dft);
	u64			readBinaryU64(const u64 &offset, const u64 &dft);
	
	// ken Lee, 2011/6/17
	char		readChar(const u64 &offset, const char &dft);

	bool		readInt(int &v);
	int			readInt(const u64 &offset, const int dft);
	u32			readU32(const u64 &offset, const u32 dft);
	u64			readU64(const u64 &offset, const u64 &dft);
	OmnString	readStr(const u64 &offset, const u32, const OmnString &);
	int			getWord(char *word, const int length);
	OmnString	getLine(bool &finished);
	bool		readToString(OmnString &buf);
	int64_t		readToBuff(const u64 &startPos,
						   const u32 size,
						   char *buff);
	bool		append(const char *buff, const int64_t &len, const bool flush = false);
	bool		append(const OmnString &buff, const bool flush = false);
	bool appendWithLen(const OmnString &buff, const bool flush = false)
	{
		OmnString len;
		len << buff.length() << "\n";
		append(len, false);
		return append(buff, flush);
	}
	void		setName(const OmnString &name) {mName = name;}
	bool		removeLine(const u32 pos);
	bool		skipChar();
	bool		skipTo(const char *pattern, const bool after);
	bool		readUntil(OmnString &buffer, 
					const char *pattern, 
					const u64 &max, 
					const bool flag, 
					bool &finished);
	bool		readBlock(const OmnString &start, 
					const OmnString &end, 
					OmnString &contents, 
					bool &err);
	bool		readAll(OmnString &contents);
	FILE*		getFile() {return mFile;}
	bool		replace(const OmnString &patten, 
					const OmnString &contents, 
					const bool allFlag);
	bool 		replace(const u64 &startPos, 
					const u32 len, 
					const OmnString &contents);
	bool		replace(const OmnString &startPattern, 
					const OmnString &endPattern, 
					const OmnString &contents);
	bool		flushFileContents();
	void		clearFileContents() {mFileContents = "";}
	int			find(const OmnString &str);
	static int 	getFiles(const OmnString &dir, 
					std::list<OmnString> &names, 
					const bool recursive);
	int 		getFilesMulti(const OmnString &str, 
					std::list<OmnString> &names, 
					const OmnString &sep,
					const bool recursive);
	bool		seek(const u64 &pos);
	bool		setContents(const char *data, const int len, const bool flag);
	bool		openFile1(const OmnString &fname, const Mode mode);
	u64			getFileCrtSize();
	static bool	writeToFile(const OmnString &fname, 
					const u64 &offset, 
					const u32 size, 
					char *data);

	// ken Lee, 2011/6/17
	bool		setChar(const u64 &offset, 
					const char value, 
					const bool flushflag);
	
	bool		setInt(const u64 &offset, 
					const int value, 
					const bool flushflag);
	bool		setU32(const u64 &offset, 
					const u32 value, 
					const bool flushflag);
	bool		setU64(const u64 &offset, 
					const u64 &value, 
					const bool flushflag);
	bool		setStr(const u64 &offset, 
					const char *value, 
					const int len,
					const bool flushflag);
	bool		setStr(const u64 &offset, 
					const OmnString &value, 
					const bool flushflag);
	bool 		forceFlush();
	bool	verifyFile(const OmnFilePtr &file);

	inline u64 getLength()
	{
		return getFileCrtSize();
	}

	/*
	u32 getLastModifyTime()
	{
		return OmnGetFileLastModifyTime(mName.data());	
	}
	*/
	
	void deleteFile()
	{
		closeFile();
		::remove(mName.data());
	}

	static u32 getLastModifyTime(const OmnString &fname)
	{
		return OmnGetFileLastModifyTime(fname.data());
	}
	
	static u64 getFileLengthStatic(const OmnString &fname)
	{
		return OmnGetFileLength(fname.data());
	}

	// Chen Ding, 11/07/2011
	void setCaching(const int num_blocks, const int blockSize);

	void goToFileEnd()
	{
		aos_assert(mFile);
		::fseek(mFile, 0, SEEK_END);
	}

	static inline bool isGoodStatic(const OmnString &fname)
	{
		OmnFile f(fname, eReadOnly AosMemoryCheckerArgs);
		return f.isGood();
	}

	static inline int64_t getDftReadSize() {return smDftReadSize;}

	int64_t read(                      
		std::vector<AosBuffPtr> &buffs,
		const int64_t offset, 
		const int64_t len, 
		AosRundata *rdata);

	// Chen Ding, 2013/02/05
	bool readToBuff(AosBuffPtr &buff, const u64 &max_read_size);

	int64_t getDeviceId()
	{
		//http://blog.csdn.net/gdujian0119/article/details/6363574
		if (mDeviceId >= 0) return mDeviceId;
		struct stat buf;
		//ice, 2013/12/13
		int r = stat(mName.data(), &buf);
		if (r == 0)
			mDeviceId = buf.st_dev;
		else
			mDeviceId = r;
		return mDeviceId;
	}

	static int64_t getDeviceId(const OmnString &fname)
	{
		struct stat buf;
		stat(fname.data(), &buf);
		int64_t dev_id = buf.st_dev;
		return dev_id;
	}

	bool    write(
			const int64_t &startPos,
			const char *buff,           
			const int64_t &len,
			const bool flush = false)
	{
	 	return put(startPos, buff, len, flush);
	}

	int64_t read(
			const int64_t &startPos, 
			const char *buff,
			const int64_t &len)
	{
		return readToBuff(startPos, len, (char*)buff);
	}

	void deleteFile(AosRundata *rdata)
	{
		deleteFile();
	}

	void readlock()
	{
		mLock->lock();
	}

	void writelock()
	{
		mLock->lock();
	}

	// Chen Ding, 2013/05/18
	u64 getCurrentPos() 
	{
		aos_assert_r(mFile, 0);
		return ftell(mFile);
	}

};
#endif

