///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/04/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomBuffArray.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Random/CommonValues.h"
#include "Rundata/Rundata.h"
#include "Util/BuffArray.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"



AosRandomBuffArray::AosRandomBuffArray(
		const int record_len)
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mCrtDocid(eInitDocid),
mRecordLength(record_len),
mNumSeeds(eDftNumSeeds),
mSeedMinLen(eSeedMinLen),
mSeedMaxLen(eSeedMaxLen),
mCrtSeedPos(0),
mCrtDocidPos(0)
{
	if (!init())
	{
		OmnThrowException("Failed init");
		return;
	}
}


AosRandomBuffArray::~AosRandomBuffArray()
{
}


bool
AosRandomBuffArray::init()
{
	aos_assert_r(mRecordLength > 0, false);

//	int64_t seed_buff_len = mSeedMaxLen * mNumSeeds;
//	AosCompareFunPtr comp_func = OmnNew AosFunStrU641(mSeedMaxLen);
//	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(comp_func, false, true, seed_buff_len);
	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(mSeedMaxLen);
	for (int i=0; i<mNumSeeds; i++)
	{
		OmnString ss = OmnRandom::nextLetterDigitStr(mSeedMinLen, mSeedMaxLen-1, true);
		buff_array->appendEntry(ss.data(), ss.length(), mRundata.getPtrNoLock());
	}

	// mSeeds are sorted. 
	buff_array->sort();
	AosBuffPtr buff = buff_array->getBuff();
	aos_assert_r(buff, false);
	char *data = buff->data();
	for (int i=0; i<mNumSeeds; i++)
	{
		mSeeds.push_back(&data[i*mSeedMaxLen]);
	}

	mDocids = OmnNew vector<u64>[mNumSeeds];
	return true;
}


u64
AosRandomBuffArray::getNewDocid()
{
	return mCrtDocid++;
}


AosBuffPtr
AosRandomBuffArray::createNRecordToBuff(
		const u64 &num_record,
		AosRundata *rdata)
{	
	// This function creates 'num_record' number of records. 
	// There are a number of seeds. Each seed keeps track of
	// the docids being created for the seed:
	// 	Seed 0:	docid, docid, ..., docid
	// 	Seed 1:	docid, docid, ..., docid
	//
	// It randomly picks a seed, append a docid to that seed's docid array. 
	// After all the docids were created, it then loops over all the seeds
	// in the order in which seeds are defined. For each seed, it checks
	// whether docids were added to it. If yes, it creates all the entries
	// for the seed. This guarantees that entries are created in order.
	//
	aos_assert_r(mNumSeeds > 0,0);
//	AosCompareFunPtr comp_func = OmnNew AosFunStrU641(mRecordLength);
//	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(
//			comp_func, true, false, (num_record+1) * mRecordLength);

	AosBuffArrayPtr buff_array = OmnNew AosBuffArray(mRecordLength);
	// create num_record records
	vector<u64> *records = OmnNew vector<u64>[mNumSeeds];
	
	int crt_seed = 0;
	for (u64 i=0; i<num_record; i++)
	{
		// create a record
		crt_seed = random()%mNumSeeds;
		records[crt_seed].push_back(getNewDocid());
	}
	
	// put all the records into buff_array and mDocids
	u64 j = 0;
	OmnString value_str;
	for(int i = 0;i < mNumSeeds;i++)
	{
		if (records[i].size()> 0)
		{
			for (j=0; j<records[i].size(); j++)
			{
				//1. put into buff_array. The value format is:
				//		<seed> + "_" + seqno + "_" + docid
				OmnString seed = mSeeds[i];
				int seqno = (int)mDocids[i].size();
				u64 docid = records[i][j];
				value_str = constructEntry(seed, seqno, docid);
				buff_array->appendEntry(value_str.data(), value_str.length(), docid, rdata);

				//2. put into mDocids
				mDocids[i].push_back(docid);
			}
		}
	}	

	if(records)
	{
		delete []records;
		records = 0;
	}
	mTotalSize += num_record;

	// Chen Ding, 2013/05/13
	//buff_array->sort();
	return buff_array->getBuff();
}


