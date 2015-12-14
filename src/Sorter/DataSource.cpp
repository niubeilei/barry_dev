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
#include "Sorter/DataSource.h"

#include "Sorter/DataSourceFixBin.h"
#include "Sorter/DataSourceVar.h"
#include "Debug/Debug.h"
#include "Thread/Sem.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"

DataSource::DataSource(
		AosNetFileObjPtr &file, 
		int buffSize,
		AosCompareFun *cmp)
:
mFile(file), 
mFileLen(0),
mReadTotal(0),
mBuffSize(buffSize/cmp->size * cmp->size),
mBuffRaw(0),
mNextBuffRaw(0),
mData(NULL),
mCmpRaw(cmp),
mSem(OmnNew OmnSem(0)),
mCondVar(OmnNew OmnCondVar()),
mEOF(false),
mMinMaxFlag(0),
mEntryLen(0),
mProcTotal(1)
{
	mFile->setCaller(this);
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	mFileLen = mFile->length(rdata.getPtr());

//OmnScreen << "read ###### file id is " << mFile->getFileId() <<  " ReqId is " << mReqId++  << ". "<< endl; 
	mFile->readData(mReqId, mBuffSize, OmnApp::getRundata().getPtr());
}


DataSource::DataSource(
		const AosBuffPtr &buff,
		AosCompareFun *cmp)
:
mFile(NULL),
mFileLen(0),
mReadTotal(0),
mBuffSize(0),
mBuff(buff),
mBuffRaw(buff.getPtr()),
mNextBuff(NULL),
mNextBuffRaw(0),
mData(buff->data()),
mCmpRaw(cmp),
mSem(OmnNew OmnSem(0)),
mCondVar(OmnNew OmnCondVar()),
mEOF(true),
mMinMaxFlag(0),
mEntryLen(0),
mReqId(0),
mProcTotal(1)
{
}

DataSource::DataSource(AosCompareFun *cmp)
:
mCmpRaw(cmp),
mEntryLen(0),
mReqId(0),
mProcTotal(1)
{
}


OmnSPtr<DataSource>
DataSource::createDataSource(
	AosNetFileObjPtr &file,
	int buffSize,
	AosCompareFun *cmp)
{
	if (cmp->mRecordType == AosDataRecordType::eBuff)
	{
		return OmnNew DataSourceVar(file, buffSize, cmp);
	}
	return OmnNew DataSourceFixBin(file, buffSize, cmp);
}

OmnSPtr<DataSource>
DataSource::createDataSource(AosCompareFun *cmp)
{
	if (cmp->mRecordType == AosDataRecordType::eBuff)
	{
		return OmnNew DataSourceVar(cmp);
	}
	return OmnNew DataSourceFixBin(cmp);
}

bool
DataSource::getEntry(DataSource *value)
{
	aos_assert_r(value, false);
	aos_assert_r(value->mBuff->dataLen() == 0, false);
	value->mBuff->setBuff(getData(), getEntryLen());
	value->mBuffRaw = value->mBuff.getPtr();
	value->mData = value->mBuff->data();
	value->mEntryLen = getEntryLen();
	return true;
}

