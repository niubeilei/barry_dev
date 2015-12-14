////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_VirtualFile_VirtualFile_h
#define AOS_VirtualFile_VirtualFile_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageMgr/NetFileMgr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"

class AosVirtualFile : public AosVirtualFileObj, public AosAioCaller 
{
	OmnDefineRCObject;

protected:
	AosBuffPtr 		mBuff;
	int				mServerId;
	int64_t			mStartOffset;
	int64_t			mEndOffset;
	int64_t			mCrtOffset;
	int64_t			mFileLength;

	AosFileReadListenerPtr	mCaller; 

public:
	AosVirtualFile()
	:
	mBuff(0),
	mServerId(-1),
	mStartOffset(0),
	mEndOffset(0),
	mCrtOffset(0),
	mFileLength(-1)
	{
	}

	AosVirtualFile(const int server_id)
	:
	mBuff(0),
	mServerId(server_id),
	mStartOffset(0),
	mEndOffset(0),
	mCrtOffset(0),
	mFileLength(-1)
	{
	}

	~AosVirtualFile(){}
/*
	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata)
	{
		int64_t offset = mCrtOffset;
		mCrtOffset += blocksize;
		AosAioCallerPtr thisptr(this, false);
		AosNetFileMgr::getSelf()->asyncReadFile(mFileId, mServerId, offset, 
				blocksize, reqId, thisptr, rdata); 
		return true;
	}
*/
	virtual AosBuffPtr getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl)
	{
		AosBuffPtr buff  = OmnNew AosBuff(size AosMemoryCheckerFileLine);	
		return buff;
	}

	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff) 
	{
		mBuff = buff;
		aos_assert(mCaller);
		AosDiskStat disk_stat;
/*
OmnScreen << "====================VirtualFile dataRead reqId : " << parm.reqId 
		  << " expected_size : " << parm.expected_size 
		  << " finished : " << parm.finished << endl;
OmnScreen << "======================================================" << endl;
*/
	    aos_assert(parm.offset != -1);
		int64_t offset = parm.offset+parm.expected_size;

		aos_assert(offset <= mEndOffset);
		bool finished = parm.finished;
		if(offset == mEndOffset)
		{
/*
OmnScreen << "====================VirtualFile dataRead reqId : " << parm.reqId << endl;
OmnScreen << " mCrtOffset : " << offset << endl;
OmnScreen << " mEndOffset : " << mEndOffset << endl;
OmnScreen << "======================================================" << endl;
*/
			finished = true;	
		}
		
		mCaller->fileReadCallBack(parm.reqId, parm.expected_size, finished, disk_stat);
	//	if (parm.finished) mCaller = 0;
	}

	virtual void readError(u64 &reqid, OmnString &errmsg)
	{
		AosDiskStat disk_stat;
		disk_stat.setError(AosDiskStat::eReadErr);
		mCaller->fileReadCallBack(reqid, 0, true, disk_stat);
	}

	virtual void dataWrite(const u64 &reqid)
	{
	}

	virtual void writeError(u64 &reqid, OmnString &errmsg)
	{
	}

	virtual void setCaller(const AosFileReadListenerPtr &caller)
	{
		mCaller = caller;
	}

	virtual AosFileReadListenerPtr getCaller()
	{
		return mCaller;
	}

	virtual void seek(const int64_t start_pos)
	{
		mCrtOffset = start_pos;		
	}

	AosBuffPtr getBuff()
	{
		AosBuffPtr buff = mBuff;
		mBuff = 0;
		return buff;
	}
};


class AosIdVirtualFile : public AosVirtualFile 
{

private:	
	u64				mFileId;

public:
	AosIdVirtualFile(
			const int server_id,
			const u64 &fileid)
	:
	AosVirtualFile(server_id),
	mFileId(fileid)
	{
	}
	
	AosIdVirtualFile(const AosFileInfo &fileinfo)
	:
	AosVirtualFile(fileinfo.mPhysicalId),
	mFileId(fileinfo.mFileId)
	{
		mStartOffset = fileinfo.mStartOffset;
		mEndOffset = fileinfo.mStartOffset + fileinfo.mFileLen;
		mCrtOffset = fileinfo.mStartOffset;
		mFileLength = fileinfo.mFileLen;
	}
	
