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
// Root stat doc is divided in two portions: 
// the key , total value, Value Index and Value Blocks . 
//
//modifyRootStatDoc
// This function assumes 'array' is an array of integers.
// If it is the root doc, its format is:

//		<index>
//			<index_header>
// 				[num_index]		8bytes
//			</index_header>
// 			<index_body>
// 				[start_idx, stat_docid]
// 				[start_idx, stat_docid]
// 					...
// 				[start_idx, stat_docid]
// 			</index_body>
// 		</index>
// 		<values>
//			[value]
//			[value]
//			...
//			[value]
//		</values>
// 		<total>		    8bytes
//
// 		<key_len>		4bytes
// 		<key>			AosIILUtil::eMaxStrValueLen	

//
// If it is not a root doc, its format is:
// 		[value]
// 		[value]
// 		...
// 		[value]
// This is a one-dimension array. Values are stored in either 
// a root doc or extension docs. 
// 1. It checks whether 'idx' is in the root doc. If yes, 
//    it adds 'value' to it.
// 2. If not, it uses the index to determine the docid of
//    the extension doc through 'ext_docid' and the relative
//    position inside the extension doc (through ext_slot_idx)
// 3. In addition, it checks whether the root doc needs to 
//    be pushed to an extension doc. If yes, 'kickout' is
//    set to true and 'kick_bucket' contains the contents
//    for the extension doc. 
//
// Modification History:
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/RootStatDoc.h"

#include "DataStructs/DataStructsUtil.h"
#include "DataStructs/StructProc.h"
#include "Util/DataTypes.h"
#include "Rundata/Rundata.h"

AosRootStatDoc::AosRootStatDoc()
{
}

AosRootStatDoc::~AosRootStatDoc()
{
}

AosRootStatDoc::AosRootStatDoc(
		char *data,
		const int64_t &data_len,
		const Parameter &parm,
		const u64 &statid,
		const OmnString &key)
:
mData(data),
mDataLen(data_len),
mIndexBodyData(0),
mNumIndex(0),
mGeneral(0),
mKey(key),
mStatId(statid),
mParm(parm)
{
	// batch add 
	mIndexBodyData = &mData[eIndexStartOff];
	mNumIndex = *(u64 *)&mData[eIndexNumOff];

	int64_t root_stat_doc_size = eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize;
	root_stat_doc_size += eGeneralValueLen + eKeyLenSize + eDftKeyLen;
	aos_assert(root_stat_doc_size <= mDataLen);
	int64_t other_pos = eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize;

	if (mNumIndex == 0 || mNumIndex == eInvNumIndex)
	{
		if (mParm.mDftValue == AosStructProc::eMinDftValue)
		{
			aos_assert(mNumIndex == eInvNumIndex);
		}
		else
		{
			aos_assert(mNumIndex == 0);
		}

		// clear index;
		mNumIndex = 0;	
		aos_assert(mParm.mIndexBodySize < mDataLen);
		memset(mIndexBodyData, 0, mParm.mIndexBodySize);

		mNumIndex ++;
		*(u64 *)&mData[eIndexNumOff] = mNumIndex;

		aos_assert(eIndexHeaderLen + sizeof(u64) + sizeof(u64) < (u64)mDataLen);
		setIndexRcd(0, 0, 0);

		// set total;
		mGeneral = 0;
		aos_assert(other_pos + eGeneralValueOff + sizeof(u64) < (u64)mDataLen);
		*(u64 *)&mData[other_pos + eGeneralValueOff] = mGeneral;

		other_pos += eGeneralValueLen;

		// set key len;
		aos_assert(mKey.length() <= eDftKeyLen);
		aos_assert(other_pos + eKeyLenOff + eKeyLenSize < (u64)mDataLen);
		*(u32 *)&mData[other_pos + eKeyLenOff] = mKey.length(); 

		// set key ;
		aos_assert(other_pos + eKeyOff + eDftKeyLen <= mDataLen);
		memset(&mData[other_pos + eKeyOff], 0, eDftKeyLen);

		aos_assert(other_pos + eKeyOff + mKey.length() <= mDataLen);
		memcpy(&mData[other_pos + eKeyOff], mKey.data(), mKey.length());
	}
	else
	{
		aos_assert(other_pos + eGeneralValueOff + eGeneralValueLen < mDataLen);
		mGeneral = *(u64 *)&mData[other_pos + eGeneralValueOff];	

		other_pos += eGeneralValueLen;
		aos_assert(other_pos + eKeyLenOff + eKeyLenSize <= mDataLen);
		u32 len = *(u32 *)&mData[other_pos + eKeyLenOff];

		aos_assert(other_pos + eKeyOff + len <= mDataLen);
		OmnString kk(&mData[other_pos + eKeyOff], len);
		aos_assert(mKey == kk);
	}
}


