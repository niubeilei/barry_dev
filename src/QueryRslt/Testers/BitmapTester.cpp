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
// 01/06/2013       Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/Testers/BitmapTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"

AosBitmapTester::AosBitmapTester()
{
	config();
}


AosBitmapTester::~AosBitmapTester()
{
}

bool
AosBitmapTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	
	mMaxBufferSize = conf->getAttrU64("zky_max_buffer_size", 0);
	aos_assert_r(mMaxBufferSize > 0, false);

	return true;
}


bool 
AosBitmapTester::start()
{
	aos_assert_r(basicTest(), false);
	return true;
}


bool
AosBitmapTester::basicTest()
{
	int tries = 10000000;
	
	for (mRound = 0; mRound<tries; mRound++)
	{
		OmnScreen << "Check: " << mRound << endl;
		aos_assert_r(initData(), false);
		OmnScreen <<"countData start "<<endl;
		aos_assert_r(countData(), false);
		OmnScreen <<"checkRslt() start "<<endl;
		aos_assert_r(checkRslt(), false);
	}
	return true;
}

bool
AosBitmapTester::initData()
{
	// This function init the data send to bitmap.
	aos_assert_r(initBlocks(), false);
	aos_assert_r(initRawData(), false);

	mA->reset();
	mB->reset();
	mAandB->reset();
	mAorB->reset();
	mAnotB->reset();

OmnScreen << "mA: " << mA->getU64(0) << " : " << mA->dataLen()/8
	<< " mB: " << mB->getU64(0) << " : " << mB->dataLen()/8
	<< " mAandB: " << mAandB->getU64(0) << " : " << mAandB->dataLen()/8
	<< "mAorB: " << mAorB->getU64(0) << " : " << mAorB->dataLen()/8 << endl;

	return true;
}

bool
AosBitmapTester::countData()
{
	// This function send the inited data to bitmap,
	// and get the rslt from bitmap.
OmnScreen << "A: " << mA->dataLen()/8
	<< " B: " << mB->dataLen()/8
	<< " AandB: " << mAandB->dataLen()/8
	<< " AorB: " << mAorB->dataLen()/8 
	<< " mBlocks'size: " << mBlocks.size() << endl;

	aos_assert_r(setBitmaps(), false);
	aos_assert_r(countAnd(), false);
	aos_assert_r(countOr(), false);
	aos_assert_r(countNotIn(), false);
	aos_assert_r(countOrAnd(), false);
	return true;
}

bool
AosBitmapTester::setBitmaps()
{
	mA->reset();
	mB->reset();
	
	OmnScreen << "-----------------------------------" << endl;
	OmnScreen << "A: " << mA->dataLen()/8 << endl;
	mBitmapA = OmnNew AosBitmap();
	aos_assert_r(mBitmapA, false);
	u64 start_time = OmnGetTimestamp();
	mBitmapA->appendBlock((u64*)mA->data(), mA->dataLen()/8);
	u64 end_time = OmnGetTimestamp();
	OmnScreen << " time: " << end_time-start_time << endl;
	OmnScreen << "-----------------------------------\n\n" << endl;
	
	OmnScreen << "-----------------------------------" << endl;
	OmnScreen << "B: " << mB->dataLen()/8 << endl;
	mBitmapB  = OmnNew AosBitmap();
	aos_assert_r(mBitmapB, false);
	start_time = OmnGetTimestamp();
	mBitmapB->appendBlock((u64*)mB->data(), mB->dataLen()/8);
	end_time = OmnGetTimestamp();
	OmnScreen << " time: " << end_time-start_time << endl;
	OmnScreen << "-----------------------------------\n\n" << endl;
	
	return true;
}

bool
AosBitmapTester::countOr()
{
	// Check 'Or', send 'mA' and 'mB'
	// to bitmap, get result from bitmap.
	mAorB->reset();
	
	mOrBitmapRslt = (AosBitmap*)(AosBitmap::countOr(mBitmapA, mBitmapB).getPtr());

	OmnScreen << "-----------------------------------" << endl;
	return true;
}

bool
AosBitmapTester::countAnd()
{
	// Check 'And', send 'mA' and 'mB'
	// to bitmap, get result from bitmap.
	mAandB->reset();
	
	mAndBitmapRslt = (AosBitmap*)(AosBitmap::countAnd(mBitmapA, mBitmapB).getPtr());
	return true;
}

