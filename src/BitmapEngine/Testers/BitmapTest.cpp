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
// 2013/02/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapEngine/Testers/BitmapTest.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BitmapUtil/BitmapUtil.h"
#include "Porting/Sleep.h"
#include "QueryRslt/Ptrs.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "BitmapMgr/BitmapMgr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"
#include "Util/File.h"


AosBitmapTest::AosBitmapTest()
:
mBitmapId(0),
mSectionId(0),
mNodeLevel(-1),
mIILLevel(-1)
{
	AosFunU64U641Ptr comp_func = OmnNew AosFunU64U641();
}


AosBitmapTest::~AosBitmapTest()
{
}


bool 
AosBitmapTest::checkDoc(const u64 docid) const
{	
	set<u64>::iterator itr = mData.find(docid);
	return itr != mData.end();
}


void 
AosBitmapTest::appendDocid(const u64 docid)
{
	mData.insert(docid);
}


void 
AosBitmapTest::removeDocid(const u64 docid)
{
	mData.erase(docid);
}


void
AosBitmapTest::saveToBuff(const AosBuffPtr &buff)
{
}


bool	
AosBitmapTest::loadFromBuff(const AosBuffPtr &buff)
{
	return true;
}
	

u64 
AosBitmapTest::nextDocid()
{	
	if (mItr == mData.end()) return 0;
	u64 docid = *mItr;
	mItr++;
	return docid;
}


void 
AosBitmapTest::clean()
{
	mData.clear();
}


AosBitmapObjPtr
AosBitmapTest::getBitmap()
{
	return OmnNew AosBitmapTest();
}


bool
AosBitmapTest::isEmpty()
{
	return mData.size() == 0;
}


bool 
AosBitmapTest::compareBitmap(const AosBitmapObjPtr &bitmap)
{
	reset();
	bitmap->reset();
	while (1)
	{
		u64 docid1 = nextDocid();
		u64 docid2 = bitmap->nextDocid();
		if (docid1 == 0)
		{
			aos_assert_r(docid2 == 0, false);
			return true;
		}

		if (docid1 != docid2)
		{
			OmnString ss;
			ss << "Bitmap Test\n";
			reset();
			bitmap->reset();
			while (1)
			{
				docid1 = nextDocid();
				docid2 = bitmap->nextDocid();
				ss << docid1 << "\t" << docid2 << "\n";
				if (docid1 == 0 && docid2 == 0) break;
			}

			OmnFile ff("tmp123456", OmnFile::eWriteCreate AosMemoryCheckerArgs);
			aos_assert_r(ff.isGood(), false);
			ff.put(0, ss.data(), ss.length(), true);
			OmnAlarm << "Invalid: " << enderr;
			return false;
		}
	}
	return true;
}


u64 
AosBitmapTest::getMemorySize()
{
	OmnNotImplementedYet;
	return 0;
}

	
void 
AosBitmapTest::appendBlock(const u64* docs, const int num_docs)
{
	OmnNotImplementedYet;
}

	
bool 
AosBitmapTest::getSections(
	   	const AosRundataPtr &rdata,
		map_t &bitmaps)
{
	bitmaps.clear();
	if (mData.size() <= 0) return true;

	set<u64>::iterator itr = mData.begin();
	while (itr != mData.end())
	{
		u64 docid = *itr;
		u16 section_id = AosBitmapUtil::getSectionId(docid);
		mapitr_t tt = bitmaps.find(section_id);
		if (tt != bitmaps.end())
		{
			AosBitmapObjPtr bb = tt->second;
			bb->appendDocid(docid);
		}
		else
		{
			AosBitmapTest* bb = OmnNew AosBitmapTest();
			bb->appendDocid(docid);
			bitmaps[section_id] = bb;
		}
	}

	return true;
}


bool 
AosBitmapTest::orBitmap(const AosBitmapObjPtr &bitmap)
{
	aos_assert_r(bitmap, false);
	bitmap->reset();
	u64 docid;
	set<u64>::iterator itr;
	while ((docid = bitmap->nextDocid()))
	{
		mData.insert(docid);
	}
	return true;
}


bool 
AosBitmapTest::andBitmap(const AosBitmapObjPtr &bitmap)
{
	OmnNotImplementedYet;
	return false;
}


void 
AosBitmapTest::returnBitmap(const AosBitmapObjPtr &bitmap)
{
	OmnNotImplementedYet;
}


AosBitmapObjPtr 
AosBitmapTest::createBitmap(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosBitmapTest::removeBits(const AosBitmapObjPtr &bitmap)
{
	bitmap->reset();
	u64 docid;

	while ((docid = bitmap->nextDocid()))
	{
		removeDocid(docid);
	}

	return true;
}


u64 
AosBitmapTest::nextNDocid(u64 *list,int &count)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosBitmapTest::getDocids(vector<u64> &docids)
{
	OmnNotImplementedYet;
	return false;
}


u64  
AosBitmapTest::getDocidCount()
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosBitmapTest::splitByPhysical(
		vector<AosBitmapObjPtr> &bitmaps,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