AosRootStatDoc::AosRootStatDoc(
		char *data,
		const int64_t &data_len,
		const Parameter &parm)
:
mData(data),
mDataLen(data_len),
mIndexBodyData(0),
mNumIndex(0),
mGeneral(0),
mKey(""),
mParm(parm)
{
	// read value
	mIndexBodyData = &mData[eIndexStartOff];
	mNumIndex = *(u64 *)&mData[eIndexNumOff];
	aos_assert(mNumIndex);

	int64_t root_stat_doc_size = eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize;
	root_stat_doc_size += eGeneralValueLen + eKeyLenSize + eDftKeyLen;
	aos_assert(root_stat_doc_size <= mDataLen);

	int64_t other_pos = eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize;
	aos_assert(other_pos < mDataLen);

	aos_assert(other_pos + eGeneralValueOff + eGeneralValueLen < mDataLen);
	mGeneral = *(u64 *)&mData[other_pos + eGeneralValueOff];	

	other_pos += eGeneralValueLen;

	aos_assert(other_pos + eKeyLenOff + eKeyLenSize < mDataLen);
	u32 len = *(u32 *)&mData[other_pos + eKeyLenOff];
	
	aos_assert(other_pos + eKeyOff + len <= mDataLen);
	OmnString key(&mData[other_pos + eKeyOff], len);
	mKey = key;
}


bool 
AosRootStatDoc::modifyRootStatDoc(
		const AosRundataPtr &rdata, 
		const u64 &idx, 
		const int64_t &value,
		const int vid,
		const AosStructProcObjPtr &proc, 
		bool &out_range)
{
	aos_assert_rr(mData, rdata, false);
	aos_assert_r(mParm.mIndexBodySize > 0 && mParm.mValuesSize > 0, false);
	aos_assert_r(mParm.mValuesSize + mParm.mIndexBodySize < mDataLen, false);
	out_range = false;

	setGeneralValue(proc, value);

	// 1. Find the block
	u64 start_idx = 0;
	u64 ext_docid = 0;
	u64 index_seqno = 0;
	bool rslt = findBlockIdxByIndex(
			idx, 
			ext_docid, 
			start_idx,
			index_seqno);
	aos_assert_r(rslt, false);

	if (ext_docid > 0 && ext_docid != eInvExtStatId)
	{
		mValueRslt.setExtInfo(ext_docid, idx - start_idx);
		return true;
	}

	if (ext_docid == eInvExtStatId)
	{
		//ext_docid = mParm.mStatIdExtIDGen->nextDocid(vid, rdata);
		ext_docid = AosDataStructsUtil::nextSysDocid(rdata, vid);
		modifyOldExtDocidByIndex(index_seqno, start_idx, ext_docid);
		mValueRslt.setExtInfo(ext_docid, idx - start_idx, true);

		//OmnScreen << "Linda Linda Linda kk:"
		//	<< ";modify:" << mStatId 
		//	<< ";start_idx:" << start_idx 
		//	<< ";" << endl;
		return true;
	}

	while (idx >= start_idx + mParm.mTimeBucketWeight)
	{
		start_idx = start_idx + mParm.mTimeBucketWeight;
		AosBuffPtr kick_bucket = kickoutBucket(
				rdata, 
				vid,
				start_idx, 
				ext_docid, out_range);

		if (!kick_bucket)
		{
			if (out_range)
			{
				return true;
			}
		}

		aos_assert_r(kick_bucket, false);
		if (ext_docid != eInvExtStatId)
		{
			mValueRslt.addKickInfo(ext_docid, kick_bucket);
			//OmnScreen << "Linda Linda kk:"
			//	<< ";start_idx:" << start_idx 
			//	<< ";" << endl;
		}
		else
		{
			//discard kick_bucket
		//	OmnScreen << "Linda Linda Discard!!! kk:"
		//		<< ";start_idx " << start_idx - mParm.mTimeBucketWeight
		//		<< ";"<< endl;
		}
	}

	aos_assert_r(idx - start_idx < mParm.mTimeBucketWeight, false);

	int64_t pos = eIndexHeaderLen + mParm.mIndexBodySize + (idx - start_idx) * AosDataType::getValueSize(mParm.mDataType);
	aos_assert_r(pos < eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize, false);
	return proc->setFieldValue(mData, mDataLen, pos, value, mParm.mDataType, mStatId);
}


