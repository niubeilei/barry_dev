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
// 05/26/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_CounterServer_CounterRcdMgr_h
#define Omn_CounterServer_CounterRcdMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Rundata/Rundata.h"
#include "CounterServer/Ptrs.h"
#include "CounterUtil/CounterRecord.h"
#include "CounterUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <map>


class AosCounterRcdMgr : public OmnRCObject 
{
	OmnDefineRCObject;
public:
	enum
	{
		eRecordSize = sizeof(int64_t) + sizeof(u64),
		eMaxRecordFileSize = 0xffffffff
	};

private:
	OmnMutexPtr         mLock;
	u32					mId;
	OmnString			mFileDir;
	OmnString			mCtlFileName;
	OmnString			mRecordFileName;
	OmnFilePtr			mCtlFile;
	u32					mCrtFileSeqno;
	u32					mCrtFileOffset;

	u64 				mCrtFileLength;
	map<u32, OmnFilePtr>    mFiles;


public:
	AosCounterRcdMgr(const AosXmlTagPtr &cfg, const u32 id);
	~AosCounterRcdMgr() {}

	bool 	config(const AosXmlTagPtr &config);

	bool	reset();

	bool	openControlFile();

	OmnFilePtr	openCrtRecordFile(const u32 &seqno);

	u64		getNextCounterId(const u32 recordLen) 
	{
		mLock->lock();
		aos_assert_r(recordLen > 0, 0);
		u64 crt_flen = mCrtFileLength;
		mCrtFileLength += recordLen;
		if (mCrtFileLength > eMaxRecordFileSize)
		{
			mCrtFileSeqno++;
			mCtlFile->setU32(sizeof(u32), mCrtFileSeqno, true);
			mCrtFileLength = recordLen;
			crt_flen = 0;
		}
		u64 counterId = ((u64)mCrtFileSeqno << 32) +  crt_flen;
		mLock->unlock();
		return counterId;
	}
	
	inline void	setRecordBuff(
					AosBuff &buff,
					const AosCounterRecord &record)
						
	{
		buff.setU64(record.getCounterId());
		buff.setI64(record.getCounterValue());
		buff.setU64(record.getCounterNameIILID());
		buff.setU64(record.getCounterValueIILID());
		buff.setU32(record.getCounterSeqno());
		buff.setU64(record.getCounterOffset());
		buff.setU32(record.getCounterTimeSize());
		buff.setU32(record.getCounterTimeType());
		buff.setI64(record.getCounterMax());
		buff.setI64(record.getCounterMin());
		buff.setI64(record.getCounterMean());
		buff.setI64(record.getCounterStderr());
		buff.setI64(record.getCounterNum());

		buff.setU32(record.getCounterExtSeqno());
		buff.setU64(record.getCounterExtOffset());

		buff.setOmnStr(record.getCounterName());
	}

	bool	createRecord(const AosCounterRecord &record,
						 const AosRundataPtr &rdata);

	/*
	bool	readCounter(AosCounterRecord &record,
						const AosRundataPtr &rdata);
	*/
	AosCounterRecordPtr	readCounter(const u64 &counterId,
						const AosRundataPtr &rdata);

	bool	modifyCounter(const AosCounterRecordPtr &rcd,
						  const AosRundataPtr &rdata);

	OmnFilePtr	getFile(const u32 &seqno);

	bool	getLocation(const u64 &counterId,
						u32 &seqno,
						u32 &offset);

	bool    procStatCounter(
			         AosCounterRecord &record,
			         const AosRundataPtr &rdata);
};
#endif

#endif