bool
AosBitmapTester::countNotIn()
{
	// Check 'NotIn', send 'mA' and 'mB'
	// to bitmap, get result from bitmap.
	return true;
}

bool
AosBitmapTester::countOrAnd()
{
	// Check 'OrAnd'.
	mOrAndRsltVect.clear();
	return true;
}

bool
AosBitmapTester::checkRslt()
{
	// This function check the result returned is true
	// or not.
	
	mA->reset();
	mB->reset();
	mAandB->reset();
	mAorB->reset();
	
	OmnScreen<< "mAandB" << endl;
	mAndBitmapRslt->reset();
	u64 id = 0;
	u64 nCount = 0;
	u64 index = 0;
	nCount = mAandB->dataLen()/8;
	for(index=0; index<nCount; index++)
	{
		id = mAndBitmapRslt->nextDocid();

		if((id == 0))
		{
			OmnScreen << "id == 0 or -1 " << " index " << index << endl;
			break;
		}
		u64 docid = mAandB->getU64(0);
		if (docid != id)
		{
		     OmnAlarm << "index : " << index << " docid: " << docid << " id" << id << enderr;

			 mAndBitmapRslt->reset();
			 u64 i = 0;
			 while(mAndBitmapRslt->nextDocid())
				 i++;
              OmnScreen<< "mAndBitmapRslt count : " << i <<endl;

			  mAndBitmapRslt->reset();
			  mAandB->reset();
			  for(i=0; i<10; i++)
			  {
				  OmnScreen<< " i : " << i << "mAndBitmapRst : " << mAndBitmapRslt->nextDocid() << " mAndB :" << mAandB->getU64(0) <<endl; 
			  }

			 aos_assert_r(docid == id, false);
		 }
	}
	aos_assert_r(index == nCount, false);

/*
	docids.clear();
	mOrBitmapRslt->getDocids(docids);
OmnScreen << "docids: " << docids.size() << " mAorB: " << mAorB->dataLen()/8 << endl; 
	aos_assert_r(mAorB->dataLen()/8 == docids.size(), false);
	for (u64 i=0; i< docids.size(); i++)
	{
		u64 docid = mAorB->getU64(0);
		if (docid != docids[i])
		{
			OmnAlarm << "docid: " << docid << " docids[" << i << "]: " << docids[i] << enderr; 
		}
	}
*/

	OmnScreen << "AorB" << endl;
	mOrBitmapRslt->reset();
	id = 0;
	nCount = mAorB->dataLen()/8;
	for(index=0; index<nCount; index++)
	{
		id = mOrBitmapRslt->nextDocid();
	    if((id == 0)) 
		{
			OmnScreen << "id == 0 or -1" << "index " << index << endl;
		    break;
		}

		u64 docid = mAorB->getU64(0);
		if (docid != id)
		{
		    OmnAlarm << "index " << index << "docid: " << docid << " id" << id << enderr;
			aos_assert_r(docid == id, false);
		}
	}
    aos_assert_r(index == nCount, false);

	/*
	aos_assert_r(mAnotB.size() == mNotInRsltVect.size(), false);
	for (u64 i=0; i< mAnotB.size(); i++)
	{
		aos_assert_r(mAnotB[i] == mNotInRsltVect[i], false);	
	}
	aos_assert_r(mOrAnd.size() == mOrAndRsltVect.size(), false);
	for (u64 i=0; i< mOrAnd.size(); i++)
	{
		aos_assert_r(mOrAnd[i] == mOrAndRsltVect[i], false);	
	}
*/
	return true;
}

bool
AosBitmapTester::initBlocks()
{
	mBlocks.clear();
	int block_num = rand() % 20 + 10;
	OmnScreen << "block_num : " << block_num << endl;
	for (int i=0; i<block_num; i++)
	{
		//OmnScreen << "block_index : " << i << endl;
		int per = rand() % 100;
		//OmnScreen << "per : " << per << endl;
		if (per < 20)
		{
			addBlock(99, 10);
		}
		else if (per < 40)
		{
			addBlock(999, 100);
		}
		else if (per < 60)
		{
			addBlock(9999, 1000);
		}
		else if (per < 80)
		{
			addBlock(99999, 10000);
		}
		else 
		{
			addBlock(99999999, 10000000);
		}
	}
	return true;
}

