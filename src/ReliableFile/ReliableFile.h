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
// 08/14/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ReliableFile_ReliableFile_h
#define AOS_ReliableFile_ReliableFile_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RlbFileType.h"
#include "StorageMgrUtil/FileInfo.h"
#include "Util/File.h"
#include "Util/LocalFile.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosReliableFile: public OmnLocalFile
{

protected:
	OmnMutexPtr			mLock;
	u64					mFileId;
	AosStorageFileInfo	mFileInfo;		
	OmnFilePtr			mLocFile;
	AosRlbFileType::E	mType;
	bool				mIsNewFile;
	bool				mUseGlobalFmt;

	static AosReliableFilePtr smReliableFiles[AosRlbFileType::eMax];
	
public:
	AosReliableFile(const AosRlbFileType::E	type);
	AosReliableFile(
			const AosRlbFileType::E type, 
			const AosStorageFileInfo &file_info,
			const bool create_flag,
			AosRundata *rdata);
	~AosReliableFile();

	OmnString getFileName() const
	{
		return mLocFile->getFileName();
	}
	int64_t getDeviceId()
	{
		return mLocFile->getDeviceId();
	}
	bool 	isGood() const;
	bool    write(
			const int64_t &startPos,
			const char *buff,           
			const int64_t &len,
			const bool flush = false)
	{
		return mLocFile->write(startPos, buff, len, flush);
	}
	int64_t read(
		const int64_t &startPos, 
		const char *buff,
		const int64_t &len)
	{
		return mLocFile->read(startPos, buff, len);
	}
	
	bool append(
			const char *buff,
			const int64_t &len,
			const bool flush = false)
	{
		return mLocFile->append(buff, len, flush);		
	}
	
	// Ketty 2014/03/05	
	bool seek(const u64 &pos){ return mLocFile->seek(pos); };
	
	// Ketty 2014/03/05	
	bool append(
		AosRundata *rdata,
		const u64 offset,
		const char *buff,
		const int64_t &len,
		const bool flush = false);
	void    unlock()
	{
		mLocFile->unlock();
	}
	void    readlock()
	{
		mLocFile->lock();
	}
	void    writelock()
	{
		mLocFile->lock();
	}
	void 	resetFile()
	{
		mLocFile->resetFile();
	}
	void deleteFile(AosRundata *rdata)
	{
		mLocFile->deleteFile();
	}

	u64		getFileId() const {return mFileId;};
	AosRlbFileType::E getRlbFileType() const {return mType;}
	bool	isNewFile(){ return mIsNewFile; };

	static bool		init();

	static AosReliableFilePtr 	getReliableFile(const u64 &file_id, 
										const AosRlbFileType::E type, 
										const bool create_flag, 
										AosRundata *rdata);
	static AosReliableFilePtr 	getReliableFile(
										const AosStorageFileInfo &file_info,
										const AosRlbFileType::E type, 
										const bool create_flag, 
										AosRundata *rdata);

	virtual AosReliableFilePtr clone(
							const AosStorageFileInfo &file_info, 
							const bool create_flag, 
							AosRundata *rdata) = 0;

	virtual bool	modifyFile(const u64 offset, 
							const char *data, 
							const int len, 
							const bool flushfalg, 
							bool &file_damaged,		// Ketty 2012/10/08
							AosRundata *rdata) = 0;

	bool       recover(AosRundata *rdata);

	u64			getFileCrtSize() const;
	u64			getLength() {return getFileCrtSize();}
	OmnString	getFileName() {return mLocFile->getFileName();}
	bool		flushFileContents();
	// Ketty 2014/02/21
	bool 		closeFile();


	bool setU8(const u64 offset, 
			const u8 value, 
			const bool flushflag, 
			AosRundata *rdata)
	{
		bool file_damaged = false;
		return modifyFile(offset, (const char *)&value, sizeof(u8), 
				flushflag, file_damaged, rdata);
	}

	bool setInt(const u64 offset, 
				const int value, 
				const bool flushflag, 
				AosRundata *rdata)
	{
		bool file_damaged = false;
		return modifyFile(offset, (const char *)&value, sizeof(int), 
				flushflag, file_damaged, rdata);
	}

	bool setU32(const u64 offset, 
				const u32 value, 
				const bool flushflag,
				AosRundata *rdata)
	{
		bool file_damaged = false;
		return modifyFile(offset, (const char *)&value, sizeof(u32), 
				flushflag, file_damaged, rdata);
	}

	bool setU64(const u64 offset, 
				const u64 value, 
				const bool flushflag, 
				AosRundata *rdata)
	{
		bool file_damaged = false;
		return modifyFile(offset, (const char *)&value, sizeof(u64), 
				flushflag,file_damaged,  rdata);
	}

	bool put(const u64 offset, 
				const char *data, 
				const int len, 
				const bool flushflag, 
				AosRundata *rdata)
	{
		bool file_damaged = false;
		return modifyFile(offset, data, len, flushflag, file_damaged, rdata);
	}

	int	readInt(const u64 &offset, const int dft, AosRundata *rdata)
	{
		char data[10];
		bool rslt = readFile(offset, sizeof(int), data, rdata);
		aos_assert_r(rslt, dft);

		// Chen Ding, 2013/05/03
		// int vv = *(int *)data;
		int *dd = (int*)data;
		return *dd;
	}

	u32	readU32(const u64 &offset, const u32 dft, AosRundata *rdata)
	{
		char data[10];
		bool rslt = readFile(offset, sizeof(u32), data, rdata);
		aos_assert_r(rslt, dft);

		// Chen Ding, 2013/05/03
		// u32 vv = *(u32 *)data;
		u32 *dd = (u32*)data;
		return *dd;
	}

	u64	readU64(const u64 &offset, const u64 dft, AosRundata *rdata)
	{
		char data[30];
		bool rslt = readFile(offset, sizeof(u64), data, rdata);
		aos_assert_r(rslt, dft);

		// Chen Ding, 2013/05/03
		// u64 vv = *(u64 *)data;
		u64 *dd = (u64*)data;
		return *dd;
	}

	OmnString readStr(
			const u64 &offset, 
			const u32 len, 
			const OmnString &dft, 
			AosRundata *rdata)
	{
		OmnString str(len+1, ' ', true);
		char *data = (char *)str.data();
		bool rslt = readToBuff(offset, len, data, rdata);
		aos_assert_r(rslt, dft);
		return str;
	}

	bool readToBuff(
			const u64 offset, 
			const u32 size, 
			char *buff, 
			AosRundata *rdata)
	{
		return readFile(offset, size, buff, rdata);
	}

	bool removeFile(AosRundata *rdata);

	void setUseGlobalFmt(const bool b){mUseGlobalFmt = b;}

protected:
	bool 	modifyLocalFile(
					const u64 &offset, 
					const char *data, 
					const int len, 
					const bool flush, 
					AosRundata *rdata);

	bool    readFile(const u64 offset, 
			        const u32 size,
					char *buff, 
					AosRundata *rdata);

	void	addCreateFileFmt(AosRundata *rdata);
	virtual OmnString getFullFilename();

private:
	bool	openFile(const bool create_flag, AosRundata *rdata);
	bool 	sendFmt(
				AosRundata *rdata,
				const u64 offset, 
				const char *buff,
				const int64_t &len,
				const bool flush);
};
#endif
