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
#include "Sorter/MergeFileSorter.h"

#include "Debug/Debug.h"
#include "Sorter/DataSourceFixBin.h"
#include "Sorter/DataSourceVar.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/Sem.h"
#include "Util/File.h"
#include "Util/BuffArrayVar.h"
#include "XmlUtil/XmlTag.h"
#include "Porting/Sleep.h"

AosMergeFileSorter::AosMergeFileSorter(const bool regflag)
:
mDataSourceNum(0),
mFinished(false),
mNeedMerge(false),
mProcTotal(0),
mMergeTotal(0)
{
}


AosMergeFileSorter::AosMergeFileSorter(
		int buffsize,
		AosCompareFun * cmp_raw,
		vector<AosNetFileObjPtr> &files,
		const AosRundataPtr &rdata)
:
mCmpRaw(cmp_raw),
mCondVar(OmnNew OmnCondVar()),
mBuffQueueLock(OmnNew OmnMutex()),
mFinished(false),
mNeedMerge(false),
mProcTotal(0),
mMergeTotal(0)
{
	if (mCmpRaw->mAosAgrs.size() > 0) 
		mNeedMerge = true;

	for(u64 i=0; i<files.size(); i++)
	{
		OmnSPtr<DataSource> source = DataSource::createDataSource(files[i], buffsize, mCmpRaw);
		mDataSources.push_back(source);
		mDataSourcesRaw.push_back(source.getPtr());
	}

	OmnSPtr<DataSource> minsource = DataSource::createDataSource(mCmpRaw);
	minsource->setMinValue();
	mDataSources.push_back(minsource);
	mDataSourcesRaw.push_back(minsource.getPtr());
	mDataSourceNum = mDataSources.size();

	mLosterTree.resize(files.size());
	buildLosterTree();
}



AosMergeFileSorter::~AosMergeFileSorter()
{
	//OmnScreen << "~~~ AosMergeFileSorter Destructor " << endl;
}

bool 
AosMergeFileSorter::sanitycheck_var(
		AosCompareFun * comp,
		char *crt, 
		const int length)
{
//	return true;
	AosBuffPtr new_buff = OmnNew AosBuff(length AosMemoryCheckerArgs);
	new_buff->setBuff(crt, length);

	int reamin_size = 0;
	AosBuffArrayVarPtr buff_arry = OmnNew AosBuffArrayVar(true);
	buff_arry->setCompareFunc(comp);
	bool rslt = buff_arry->setBodyBuff(new_buff, reamin_size);
	aos_assert_r(rslt, false);
	AosBuffPtr head_buff = buff_arry->getHeadBuff();
	rslt = sanitycheck(comp, head_buff->data(), head_buff->dataLen());
	aos_assert_r(rslt, false);
	return true;
}

bool
AosMergeFileSorter::sanitycheck(
		AosCompareFun * comp,
		char *crt, 
		const int length)
{
//	return true;
	int record_len = comp->size;
	int entries = length/record_len;
	for (int i=0; i<entries; ++i)
	{
		// 1. check file is sorted
		if (i>0)
		{
			//aos_assert_r(comp->cmp(crt, crt-record_len) >= 0, false);
			if (comp->cmp(crt, crt-record_len) < 0)
			{
				OmnAlarm << enderr;
				return comp->cmp(crt, crt-record_len);
			}
		}

		// 2. check the entry whether is right
		//bool rslt = checkEntry(crt);
		//aos_assert_r(rslt, false);
		
		crt += record_len;
	}
	return true;
}


bool
AosMergeFileSorter::buildLosterTree()
{
	for (size_t i=0; i<mLosterTree.size(); i++)
	{
		mLosterTree[i] = mDataSourcesRaw.size()-1;
	}

	for (int i = mDataSourcesRaw.size() - 2; i >= 0; i--)
	{
		int win = i;
		int p_idx = (i + mDataSourcesRaw.size() - 1)/2;
		while(p_idx>0)
		{
			if (mNeedMerge)
			{
				if (*(mDataSourcesRaw[mLosterTree[p_idx]]) == *(mDataSourcesRaw[win]))
				{
					//mCmpRaw->mergeData(mDataSources[win]->getData(), mDataSources[mLosterTree[p_idx]]->getData());
					mMergeTotal++;
					mCmpRaw->mergeData(mDataSourcesRaw[mLosterTree[p_idx]]->getHeadBuff(), mDataSourcesRaw[win]->getHeadBuff());
					mDataSourcesRaw[win]->moveNext();
					p_idx = (win + mDataSourceNum - 1)/2;
					continue;
				}
			}
			if (*(mDataSourcesRaw[mLosterTree[p_idx]]) < *(mDataSourcesRaw[win]))
			{
				int tmp = win;
				win = mLosterTree[p_idx];
				mLosterTree[p_idx] = tmp;
			}

			p_idx = p_idx/2;
		}
		mLosterTree[0] = win;
	}
	return true;
}