AosBuffPtr
AosRootStatDoc::kickoutBucket(
		const AosRundataPtr &rdata,
		const int vid,
		const u64 &new_root_start_idx,
		u64 &old_root_extdid, 
		bool &out_range)
{
	// 1. get kick bucket buff and reset root stat doc;
	// 2. modify extension bucket index;
	// 3. modify root stat doc index, (start_idx , docid);
	out_range = false;
	sanityCheck();

	// copy data
	AosBuffPtr kick_bucket = getKickOutBucket();
	if (isValid(kick_bucket))
	{
		//old_root_extdid = mParm.mStatIdExtIDGen->nextDocid(vid, rdata);
		old_root_extdid = AosDataStructsUtil::nextSysDocid(rdata, vid);
	}
	else
	{
		old_root_extdid = eInvExtStatId;
	}

	bool rslt = setOldRootDocidByIndex(
			mNumIndex -1, 
			new_root_start_idx, 
			old_root_extdid);
	aos_assert_r(rslt, 0);
	
	//modify 'num_index' 
	mNumIndex ++;
	if (mNumIndex > mParm.mMaxNumIndex)
	{
		out_range = true;
		// OmnAlarm << "num_index > max_index" << enderr;
		return 0;
	}
	*(u64 *)&mData[eIndexNumOff] = mNumIndex;

	rslt = appendNewRootIndex(
			mNumIndex -1,
			new_root_start_idx);
	aos_assert_r(rslt, 0);

	return kick_bucket;
}


bool
AosRootStatDoc::setOldRootDocidByIndex(
		const u64 &index_seqno,
		const u64 &check_start_idx,
		const u64 &ext_docid)
{
	// modify extension bucket index 
	int64_t pos = index_seqno * eIndexRcdLen;
	aos_assert_r(pos + eIndexRcdLen <= mParm.mIndexBodySize, false);

	aos_assert_r(getIndexRcdDocid(pos) == 0, false);

	u64 startidx = getIndexRcdStartIdx(pos);
	aos_assert_r(startidx == check_start_idx - mParm.mTimeBucketWeight, false);

	setIndexRcd(pos, startidx, ext_docid);
	return true;

}


bool
AosRootStatDoc::modifyOldExtDocidByIndex(
		const u64 &index_seqno,
		const u64 &check_start_idx,
		const u64 &ext_docid)
{
	// modify extension bucket index 
	int64_t pos = index_seqno * eIndexRcdLen;
	aos_assert_r(pos + eIndexRcdLen <= mParm.mIndexBodySize, false);

	aos_assert_r(getIndexRcdDocid(pos) == eInvExtStatId, false);

	u64 startidx = getIndexRcdStartIdx(pos);
	aos_assert_r(startidx == check_start_idx, false);

	setIndexRcd(pos, startidx, ext_docid);
	return true;

}


AosBuffPtr
AosRootStatDoc::getKickOutBucket()
{
	int64_t size = mParm.mValuesSize;
	AosBuffPtr kick_bucket = OmnNew AosBuff(size AosMemoryCheckerArgs); 
	char * data = kick_bucket->data();
	memset(data, 0, kick_bucket->dataLen());

	int64_t values_pos = eIndexHeaderLen + mParm.mIndexBodySize;
	memcpy(data, &mData[values_pos], size);
	kick_bucket->setDataLen(size);

	//reset root stat doc
	//memset(&mData[values_pos], 0, size);
	memset(&mData[values_pos], mParm.mDftValue, size);
	return kick_bucket;
}