OmnString	
AosRandomBuffArray::getSeed(const int idx, int &record_len)
{
	record_len = mRecordLength;
	return mSeeds[idx];
}


OmnString	
AosRandomBuffArray::getRecordsBySeeds(const int idx,vector<u64>* &vector_ptr)
{
	vector_ptr = &(mDocids[idx]);
	return mSeeds[idx];
}


OmnString 
AosRandomBuffArray::constructEntry(
		const OmnString &seed, 
		const int seqno, 
		const u64 docid)
{
	// Entries are constricted as:
	// 		seed + "_" + seqno + "_" + docid
	// where 'seqno' is the index of the docid array at which 'docid' 
	// locates. As an example:
	// 	Seed i:	docid0, docid1, docid2, docid3, docid4, docid5
	// Its values are:
	// 		seed + "_" + 00000000 + "_" + docid0
	// 		seed + "_" + 00000001 + "_" + docid1
	// 		seed + "_" + 00000002 + "_" + docid2
	// 		...
	OmnString value_str = seed;
	char buff[100];
	sprintf(buff, "%08d", seqno);
	value_str << "_" << buff << "_" << docid;
	return value_str;
}


bool 
AosRandomBuffArray::getStartPosGT(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getStartPosGreater(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	if (mDocids[seed_idx].size() == 0) return true;

	// Found the starting seed index. Starting from the start
	// position for that seed, it finds the first entry:
	// 		value > mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=size-1; i>=0; i--)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value <= ss)
		{
			if (i == size-1)
			{
				if (seed_idx >= mNumSeeds-1)
				{
					seed_idx = -1;
					return true;
				}

				seed_idx++;
				entry_idx = 0;
				return true;
			}
			entry_idx = i+1;
			return true;
		}
	}

	entry_idx = 0;
	return true;
}


bool 
AosRandomBuffArray::getStartPosGE(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getStartPosGreater(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the start
	// position for that seed, it finds the first entry:
	// 		value >= mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=0; i<size; i++)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value <= ss)
		{
			entry_idx = i;
			return true;
		}

		/*
			if (i == size-1)
			{
				if (seed_idx >= mNumSeeds-1)
				{
					seed_idx = -1;
					return true;
				}

				seed_idx++;
				entry_idx = 0;
				return true;
			}
			entry_idx = i+1;
			return true;
		*/
	}

	if (seed_idx == mNumSeeds-1)
	{
		seed_idx = -1;
	}
	else
	{
		seed_idx++;
	}
	return true;
}


