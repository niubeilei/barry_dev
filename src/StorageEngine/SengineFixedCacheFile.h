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
// 01/12/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_StorageEngine_SengineFixedCacheFile_h
#define Aos_StorageEngine_SengineFixedCacheFile_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "StorageEngine/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "XmlUtil/Ptrs.h"

#include <queue>
struct AosTransOffset
{
	u64 		docid;
	u32 		offset;

	AosTransOffset(const u64 id, const u32 of)
	:
	docid(id),
	offset(of)
	{
	}

	~AosTransOffset() {};

	bool operator < (const  AosTransOffset &rhs) const
	{
		return  docid < rhs.docid;
	}
};

struct AosFixedFileInfo: public OmnRCObject
{
private:
	OmnDefineRCObject;

	u64 						fileid;
	OmnFilePtr  				file;
	u32							write_last_offset;
	vector<AosTransOffset> 		sort_trans_offset;
	u32 						read_index;
	u32							num_req;
	bool						is_full;
	u32							modify_time;	

public:
	AosFixedFileInfo(u64 fid, OmnFilePtr &ff)
	:
	fileid(fid),
	file(ff),
	write_last_offset(0),
	read_index(0),
	num_req(0),
	is_full(false)
	{
	}
	
	~AosFixedFileInfo(){}

	u64 getFileId() const {return fileid;}

	OmnFilePtr getFile() const {return file;}

	u32 getLastOffset() const {return write_last_offset;}

	void setLastOffset(u32 f) {write_last_offset = f;}

	u32 getReadOffset() 
	{
		if (read_index >= sort_trans_offset.size()) return (u32)-1; 
		if (read_index == 0)
		{
			sortVector();
			//print();
		}

		AosTransOffset vv = sort_trans_offset[read_index++];
		return vv.offset;
	}

	void setFull(bool b) {is_full = b;}

	bool getFull() {return is_full;}

	void numReqMinus()
	{
		aos_assert(num_req > 0);
		num_req --;
		if (num_req == 0)
		{
			aos_assert(read_index >= sort_trans_offset.size());
		}
	}

	void numReqPlus(){num_req ++;}

	u32 getNumReq(){return num_req;}

	bool isReadFinished()
	{
		return (num_req == 0);
	}

	void setSortTransOffset(const u64 docid, const u32 offset)
	{
		AosTransOffset oo(docid, offset);
		sort_trans_offset.push_back(oo);
	}

	void sortVector()
	{
		//http://blog.csdn.net/aguisy/article/details/5787257
		sort(sort_trans_offset.begin(), sort_trans_offset.end());
	}

	void print()
	{
		vector<AosTransOffset>::iterator itr = sort_trans_offset.begin();
		for (; itr != sort_trans_offset.end(); ++itr)
		{
			AosTransOffset oo = *itr;
			OmnScreen << "docid :" << oo.docid << " , " << oo.offset << endl;
		}
	}

	u32 getLastModifyTime()
	{
		return OmnFile::getLastModifyTime(file->getFileName());
	}

	u64 getFileLength()
	{
		return OmnFile::getFileLengthStatic(file->getFileName());
	}


};

class AosSengineFixedCacheFile : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eFlagByte0 = 'g',
		eFlagByte1 = 'e',
		eFlagByte2 = 'y',
		eFlagByte3 = 'z',

		eMaxFileSize = 300000000 //300M
	};

private:
	u64							mId;
	queue<AosFixedFileInfoPtr>	mReadQueue;
	AosFixedFileInfoPtr			mWriteFile;
	u64							mSizeid;
	int							mRecordSize;
	OmnMutexPtr                 mLock;
	u64							mJobId;
	u32 						mLogicTaskid;	
	bool						mIsRemoveFile;

public:
	AosSengineFixedCacheFile(
			const u64 &id,
			const u64 &job_id,
			const u32 &logic_taskid,
			const u32 &sizeid,
			const int &record_size,
			const bool removefile);
	~AosSengineFixedCacheFile();

	bool saveToFile(const u64 &num_docs,
				const AosBuffPtr &docid_buff,
				const AosBuffPtr &buff,
				bool &full_file,
				const AosRundataPtr &rdata);

	bool readFromFile(u64 &num_docs,
				AosBuffPtr &docid_buff,
				AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	u64 getSizeId() const {return mSizeid;}

	int getRecordSize() const {return mRecordSize;}

	u32 getReadQueueSize() const{return mReadQueue.size();}

	u64 getId()const {return mId;}

	u64 getJobId() const {return mJobId;}

	u32 getLogicTaskid() const {return mLogicTaskid;}

	bool cleanData(const AosRundataPtr &rdata);
	
	u32 getLastModifyTime()
	{
		mLock->lock();
		if (!mWriteFile) 
		{
			mLock->unlock();
			return 0;
		}
		u32 time =  mWriteFile->getLastModifyTime();
		mLock->unlock();
		return time;
	}

	u64 getFileLength()
	{
		mLock->lock();
		if(!mWriteFile) 
		{
			mLock->unlock();
			return 0;
		}
		u64 size = mWriteFile->getFileLength();
		mLock->unlock();
		return size;
	}

	static bool readFromFileStatic(
			const OmnFilePtr &file,
			const u32 &first_offset,
			const u64 &file_size,
			u64 &num_docs,
			AosBuffPtr &docidbuff,
			AosBuffPtr &buff,
			u32 &last_offset,
			const AosRundataPtr &rdata);


private:
	AosFixedFileInfoPtr getWriteFileLocked(
			const u32 &requested_space,
			bool &full_file,
			const AosRundataPtr &rdata);

	AosFixedFileInfoPtr getReadFile();

	bool deleteFileinfoLock(
			const AosFixedFileInfoPtr &fileinfo,
			const AosRundataPtr &rdata);

	bool sanityCheck(const OmnFilePtr &file, const u32 &offset);
};
#endif

#endif
