////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/27/2012 Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_DataSource_h
#define Aos_Sorter_DataSource_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/Ptrs.h"
#include "Thread/Sem.h"
#include "Util/CompareFun.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class DataSource : public AosFileReadListener
{
	OmnDefineRCObject;	

public:
	AosNetFileObjPtr 		mFile;
	u64						mFileLen;
	u64						mReadTotal;
	int 					mBuffSize;
	AosBuffPtr				mBuff;
	AosBuff					*mBuffRaw;
	AosBuffPtr				mNextBuff;
	AosBuff					*mNextBuffRaw;
	char * 					mData;
	AosCompareFun   		*mCmpRaw;
	OmnSemPtr				mSem;
	OmnCondVarPtr			mCondVar;
	bool					mEOF;
	int 					mMinMaxFlag;
	int						mEntryLen;
	int						mReqId;
	int						mProcTotal;

	DataSource(AosNetFileObjPtr &file, int buffSize, AosCompareFun *cmp);
	DataSource(const AosBuffPtr &buff, AosCompareFun *cmp);
	DataSource(AosCompareFun *cmp);

	virtual bool operator > (DataSource &rhs) 
	{
		if (mMinMaxFlag == 1) return false;
		if (mMinMaxFlag == 2) return true;
		if (rhs.mMinMaxFlag == 1) return true;
		if (rhs.mMinMaxFlag == 2) return false;

		return mCmpRaw->cmp(getHeadBuff(), rhs.getHeadBuff()) > 0;
	}

	virtual bool operator < (DataSource &rhs) 
	{
		if (mMinMaxFlag == 1) return true;
		if (mMinMaxFlag == 2) return false;
		if (rhs.mMinMaxFlag == 1) return false;
		if (rhs.mMinMaxFlag == 2) return true;
		return mCmpRaw->cmp(getHeadBuff(), rhs.getHeadBuff()) < 0;
	}

	virtual bool operator == (DataSource &rhs) 
	{
		if (mMinMaxFlag != 0) return false;
		if (rhs.mMinMaxFlag != 0) return false;

		return mCmpRaw->cmp(getHeadBuff(), rhs.getHeadBuff()) == 0;
	}

	virtual void setMinValue() 
	{
		mMinMaxFlag = 1;
	}

	virtual void setMaxValue() 
	{
		mMinMaxFlag = 2;
	}

	virtual bool isMaxValue()
	{
		return mMinMaxFlag == 2;
	}

	//virtual int getDataLen();
	virtual char* getHeadBuff() = 0;
	virtual char* getData() = 0;
	virtual int getEntryLen() = 0;
	virtual void moveNext() = 0;
	virtual bool getEntry(DataSource *value);

	virtual bool empty()
	{
		if (mEOF && !mBuff && !mNextBuff)
		{
			return true;
		}
		return false;
	}

	virtual void fileReadCallBack(
					const u64 &reqId, 
					const int64_t &expected_size, 
					const bool &finished, 
					const AosDiskStat &disk_stat) = 0;

	static OmnSPtr<DataSource> createDataSource(
					AosNetFileObjPtr &file,
					int buffSize,
					AosCompareFun *cmp);

	static OmnSPtr<DataSource> createDataSource(AosCompareFun *cmp);
};
#endif

