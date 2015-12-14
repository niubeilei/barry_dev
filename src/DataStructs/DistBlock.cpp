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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/DistBlock.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataStructs/DataStructsUtil.h"

AosDistBlock::AosDistBlock(const int stat_doc_size, const int dft)
:
mDistributionBlock(0),
mDistBlockData(0),
mDistBlockDocid(0),
mDistBlockId(eAosInvBlockId),
mDistBlockVid(-1),
mStartDocid(0),
mStatDocSize(stat_doc_size),
mDocsPerDistBlock(0),
mNewDistBlock(false),
mDftValue(dft)
{
	//mDocsPerDistBlock = eDfmDistBlockSize / mStatDocSize;
	mDocsPerDistBlock = calculateDocPerDistBlock(mStatDocSize);
}

AosDistBlock::AosDistBlock()
:
mDistributionBlock(0),
mDistBlockData(0),
mDistBlockDocid(0),
mDistBlockId(eAosInvBlockId),
mDistBlockVid(-1),
mStartDocid(0),
mStatDocSize(0),
mDocsPerDistBlock(0),
mNewDistBlock(false),
mDftValue(0)
{
}

AosDistBlock::~AosDistBlock()
{
}

void 
AosDistBlock::set(const AosBuffPtr &buff, 
			const u64 &dist_block_did, 
			const u32 dist_block_id,
			const bool &new_dist_block)
{
	mNewDistBlock = new_dist_block;
	mDistributionBlock = buff;
	mDistBlockData = mDistributionBlock->data();
	mDistBlockDocid = dist_block_did;

	int vvid = AosDataStructsUtil::getVirtualIdByGroupid(dist_block_id);
	int vid = AosGetCubeId(dist_block_did);
	aos_assert(vvid == vid);
	mDistBlockVid = vid;

	aos_assert(dist_block_id != eAosInvBlockId);
	mDistBlockId = dist_block_id;

	mStartDocid = mDistBlockId * mDocsPerDistBlock;
}


void 
AosDistBlock::clear()
{
	mDistributionBlock = 0;
	mDistBlockData = 0;
	mDistBlockDocid = 0;
	mDistBlockVid = -1;
	mDistBlockId = eAosInvBlockId;
	mStartDocid = 0;
}


char *
AosDistBlock::getStatDoc(const u64 &docid) 
{
	aos_assert_r(mDocsPerDistBlock > 0, 0);
	aos_assert_r(mDistBlockId != eAosInvBlockId, 0);
	aos_assert_r(docid >= mStartDocid, 0);

	u64 delta = docid - mStartDocid; 
	u32 start_pos = delta * mStatDocSize;
	int64_t new_size = start_pos + mStatDocSize;
	aos_assert_r(new_size <= eDfmDistBlockSize, 0);
	if (new_size > mDistributionBlock->dataLen())
	{
		// The stat doc does not exist. Check whether we need to create
		//if (!create_flag) return 0;

		if (new_size > mDistributionBlock->buffLen())
		{
			int64_t rslt_size = mDistributionBlock->resize(new_size);
			aos_assert_r(rslt_size >= new_size, false);

			mDistBlockData = mDistributionBlock->data();
		}

		int64_t data_len = mDistributionBlock->dataLen();
		int64_t size = new_size - data_len;
		aos_assert_r(data_len + size <= mDistributionBlock->buffLen(), 0);

		//memset(&mDistBlockData[data_len], 0, size);
		memset(&mDistBlockData[data_len], mDftValue, size);
		mDistributionBlock->setDataLen(new_size);
//OmnScreen << "new new new new; mDistBlockDocid:" << mDistBlockDocid << ";mStartDocid:" << mStartDocid << "; docid:" << docid << ";" << endl;
	}

	aos_assert_r(mDistributionBlock->dataLen() % mStatDocSize == 0, 0);
	aos_assert_r(mDistributionBlock->dataLen() >= new_size, 0);
//OmnScreen << "new new new new1; mDistBlockDocid:" << mDistBlockDocid << ";mStartDocid:" << mStartDocid << "; docid:" << docid << ";" 
//	<< mDistributionBlock->dataLen()<< endl;
//cout << "AAAAAAAAAAAAAAAaa Dist Block; docid:" << docid << "; " << (void *) &mDistBlockData[start_pos] << endl;
	aos_assert_r(mDistributionBlock->dataLen() >= start_pos + mStatDocSize, 0);
	return &mDistBlockData[start_pos];
}


void 
AosDistBlock::setStatDoc(const u64 &docid, const AosBuffPtr &buff)
{
	aos_assert(mDocsPerDistBlock);
	aos_assert(buff->dataLen() == mStatDocSize);
	aos_assert(mDistBlockId != eAosInvBlockId);
	aos_assert(mDistributionBlock);
	aos_assert(docid >= mStartDocid);

	u64 delta = docid - mStartDocid; 
	u32 start_pos = delta * mStatDocSize;

	int64_t new_size = start_pos + mStatDocSize;
	aos_assert(new_size <= eDfmDistBlockSize);
	if (new_size >= mDistributionBlock->buffLen())
	{
//OmnScreen << "new new new new2; mDistBlockDocid:" << mDistBlockDocid << ";mStartDocid:" << mStartDocid << "; docid:" << docid << ";" << endl;
		mDistributionBlock->resize(new_size);
	}

	mDistributionBlock->setCrtIdx(start_pos);
	mDistributionBlock->setBuff(buff);
	mDistBlockData = mDistributionBlock->data();
}


void
AosDistBlock::setStatDocDftValue(
		char *data,
		const int dft,
		const int64_t &len)
{
	memset(data, dft, len);
}

/*
int 
AosDistBlock::getStatDocSize(const u64 &docid)
{
	u64 delta = docid - mStartDocid; 
	u32 start_pos = delta * mStatDocSize;
	int64_t new_size = start_pos + mStatDocSize;
	aos_assert_r(mDistributionBlock->dataLen() >= new_size, 0); 
	return mStatDocSize;

}
*/