bool
AosBitmapTester::initRawData()
{
	mA = OmnNew AosBuff(100000000 AosMemoryCheckerArgs);
	mB = OmnNew AosBuff(100000000 AosMemoryCheckerArgs);
	mAandB = OmnNew AosBuff(100000000 AosMemoryCheckerArgs);
	mAorB = OmnNew AosBuff(100000000 AosMemoryCheckerArgs);
	mAnotB = OmnNew AosBuff(100000000 AosMemoryCheckerArgs);
	
	int a_rate = 0;
	int b_rate = 0;
	bool is_a, is_b;
	int a_count = 0;
	int b_count = 0;
	int ab_andCount = 0;
    int ab_orCount = 0;
	for (int i=0; i<(int)mBlocks.size(); i++)
	{
		a_rate = rand() % 100;
		b_rate = rand() % 100;
		for (u64 docid=mBlocks[i].start; docid<mBlocks[i].end; docid++)
		{
			if(docid == 0)
				continue;

if (docid == 0)
	OmnMark;
			is_a = false;
			is_b = false;
			aos_assert_r(docid >= 0, false);
			if (percent(a_rate)) 
			{
				is_a = true;
				if (mA->dataLen() + sizeof(u64) > mMaxBufferSize) 
				{
					OmnScreen<< " a_count : " << a_count << endl;
					return true; 
				}
				mA->setU64(docid);
				a_count++;
if(mA->dataLen() != a_count * 8)
	OmnMark;
if(((u64*)mA->data())[mA->dataLen()/8] != 0)
	OmnMark;

			}
			if (percent(b_rate)) 
			{
				is_b = true;
				if (mB->dataLen() + sizeof(u64) > mMaxBufferSize) 
				{
					if (is_a) mA->setDataLen(mA->dataLen()-sizeof(u64));
					OmnScreen<< " b_count : " << b_count << endl;
					return true;
				}
				mB->setU64(docid);
				b_count++;
			}
			if (is_a && is_b) 
			{
				if (mAandB->dataLen() + sizeof(u64) > mMaxBufferSize)
				{
					mA->setDataLen(mA->dataLen()-sizeof(u64));
					mB->setDataLen(mB->dataLen()-sizeof(u64));
					OmnScreen<< " ab_andCount : " << ab_andCount << endl;
					return true;
				}
				mAandB->setU64(docid);
				ab_andCount++;
			}
			if (is_a || is_b) 
			{
				if (mAorB->dataLen() + sizeof(u64) > mMaxBufferSize)
				{
					if (is_a) mA->setDataLen(mA->dataLen()-sizeof(u64));
					if (is_b) mB->setDataLen(mB->dataLen()-sizeof(u64));
					OmnScreen<< " ab_orCount : " << ab_orCount <<endl;
					return true;
				}
				mAorB->setU64(docid);
				ab_orCount++;
			}
			if (is_a && (!is_b)) 
			{
				if (mAnotB->dataLen() + sizeof(u64) > mMaxBufferSize)
				{
					if (is_a) mA->setDataLen(mA->dataLen()-sizeof(u64));
					if (is_b) mB->setDataLen(mB->dataLen()-sizeof(u64));
					return true;
				}
				mAnotB->setU64(docid);
			}
		}
	}

    OmnScreen<< " a_count : " << a_count << endl;
	OmnScreen<< " b_count : " << b_count << endl;
	OmnScreen<< " ab_andCount : " << ab_andCount << endl;
	OmnScreen<< " ab_orCount : " << ab_orCount <<endl;
	return true;
}

bool
AosBitmapTester::addBlock(const u64 &max_size, const u64 &min_size)
{
	// This function init a block([100, 1000))
	u64 start;
	if (mBlocks.size() == 0)
	{
		start = rand();
	}
	else
	{
		start = mBlocks[mBlocks.size()-1].end + rand();
	}

	u64 len = rand() % (max_size - min_size) + min_size;
	u64 end = start + len;
	if (isInBlocks(start, end))
	{
		OmnAlarm << "Inter error" << enderr;
		return false;
	}

	AosBlock block(start, end);
	mBlocks.push_back(block);
/*
	for (int i=0; i<mBlocks.size(); i++)
	{
OmnScreen << i << ": " << mBlocks[i].start << " : " << mBlocks[i].end << endl;
	}
	*/
	return true;
}

bool
AosBitmapTester::isInBlocks(const u64 &start, const u64 &end)
{
	if(mBlocks.size() == 0)
		return false;
    if (start <= mBlocks[mBlocks.size()-1].end) return true;
    return false;
}

bool
AosBitmapTester::percent(const int &per)
{
	return (rand() % 100 <= per);
}