bool
AosRootStatDoc::isValid(const AosBuffPtr &kick_bucket)
{
	char *data = kick_bucket->data();
	int64_t data_len = kick_bucket->dataLen();
	char cmp_data[data_len];      
	//memset(cmp_data, 0, data_len);
	memset(cmp_data, mParm.mDftValue, data_len);
	if (memcmp(data, cmp_data, data_len) == 0)
	{
		return false;
	}
	return true;
}


bool
AosRootStatDoc::appendNewRootIndex(
		const u64 &index_seqno,
		const u64 &start_idx)
{
	// modify root stat doc, 'start_idx' and  'docid';
	int64_t pos = index_seqno * eIndexRcdLen;
	aos_assert_r(pos + eIndexRcdLen <= mParm.mIndexBodySize, false);

	aos_assert_r(getIndexRcdDocid(pos) == 0, false);
	aos_assert_r(getIndexRcdStartIdx(pos) == 0, false);

	setIndexRcd(pos, start_idx, 0);
	return true;
}


bool
AosRootStatDoc::findBlockIdxByIndex(
		const u64 &idx,
		u64 &ext_docid,
		u64 &start_idx,
		u64 &index_seqno)
{
	char * index_array = mIndexBodyData;
	index_seqno = findBoundary(index_array, mParm.mIndexBodySize, idx);
	return getIndexByIndexSeqno(index_seqno, ext_docid, start_idx);
}

bool
AosRootStatDoc::getIndexByIndexSeqno(
		u64 &index_seqno,
		u64 &ext_docid,
		u64 &start_idx)
{
	int64_t pos = index_seqno * eIndexRcdLen;
	aos_assert_r(pos + eIndexRcdLen <= mParm.mIndexBodySize, false);

	ext_docid = getIndexRcdDocid(pos);
	start_idx = getIndexRcdStartIdx(pos);
	return true;
}


u64
AosRootStatDoc::findBoundary(
		const char *array,
		const int64_t &data_len,
		const u64 &value)
{
	// This function assumes 'array' is an array of 'records'.
	// If it is the root doc, its format is:
	// 		[start_idx, stat_docid]
	// 		[start_idx, stat_docid]
	// 		...
	// 		[start_idx, stat_docid]
	//
	//		[value, value, ..., value]
	// example:
	// 		stat_idx    docid
	// 		0, 			8979
	// 		1000,		8888 
	// 		2000, 		7789
	// 		3000,		eInvExtStatId    //discard
	// 		4000,		0
	
	u64 idx = 0;
	for (u64 i = 0; i< mNumIndex; i++)
	{
		int64_t i_pos = i * eIndexRcdLen + eIndexRcdStartOff;
		if (*(u64 *)&array[i_pos] > value)   break;
		idx = i;
	}
	return idx;
}


void
AosRootStatDoc::setIndexRcd(
		const int64_t pos,
		const u64 &start_idx,
		const u64 &docid)
{
	*(u64 *)&mIndexBodyData[pos + eIndexRcdStartOff] = start_idx;
	*(u64 *)&mIndexBodyData[pos + eIndexRcdDocidOff] = docid;
}


void
AosRootStatDoc::setGeneralValue(const AosStructProcObjPtr &proc,  const int64_t &value)
{
	mGeneral = proc->calculateGeneralValue(value, mGeneral);
	int64_t pos = eIndexHeaderLen + mParm.mIndexBodySize + mParm.mValuesSize;
	*(u64 *)&mData[pos + eGeneralValueOff] = mGeneral;
}


u64 
AosRootStatDoc::getIndexRcdStartIdx(const int64_t &pos)
{
	return *(u64 *)&mIndexBodyData[pos + eIndexRcdStartOff];
}

u64 
AosRootStatDoc::getIndexRcdDocid(const int64_t pos)
{
	return *(u64 *)&mIndexBodyData[pos + eIndexRcdDocidOff];
}