bool 
AosRandomBuffArray::getStartPosEQ(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getStartPosGreater(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the start
	// position for that seed, it finds the first entry:
	// 		value >= mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=size-1; i>=0; i++)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value == ss)
		{
			entry_idx = i;
			return true;
		}

		if (value < ss)
		{
			seed_idx = -1;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


bool 
AosRandomBuffArray::getStartPosLT(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	seed_idx = -1;
	for (int i=0; i<mNumSeeds; i++)
	{
		if (mDocids[i].size() > 0)
		{
			seed_idx = i;
			break;
		}
	}

	if (seed_idx < 0) return true;

	OmnString ss = constructEntry(mSeeds[seed_idx], 0, mDocids[seed_idx][0]);
	// 'ss' is the smallest entry.
	if (ss >= value)
	{
		seed_idx = -1;
		return true;
	}
	entry_idx = 0;
	return true;
}


bool 
AosRandomBuffArray::getStartPosLE(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	seed_idx = -1;
	for (int i=0; i<mNumSeeds; i++)
	{
		if (mDocids[i].size() > 0)
		{
			seed_idx = i;
			break;
		}
	}

	if (seed_idx < 0) return true;

	OmnString ss = constructEntry(mSeeds[seed_idx], 0, mDocids[seed_idx][0]);
	// 'ss' is the smallest entry.
	if (ss > value)
	{
		seed_idx = -1;
		return true;
	}
	entry_idx = 0;
	return true;
}


bool
AosRandomBuffArray::getStartPosGreater(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx) 
{
	// This function finds the first seed:
	// 		value <= mSeeds[seed_idx]
	seed_idx = -1;

	int left = 0; 
	int right = mNumSeeds-1;
	while (left < right)
	{
		int nn = (left + right) / 2;

		if (strncmp(value.data(), mSeeds[nn].data(), mSeeds[nn].length()) <= 0)
		{
			right = nn;
		}
		else
		{
			// value > mSeeds[nn]
			left = nn+1;
		}
	}

	seed_idx = left;
	while (seed_idx < mNumSeeds && mDocids[seed_idx].size() == 0) seed_idx++;
	if (seed_idx >= mNumSeeds) seed_idx = -1;
	return true;
}


bool
AosRandomBuffArray::getEndPosGreater(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx) 
{
	// This function finds the last seed:
	// 		value >= mSeeds[seed_idx]
	int left = 0; 
	int right = mNumSeeds-1;

	seed_idx = -1;
	if (value < mSeeds[0]) return true;

	while (left < right)
	{
		int nn = (left + right) / 2;
		if (left == right)
		{
			aos_assert_r(value >= mSeeds[left], false);
			seed_idx = left;
			return true;
		}

		if (value >= mSeeds[nn])
		{
			left = nn;
		}
		else
		{
			// value < mSeeds[nn]
			right = nn-1;
		}
	}

	// left == right
	seed_idx = left;
	return true;
}


bool 
AosRandomBuffArray::getStartPosLess(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx) 
{
	// This function finds the first seed:
	// 		value <= mSeeds[seed_idx]
	int left = 0; 
	int right = mNumSeeds-1;

	seed_idx = -1;
	if (value > mSeeds[mNumSeeds-1]) return true;

	while (left < right)
	{
		int nn = (left + right) / 2;
		if (left == right)
		{
			aos_assert_r(value <= mSeeds[left], false);
			seed_idx = left;
			return true;
		}

		if (value <= mSeeds[nn])
		{
			left = nn;
		}
		else
		{
			// value > mSeeds[nn]
			right = nn-1;
		}
	}

	// left == right
	seed_idx = left;
	return true;
}


bool 
AosRandomBuffArray::getEndPosLess(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx) 
{
	// This function finds the last seed:
	// 		mSeeds[seed_idx] <= value
	int left = 0; 
	int right = mNumSeeds-1;

	seed_idx = -1;
	while (left < right)
	{
		int nn = (left + right) / 2;

		if (mSeeds[nn] <= value)
		{
			if (left + 1 == right)
			{
				if (mSeeds[nn+1] <= value)
				{
					left++;
				}
				break;
			}
			left = nn;
		}
		else
		{
			right = nn-1;
		}
	}

	aos_assert_r(left == right || left + 1 == right, false);
	while (left >= 0 && mDocids[left].size() == 0) left--;
	seed_idx = left;
	return true;
}


bool 
AosRandomBuffArray::getStartPosNE(
		AosRundata *rdata, 
		const OmnString &value,
		int &start_seed_idx, 
		int &start_entry_idx)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosRandomBuffArray::getStartPosAN(
		AosRundata *rdata, 
		const OmnString &value,
		int &start_seed_idx, 
		int &start_entry_idx)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosRandomBuffArray::getStartPos(
		AosRundata *rdata, 
		const AosOpr	&opr,
		const OmnString &value,
		int &start_seed_idx, 
		int &start_entry_idx)
{
	switch (opr)
	{
	case eAosOpr_gt:
		 return getStartPosGT(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_ge:
		 return getStartPosGE(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_eq:
		 return getStartPosEQ(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_lt:
		 return getStartPosLT(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_le:
		 return getStartPosLE(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_ne:
		 return getStartPosNE(rdata, value, start_seed_idx, start_entry_idx);

	case eAosOpr_an:
		 return getStartPosAN(rdata, value, start_seed_idx, start_entry_idx);

	// The following are not implementedYet;
	case eAosOpr_Objid:
	case eAosOpr_prefix:
	case eAosOpr_like:
	// case eAosOpr_range:		Chen Ding, CHENDING20130920
	case eAosOpr_date:
	case eAosOpr_epoch:
	case eAosOpr_range_ge_le:
	case eAosOpr_range_ge_lt:
	case eAosOpr_range_gt_le:
	case eAosOpr_range_gt_lt:
	     OmnNotImplementedYet;
		 return false;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosRandomBuffArray::getEndPos(
		AosRundata *rdata, 
		const AosOpr	&opr,
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx)
{
	switch (opr)
	{
	case eAosOpr_gt:
		 return getEndPosGT(rdata, value, seed_idx, entry_idx);

	case eAosOpr_ge:
		 return getEndPosGE(rdata, value, seed_idx, entry_idx);

	case eAosOpr_eq:
		 return getEndPosEQ(rdata, value, seed_idx, entry_idx);

	case eAosOpr_lt:
		 return getEndPosLT(rdata, value, seed_idx, entry_idx);

	case eAosOpr_le:
		 return getEndPosLE(rdata, value, seed_idx, entry_idx);

	case eAosOpr_ne:
		 return getEndPosNE(rdata, value, seed_idx, entry_idx);

	case eAosOpr_an:
		 return getEndPosAN(rdata, value, seed_idx, entry_idx);

	// The following are not implementedYet;
	case eAosOpr_Objid:
	case eAosOpr_prefix:
	case eAosOpr_like:
	// case eAosOpr_range:		Chen Ding, CHENDING20130920
	case eAosOpr_date:
	case eAosOpr_epoch:
	case eAosOpr_range_ge_le:
	case eAosOpr_range_ge_lt:
	case eAosOpr_range_gt_le:
	case eAosOpr_range_gt_lt:
	     OmnNotImplementedYet;
		 return false;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosRandomBuffArray::query(
		AosRundata *rdata,
		const AosOpr	&opr1,
		const OmnString &value1,
		const AosOpr	&opr2,
		const OmnString &value2,
		AosBitmapObjPtr &bitmap)
{
	// This function retrieves all the entries that meet the condition:
	// 		[value1, opr1] and [value2, opr2]
	int start_seed_idx, start_entry_idx, end_seed_idx, end_entry_idx;
	bool rslt = getStartPos(rdata, opr1, value1, start_seed_idx, start_entry_idx);
	aos_assert_r(rslt, false);
	if (bitmap) bitmap->clear();
	if (start_seed_idx < 0) return true;

	rslt = getEndPos(rdata, opr2, value2, end_seed_idx, end_entry_idx);
	aos_assert_r(rslt, false);
	aos_assert_r(end_seed_idx >= 0 && end_seed_idx < mNumSeeds, false);
	aos_assert_r(end_entry_idx >= 0 && (u32)end_entry_idx < mDocids[end_seed_idx].size(), false);
	aos_assert_r(start_seed_idx <= end_seed_idx, false);

	if (!bitmap) bitmap = AosGetBitmap();
	aos_assert_r(bitmap, false);

	int entry_end = -1;
	int seed_idx = start_seed_idx;
	int entry_start = start_entry_idx;
	while (seed_idx <= end_seed_idx)
	{
		if (mDocids[seed_idx].size() == 0) 
		{
			seed_idx++;
			continue;
		}

		if (start_seed_idx == end_seed_idx)
		{
			entry_start = start_entry_idx;
			entry_end = end_entry_idx;
			seed_idx = start_seed_idx;
			aos_assert_r(entry_end >= 0 && (u32)entry_end < mDocids[seed_idx].size(), false);
			entry_end++;
		}
		else if (seed_idx == start_seed_idx)
		{
			entry_start = start_entry_idx;
			entry_end = (int)mDocids[seed_idx].size();
		}
		else if (seed_idx < end_seed_idx)
		{
			entry_start = 0;
			entry_end = (int)mDocids[seed_idx].size();
		}
		else
		{
			entry_start = 0;
			entry_end = end_entry_idx;
			aos_assert_r(entry_end >= 0 && (u32)entry_end < mDocids[seed_idx].size(), false);
			entry_end++;
		}

		for (int idx = entry_start; idx < entry_end; idx++)
		{
			// OmnScreen << "Append: " << mDocids[seed_idx][idx] << endl;
			bitmap->appendDocid(mDocids[seed_idx][idx]);
			aos_assert_r(bitmap->checkDoc(mDocids[seed_idx][idx]), false);
		}

		seed_idx++;
	}

	return true;
}


bool
AosRandomBuffArray::query(
		AosRundata *rdata,
		const AosOpr	&opr1,
		const OmnString &value1,
		const AosOpr	&opr2,
		const OmnString &value2,
		const u64		&start_idx,
		const u64		&page_size,
		vector<OmnString>  &values,
		vector<u64>     &docids)
{
	// This function retrieves all the entries that meet the condition:
	// 		[value1, opr1] and [value2, opr2]
	int start_seed_idx, start_entry_idx, end_seed_idx, end_entry_idx;
	bool rslt = getStartPos(rdata, opr1, value1, start_seed_idx, start_entry_idx);
	aos_assert_r(rslt, false);
	if (start_seed_idx < 0) return true;

	values.clear();
	docids.clear();
	rslt = getEndPos(rdata, opr2, value2, end_seed_idx, end_entry_idx);
	aos_assert_r(rslt, false);
	aos_assert_r(end_seed_idx >= 0 && end_seed_idx < mNumSeeds, false);
	aos_assert_r(end_entry_idx >= 0 && (u32)end_entry_idx < mDocids[end_seed_idx].size(), false);
	aos_assert_r(start_seed_idx <= end_seed_idx, false);

	int entry_end = -1;
	int seed_idx = start_seed_idx;
	int entry_start = start_entry_idx;
	u64 num_skipped = 0;
	while (seed_idx <= end_seed_idx)
	{
		if (mDocids[seed_idx].size() == 0) 
		{
			seed_idx++;
			continue;
		}

		if (start_seed_idx == end_seed_idx)
		{
			entry_start = start_entry_idx;
			entry_end = end_entry_idx;
			seed_idx = start_seed_idx;
			aos_assert_r(entry_end >= 0 && (u32)entry_end < mDocids[seed_idx].size(), false);
			entry_end++;
		}
		else if (seed_idx == start_seed_idx)
		{
			entry_start = start_entry_idx;
			entry_end = (int)mDocids[seed_idx].size();
		}
		else if (seed_idx < end_seed_idx)
		{
			entry_start = 0;
			entry_end = (int)mDocids[seed_idx].size();
		}
		else
		{
			entry_start = 0;
			entry_end = end_entry_idx;
			aos_assert_r(entry_end >= 0 && (u32)entry_end < mDocids[seed_idx].size(), false);
			entry_end++;
		}

		for (int idx = entry_start; idx < entry_end; idx++)
		{
			if (num_skipped < start_idx)
			{
				num_skipped++;
			}
			else
			{
				OmnString ss = constructEntry(mSeeds[seed_idx], idx, mDocids[seed_idx][idx]);
				values.push_back(ss);
				docids.push_back(mDocids[seed_idx][idx]);
				if (docids.size() == page_size) return true;
			}
		}

		seed_idx++;
	}
	return true;
}
						

bool 
AosRandomBuffArray::getEndPosLT(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getEndPosLess(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the start
	// position for that seed, it goes back until it finds the
	// first first entry:
	// 		value < mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=0; i<size; i++)
	{
		OmnString ss = constructEntry(mSeeds[i], i, mDocids[seed_idx][i]);
		if (value >= ss)
		{
			// This is the first entry that does not meet the condition.
			if (i == 0)
			{
				seed_idx--;
				return true;
			}

			entry_idx = i-1;
			return true;
		}
	}

	entry_idx = size-1;
	return true;
}


bool 
AosRandomBuffArray::getEndPosLE(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getEndPosLess(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the start
	// position for that seed, it goes back until it finds the
	// first first entry:
	// 		value < mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=size-1; i>=0; i--)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (ss <= value)
		{
			entry_idx = i;
			return true;
		}
	}

	seed_idx--;
	return true;
}


bool 
AosRandomBuffArray::getEndPosEQ(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getEndPosLess(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the start
	// position for that seed, it goes back until it finds the
	// first first entry:
	// 		value < mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=0; i<size; i++)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value == ss)
		{
			entry_idx = i;
			return true;
		}

		if (value < ss)
		{
			seed_idx = -1;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


bool 
AosRandomBuffArray::getEndPosGT(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getEndPosGreater(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the end, 
	// it goes back until it finds the first first entry:
	// 		value > mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=size-1; i>=0; i--)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value > ss)
		{
			entry_idx = i;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


bool 
AosRandomBuffArray::getEndPosGE(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	bool rslt = getEndPosGreater(rdata, value, seed_idx);
	aos_assert_r(rslt, false);
	if (seed_idx < 0) return true;
	aos_assert_r(seed_idx >= 0 && seed_idx < mNumSeeds, false);
	aos_assert_r(mDocids[seed_idx].size() > 0, false);

	// Found the starting seed index. Starting from the end, 
	// it goes back until it finds the first first entry:
	// 		value >= mSeeds[seed_idx]
	// If no entries meet the above condition, return the next seed idx.
	u32 size = mDocids[seed_idx].size();
	for (u32 i=size-1; i>=0; i--)
	{
		OmnString ss = constructEntry(mSeeds[seed_idx], i, mDocids[seed_idx][i]);
		if (value >= ss)
		{
			entry_idx = i;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


bool 
AosRandomBuffArray::getEndPosNE(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	for (int i=mNumSeeds-1; i>=0; i--)
	{
		if (mDocids[i].size() > 0) 
		{
			seed_idx = i;
			entry_idx = mDocids[i].size()-1;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


bool 
AosRandomBuffArray::getEndPosAN(
		AosRundata *rdata, 
		const OmnString &value,
		int &seed_idx, 
		int &entry_idx) 
{
	for (int i=mNumSeeds-1; i>=0; i--)
	{
		if (mDocids[i].size() > 0) 
		{
			seed_idx = i;
			entry_idx = mDocids[i].size()-1;
			return true;
		}
	}

	seed_idx = -1;
	return true;
}


// Chen Ding, 2013/09/21
bool 
AosRandomBuffArray::query(			
		AosRundata *rdata, 
		OmnString &start_value,
		OmnString &end_value, 
		AosBitmapObjPtr &bitmap)
{
	// This function randomly picks a range:
	// 		[start_value, end_value]
	// and returns the bitmap.
	bitmap = AosGetBitmap();
	while (1)
	{
		aos_assert_rr(mSeeds.size() > 0, rdata, false);
		int idx1 = rand() % mSeeds.size();
		int idx2 = rand() % mSeeds.size();
		if (idx1 > idx2)
		{
			int dd = idx1;
			idx1 = idx2;
			idx2 = dd;
		}

		aos_assert_r(idx1 >= 0, false);
		aos_assert_r(idx2 >= 0, false);

		// Make sure idx1 is not empty
		while (idx1 >= 0 && mDocids[idx1].size() == 0) idx1--;
		if (idx1 < 0) continue;

		// Make sure idx2 is not empty
		while ((u32)idx2 < mSeeds.size() && mDocids[idx2].size() == 0) idx1++;
		if ((u32)idx2 >= mSeeds.size()) continue;

		if (idx1 == idx2)
		{
			int nn1 = rand() % mDocids[idx1].size();
			int nn2 = rand() % mDocids[idx1].size();
			if (nn1 > nn2)
			{
				int nn = nn1;
				nn1 = nn2;
				nn2 = nn;
			}

			start_value = constructEntry(mSeeds[idx1], nn1, mDocids[idx1][nn1]);
			end_value   = constructEntry(mSeeds[idx1], nn2, mDocids[idx1][nn2]);
			for (int i=nn1; i<=nn2; i++)
			{
				bitmap->appendDocid(mDocids[idx1][i]);
			}
			return true;
		}

		// Handle the first one
		u32 nn = rand() % mDocids[idx1].size();
		start_value = constructEntry(mSeeds[idx1], nn, mDocids[idx1][nn]);
		for (u32 i=nn; i<mDocids[idx1].size(); i++)
		{
			bitmap->appendDocid(mDocids[idx1][i]);
		}

		// Handle the last one
		nn = rand() % mDocids[idx2].size();
		end_value = constructEntry(mSeeds[idx2], nn, mDocids[idx2][nn]);
		for (u32 i=0; i<=nn; i++)
		{
			bitmap->appendDocid(mDocids[idx2][i]);
		}

		// Handle the rest
		for (int i=idx1+1; i<=idx2-1; i++)
		{
			for (u32 j=0; j<mDocids[i].size(); j++)
			{
				bitmap->appendDocid(mDocids[i][j]);
			}
		}
		return true;
	}	

	return true;
}