bool
AosMergeFileSorter::sort()
{
	mTime = OmnGetSecond();
	bool rslt = false;
	vector<int> v;
	DataSource *value = NULL;
	if (mCmpRaw->mRecordType == AosDataRecordType::eBuff)
	{
		AosBuffPtr tmp_buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
		value = OmnNew DataSourceVar(tmp_buff, mCmpRaw); 
	}
	else
	{
		AosBuffPtr tmp_buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
		value = OmnNew DataSourceFixBin(tmp_buff, mCmpRaw); 
	}
	AosBuff *buff = OmnNew AosBuff(eDefBuffSize AosMemoryCheckerArgs);

	do {
		int idx = mLosterTree[0];
tag:
		if (mDataSourcesRaw[idx]->isMaxValue())
		{
			if (mCmpRaw->mRecordType == AosDataRecordType::eBuff)
			{
				rslt = sanitycheck_var(mCmpRaw, buff->data(), buff->dataLen());
			}
			else
			{
				rslt = sanitycheck(mCmpRaw, buff->data(), buff->dataLen());
			}
			aos_assert_r(rslt, false);

			mBuffQueueLock->lock();
			mBuffQueue.push(buff);
			mFinished = true;
			mCondVar->signal();
			mBuffQueueLock->unlock();
			break;
		}
	
		if (buff->dataLen() + mDataSourcesRaw[idx]->getEntryLen() > eDefBuffSize)
		{
			if (mCmpRaw->mRecordType == AosDataRecordType::eBuff)
			{
				rslt = sanitycheck_var(mCmpRaw, buff->data(), buff->dataLen());
			}
			else
			{
				rslt = sanitycheck(mCmpRaw, buff->data(), buff->dataLen());
			}
			aos_assert_r(rslt, false);

			mBuffQueueLock->lock();
			mBuffQueue.push(buff);
			mCondVar->signal();
			mBuffQueueLock->unlock();

			buff = OmnNew AosBuff(eDefBuffSize AosMemoryCheckerArgs);
		}

		if (mNeedMerge)
		{
			mDataSourcesRaw[idx]->getEntry(value);
			mDataSourcesRaw[idx]->moveNext();
			if (!mDataSourcesRaw[idx]->empty())
			{
				if (*(mDataSourcesRaw[idx]) == *value)
				{
					mMergeTotal++;
					mCmpRaw->mergeData(mDataSourcesRaw[idx]->getHeadBuff(), value->getHeadBuff());
					value->mBuff->clear();
					goto tag;
				}
				else
				{
					mProcTotal++;
					buff->setBuff(value->getData(), value->getEntryLen());
					value->mBuff->clear();
				}
			}
			else
			{
				mProcTotal++;
				buff->setBuff(value->getData(), value->getEntryLen());
				value->mBuff->clear();
			}
		}
		else
		{
			mProcTotal++;
			buff->setBuff(mDataSourcesRaw[idx]->getData(), mDataSourcesRaw[idx]->getEntryLen());
			mDataSourcesRaw[idx]->moveNext();
		}

		int win = idx;
		int p_idx = (idx + mDataSourceNum - 1)/2;
		while(p_idx>0)
		{
			if (mNeedMerge)
			{
				if (*(mDataSourcesRaw[mLosterTree[p_idx]]) == *(mDataSourcesRaw[win]))
				{
					//mCmpRaw->mergeData(mDataSources[win]->getData(), mDataSources[mLosterTree[p_idx]]->getData());
					mMergeTotal++;
					mCmpRaw->mergeData(mDataSourcesRaw[mLosterTree[p_idx]]->getHeadBuff(), mDataSourcesRaw[win]->getHeadBuff());
					mDataSourcesRaw[win]->moveNext();
					p_idx = (win + mDataSourceNum - 1)/2;
					continue;
				}
			}
			if (*(mDataSourcesRaw[mLosterTree[p_idx]]) < *(mDataSourcesRaw[win]))
			{
				int tmp = win;
				win = mLosterTree[p_idx];
				mLosterTree[p_idx] = tmp;
			}
			p_idx = p_idx/2;
		}
		mLosterTree[0] = win;
	}while(1);
	OmnScreen << "@@@@andy :: MergeFileSorter End Time is " << OmnGetSecond() - mTime  << ". " << endl;
	OmnScreen << "@@@@andy :: mProcTotal : " << mProcTotal << ", mMergeTotal : " << mMergeTotal << "." << endl;
	OmnDelete value;
	return true;
}


AosBuffPtr 
AosMergeFileSorter::nextBuff()
{
	mBuffQueueLock->lock();

	AosBuffPtr mTmpBuff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	while (mBuffQueue.empty())
	{
		if (mFinished)
		{
			mBuffQueueLock->unlock();
			return NULL;
		}
		mCondVar->wait(mBuffQueueLock);
	}
	AosBuffPtr buff = mBuffQueue.front();
	bool rslt = AosMergeFileSorter::sanitycheck(mTmpBuff, buff, this);
	aos_assert_r(rslt, NULL);
	mBuffQueue.pop();

	mBuffQueueLock->unlock();
	return buff;
}

bool
AosMergeFileSorter::sanitycheck(
	const AosBuffPtr &tmp_buff,
	const AosBuffPtr &buff,
	const AosMergeFileSorterPtr &sort)
{
	//return true;
	if (sort->mCmpRaw->mRecordType == AosDataRecordType::eBuff) {
		AosMergeFileSorter::sanitycheck_var(sort->mCmpRaw, buff->data(), buff->dataLen());
	} else {
		AosMergeFileSorter::sanitycheck(sort->mCmpRaw, buff->data(), buff->dataLen());
	}
	if (tmp_buff->dataLen() == 0) {
		tmp_buff->setBuff(buff->data(), buff->dataLen());
	} else {
		tmp_buff->setBuff(buff->data(), buff->dataLen());
		if (sort->mCmpRaw->mRecordType == AosDataRecordType::eBuff) {
			AosMergeFileSorter::sanitycheck_var(sort->mCmpRaw, tmp_buff->data(), tmp_buff->dataLen());
		} else {
			AosMergeFileSorter::sanitycheck(sort->mCmpRaw, tmp_buff->data(), tmp_buff->dataLen());
		}
		tmp_buff->clear();
		tmp_buff->setBuff(buff->data(), buff->dataLen());
	}

	return true;
}