bool 
AosRootStatDoc::getStatDocIndex(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		const u64 &idx, 
		u64 &ext_docid,
		u64 &start_idx)
{
	aos_assert_r(mParm.mIndexBodySize > 0, false);

	// 1. Find the block
	u64 index_seqno;
	bool rslt = findBlockIdxByIndex(
			idx, 
			ext_docid, 
			start_idx,
			index_seqno);
	aos_assert_r(rslt, false);

	if (ext_docid == eInvExtStatId)
	{
		char cmp_data[mParm.mValuesSize];      
		//memset(cmp_data, 0, mParm.mValuesSize);
		memset(cmp_data, mParm.mDftValue, mParm.mValuesSize);
		buff->setBuff(cmp_data, mParm.mValuesSize);
		return true;
	}

	if (ext_docid > 0) return true;
	
	if (idx >= start_idx + mParm.mTimeBucketWeight) 
	{
		//print(mIndexBodyData, mParm.mIndexBodySize, mNumIndex);
		return false;
	}

	int64_t values_pos = eIndexHeaderLen + mParm.mIndexBodySize;
	aos_assert_r(values_pos + mParm.mValuesSize < mDataLen, false);

	buff->setBuff(&mData[values_pos], mParm.mValuesSize);
	return true;
}


bool 
AosRootStatDoc::getAllStatDocIndex(
		const AosRundataPtr &rdata,
		vector<u64> &ext_docids,
		vector<u64> &start_idxs)
{
	aos_assert_r(mParm.mIndexBodySize > 0, false);

	bool rslt = false;
	u64 ext_docid = 0;
	u64 start_idx = 0;
	for (u64 i = 0; i < mNumIndex; i++)
	{
		rslt = getIndexByIndexSeqno(i, ext_docid, start_idx);
		aos_assert_r(rslt, false);
		ext_docids.push_back(ext_docid);
		// Ketty 2014/07/07
		//start_idxs.push_back(ext_docid);
		start_idxs.push_back(start_idx);
	}
	aos_assert_r(ext_docids.size() == start_idxs.size(), false);
	return true;
}


bool
AosRootStatDoc::getRootStatDoc(const AosBuffPtr &buff)
{
	int64_t values_pos = eIndexHeaderLen + mParm.mIndexBodySize;
	aos_assert_r(values_pos + mParm.mValuesSize < mDataLen, false);

	buff->setBuff(&mData[values_pos], mParm.mValuesSize);
	return true;
}


bool
AosRootStatDoc::sanityCheck()
{
	int64_t data_len = mParm.mIndexBodySize;

//print(mIndexBodyData, data_len, mNumIndex);
	u64 num = 0;
	int64_t crt_idx = 0;
	while (crt_idx < data_len)
	{
		u64 ext_docid = getIndexRcdDocid(crt_idx);
		u64 start_idx = getIndexRcdStartIdx(crt_idx);

		if (start_idx != 0 || crt_idx == 0)
		{
			if (num == mNumIndex -1)
			{
				aos_assert_r(ext_docid == 0, false);
			}
			else
			{
				aos_assert_r(ext_docid != 0, false);
			}
			num ++;
		}
		else
		{
			aos_assert_r(ext_docid == 0, false);
		}
		crt_idx += eIndexRcdLen;
	}
	aos_assert_r(num == mNumIndex, false);
	return true;
}


void
AosRootStatDoc::print(
		const char *array,
		const int64_t &data_len,
		const u64 &num_index)
{
	OmnScreen << "----------start------------" << endl;
	OmnScreen << "EEE docid:" << mStatId << ";" << endl;
	for (u32 i = 0; i< num_index; i++)
	{
		int64_t pos = i * eIndexRcdLen + eIndexRcdStartOff;
		aos_assert(pos < data_len);
		OmnScreen << "start_idx:"<<*(u64 *)&array[pos + eIndexRcdStartOff] << "; ext_docid:" << *(u64 *)&array[pos + eIndexRcdDocidOff] << ";"<< endl;
	}
	OmnScreen << "---------end---------------" << endl;
}

