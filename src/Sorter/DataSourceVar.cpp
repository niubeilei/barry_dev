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
#include "Util/BuffArrayVar.h"
#include "Sorter/DataSourceVar.h"
#include "Sorter/MergeFileSorter.h"
#include "Rundata/Rundata.h"

DataSourceVar::DataSourceVar(AosNetFileObjPtr &file, int buffSize, AosCompareFun *cmp)
:
DataSource(file, buffSize, cmp),
mIncompleteData(OmnNew AosBuff(1000 AosMemoryCheckerArgs)),
mIncompleteDataRaw(mIncompleteData.getPtr()),
mHeadBuff(OmnNew AosBuff(1000 AosMemoryCheckerArgs)),
mHeadBuffRaw(mHeadBuff.getPtr())
{ 
}

DataSourceVar::DataSourceVar(
		const AosBuffPtr &buff, 
		AosCompareFun *cmp)
:
DataSource(buff, cmp),
mIncompleteData(OmnNew AosBuff(1000 AosMemoryCheckerArgs)),
mIncompleteDataRaw(mIncompleteData.getPtr()),
mHeadBuff(OmnNew AosBuff(1000 AosMemoryCheckerArgs)),
mHeadBuffRaw(mHeadBuff.getPtr())
{ 
}

DataSourceVar::DataSourceVar(AosCompareFun *cmp)
:
DataSource(cmp)
{
}

char *
DataSourceVar::getHeadBuff()
{
	if (mHeadBuffRaw->dataLen() == 0)
		procHeadBuff();

	return mHeadBuffRaw->data();
}

char *
DataSourceVar::getData()
{
	if (!mData)
	{
		aos_assert_r(!mBuffRaw, NULL); 
		while (!mNextBuffRaw)
		{
			aos_assert_r(!mEOF, NULL);
			mSem->wait();
		}

		if (mIncompleteDataRaw->dataLen() > 0)
		{
			mIncompleteDataRaw->setBuff(mNextBuffRaw->data(), mNextBuffRaw->dataLen());
			mBuff = mIncompleteData;
			mBuffRaw = mBuff.getPtr();
			mData = mBuffRaw->data();

			//mIncompleteDataRaw->clear();
			mIncompleteData = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
			mIncompleteDataRaw = mIncompleteData.getPtr();
		}
		else
		{
			mBuff = mNextBuff;
			mBuffRaw = mBuff.getPtr();
			mData = mBuffRaw->data();
		}
		aos_assert_r(mBuffRaw, NULL);
		aos_assert_r(mData, NULL);
		mNextBuff = 0;
		mNextBuffRaw = 0;
		if (!mEOF)
		{
			mFile->readData(mReqId, mBuffSize, OmnApp::getRundata().getPtr());
		}
	}
	aos_assert_r(mData, NULL)
	return mData;
}

void
DataSourceVar::procHeadBuff()
{
	int	record_len = 0;
	int crt_pos = 0, str_len = 0;

	char *body_buff = getData();
	aos_assert(body_buff);

	i64 body_addr = (i64)body_buff;

	vector<AosDataFieldType::E> types;
	mCmpRaw->getDataFieldsType(types);
	aos_assert(types.size() < 65536);
	mHeadBuffRaw->reset();
	mHeadBuffRaw->setInt(0);
	mHeadBuffRaw->setI64(body_addr);
	record_len = *(int *)(body_buff);

	int decode_len = record_len;
	bool rslt = AosBuff::decodeRecordBuffLength(decode_len);
	aos_assert(rslt);
	mEntryLen = decode_len + sizeof(int);
	//crt_pos += sizeof(int);
	rslt = AosBuffArrayVar::buildHeaderBuff(types, mHeadBuffRaw, body_buff+sizeof(int));
	aos_assert(rslt);
}


void 
DataSourceVar::moveNext()
{
	mHeadBuff->clear();
	aos_assert(checkIsCompleteEntry());
	aos_assert(mEntryLen > 0);

	mData = mData + mEntryLen;
	mEntryLen = 0;
	if (checkIsCompleteEntry())
	{
		mProcTotal++;
		return;
	}
	else
	{
		aos_assert(mIncompleteDataRaw->dataLen() == 0);
		mIncompleteDataRaw->setBuff(mData, mBuffRaw->data() + mBuffRaw->dataLen() - mData);
		mBuff = 0;
		mBuffRaw = 0;
		mData = 0;
	}
	if (empty())
	{
		aos_assert(mIncompleteDataRaw->dataLen() == 0);
		mFile = NULL;
		setMaxValue();
	}
}

bool
DataSourceVar::checkIsCompleteEntry()
{
	if (mBuffRaw->data() + mBuffRaw->dataLen() - mData < 4)
	{
		return false;
	}
	else
	{
		if (mEntryLen != 0)
		{
			return (mBuffRaw->data() + mBuffRaw->dataLen() - mData) >= mEntryLen;
		}
		int record_len = *(int *)(getData());
		int decode_len = record_len;
		bool rslt = AosBuff::decodeRecordBuffLength(decode_len);
		aos_assert_r(rslt, false);
		return (mBuffRaw->data() + mBuffRaw->dataLen() - mData) >= (i64)(decode_len + sizeof(int));
	}
	return false;
}

void 
DataSourceVar::fileReadCallBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished, 
		const AosDiskStat &disk_stat)
{
//OmnScreen << " @@@@@@@@@@@@@@@ : ReadCallBack " << endl;
//aos_assert(!mNextBuffRaw);
//aos_assert(expected_size > 0);
//aos_assert(!mEOF);

//OmnScreen << "call back ###### file id is " << mFile->getFileId() <<  "ReqId is " << reqId  << ". "<< endl; 
//OmnScreen << "call back mReadTotal is " << mReadTotal  << ", " << "expected_size is " << expected_size << endl;
	mNextBuff = mFile->getBuff();
	mNextBuffRaw = mNextBuff.getPtr();
	mReadTotal += expected_size;
	if (mReadTotal == mFileLen)
	{
		mEOF = true;
	}
	mSem->post();
	return;
}

int 
DataSourceVar::getEntryLen()
{
	if (mEntryLen == 0)
	{
		int record_len = *(int *)(getData());
		int decode_len = record_len;
		bool rslt = AosBuff::decodeRecordBuffLength(decode_len);
		aos_assert_r(rslt, -1);
		mEntryLen = decode_len + sizeof(int);
	}
	return mEntryLen; 
}

bool 
DataSourceVar::getEntry(DataSource *value)
{
	aos_assert_r(value, false);
	DataSourceVar *vv = dynamic_cast<DataSourceVar *>(value);
	aos_assert_r(vv, false);
	aos_assert_r(vv->mBuff->dataLen() == 0, false);
	vv->mBuff->setBuff(getData(), getEntryLen());
	vv->mBuffRaw = vv->mBuff.getPtr();
	vv->mData = vv->mBuff->data();
	vv->mEntryLen = getEntryLen();
	vv->mHeadBuff->clear();
	/*
	if (mHeadBuffRaw->dataLen() > 0)
	{
		vv->mHeadBuff->setBuff(mHeadBuffRaw->data(), mHeadBuffRaw->dataLen());
	}
	*/
	vv->mHeadBuffRaw = vv->mHeadBuff.getPtr();
	return true;
}