	~AosIdVirtualFile(){}
	
	virtual int64_t length(AosRundata *rdata)
	{
		if (mFileLength > 0) return mFileLength;
		AosDiskStat disk_stat;
		bool rslt = AosNetFileMgr::getSelf()->getFileLengthById(
			mFileLength, mFileId, mServerId, disk_stat, rdata);
		aos_assert_r(rslt, -1);
		mEndOffset = mStartOffset + mFileLength;
/*
OmnScreen << "===============================  IdVirtualFile length " << endl;
OmnScreen << " mStartOffset : " << mStartOffset << endl;
OmnScreen << " mFileLength : " << mFileLength << endl;
OmnScreen << " mEndOffset : " << mEndOffset << endl;
OmnScreen << "======================================================" << endl;
*/
		return mFileLength;
	}


	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize, 
			AosRundata *rdata)
	{
		if(mFileLength < 0) length(rdata);

		int64_t offset = mCrtOffset;
		int64_t bsize = blocksize;

		if (offset + bsize > mEndOffset) 
		{
			bsize = mEndOffset - offset;
		}
		aos_assert_r(bsize >= 0, false);
		mCrtOffset += bsize;
/*
OmnScreen << "===============================  IdVirtualFile readdata " 
			<< " reqId : " << reqId 
			<< " mFileId : " << mFileId
			<< " mCrtOffset : " << offset 
			<< " mStartOffset : " << mStartOffset 
			<< " mEndOffset : " << mEndOffset
			<< " bsize : " << bsize 
			<< " blocksize : " << blocksize
			<< " mServerId : " << mServerId 
			<< "========================================================" 
			<< endl;
*/
		AosAioCallerPtr thisptr(this, false);
		return AosNetFileMgr::getSelf()->asyncReadFile(mFileId,
			mServerId, offset, bsize, reqId, thisptr, rdata); 
	}
};


class AosNameVirtualFile : public AosVirtualFile
{

private:
	OmnString		mFileName;

public:
	AosNameVirtualFile(
			const int server_id,
			const OmnString &filename)
	:
	AosVirtualFile(server_id),
	mFileName(filename)
	{
	}

	AosNameVirtualFile(const AosFileInfo &fileinfo)
	:
	AosVirtualFile(fileinfo.mPhysicalId),
	mFileName(fileinfo.mFileName)
	{
		mStartOffset = fileinfo.mStartOffset;
		mEndOffset = fileinfo.mStartOffset + fileinfo.mFileLen;
		mCrtOffset = fileinfo.mStartOffset;
		mFileLength = fileinfo.mFileLen;
	}

	~AosNameVirtualFile(){}

	virtual int64_t length(AosRundata *rdata)
	{
		if (mFileLength > 0) return mFileLength;
		AosDiskStat disk_stat;
		bool rslt = AosNetFileMgr::getSelf()->getFileLength(
			mFileLength, mFileName, mServerId, disk_stat, rdata);
		aos_assert_r(rslt, -1);
		mEndOffset = mStartOffset + mFileLength;
		return mFileLength;
	}

	virtual bool readData(
			const u64 &reqId,
			const int64_t &blocksize,
			AosRundata *rdata)
	{
		if(mFileLength < 0) length(rdata);

		int64_t offset = mCrtOffset;
		int64_t bsize = blocksize;

		if (offset + bsize > mEndOffset) 
		{
			bsize = mEndOffset - offset;
		}
		aos_assert_r(bsize >= 0, false);
		mCrtOffset += bsize;
/*
OmnScreen << "===============================NameVirtualFile readdata " 
			<< " reqId : " << reqId 
			<< " mFileName : " << mFileName
			<< " mCrtOffset : " << offset 
			<< " mStartOffset : " << mStartOffset 
			<< " mEndOffset : " << mEndOffset
			<< " bsize : " << bsize 
			<< " blocksize : " << blocksize
			<< " mServerId : " << mServerId 
			<< "========================================================" 
			<< endl;
*/
		AosAioCallerPtr thisptr(this, false);
		AosNetFileMgr::getSelf()->asyncReadFile(mFileName,
			mServerId, offset, bsize, reqId, thisptr, rdata); 
		return true;
	}
};
#endif
#endif
