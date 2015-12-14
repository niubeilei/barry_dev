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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataShuffler/DocidShufflerProc.h"

#include "Util/DataRecord.h"
#include "Rundata/Rundata.h"
#include "Util/UtUtil.h"

AosDocidShuffler::AosDocidShuffler(const bool flag)
:
AosDataShuffler(AOSDATASHUFFLER_DOCID, AosDataShufflerType::eDocid, flag),
mStartPos(-1),
mLength(-1),
mVirNumServers(-1),
mCrtServerNum(0)
{
}

AosDocidShuffler::AosDocidShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataShuffler(AOSDATASHUFFLER_DOCID, AosDataShufflerType::eDocid, false)
{
	config(conf, rdata);
	try
	{
		// 1. Retrieve mStartPos
		mStartPos = conf->getAttrInt(AOSTAG_START_POS, 0);
		if (mStartPos < 0)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

		// 2. Retrieve mLength
		mLength = conf->getAttrInt(AOSTAG_LENGTH, -1);
		if (mLength <= 0)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

		//3. Retrieve virtual
		mVirNumServers = conf->getAttrInt(AOSTAG_NUM_VIRTUALS, 0);
		if (mVirNumServers <= 0)
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

		
		OmnString datatype = conf->getAttrStr(AOSTAG_DATATYPE, "");
		mDataType = AosDataType::toEnum(datatype);
		if (!AosDataType::isValid(mDataType))
		{
			AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
			return;
		}

	}
	catch(...)
	{
		OmnAlarm << "Failed creating object" << enderr;
	}
}


AosDocidShuffler::~AosDocidShuffler()
{
}


bool 
AosDocidShuffler::shuffle(
		const AosDataCacherPtr &cacher,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	AosDataBlobPtr blob = cacher->getBlob();
	if (blob->size() > eMinSizeToRunInThreads) return shufflerInThreads(cacher, cachers, rdata);

	aos_assert_r(cacher, false);
	aos_assert_r(cachers.size() >= (u32)mNumServers, false);
	for (u32 i=0; i<cachers.size(); i++) aos_assert_rr(cachers[i], rdata, false);

	switch (mDataType)
	{
	case AosDataType::eString:
		 return shufflerStr(blob, cachers, rdata);
		
	case AosDataType::eU64:
		 return shufflerU64(blob, cachers, rdata);

	case AosDataType::eU32:
		 return shufflerU32(blob, cachers, rdata);

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
	return false;
}


bool
AosDocidShuffler::shufflerStr(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	int record_len = 0;
	blob->resetRecordLoop();
	char *record;
	char *key;
	u64 distid;
	int idx;
	while ((record = blob->nextValue(record_len)))
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		key = &record[mStartPos];
		if (AosStr2U64(key, mLength, true, distid))
		{
			idx = route(distid); 
			aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);

			cachers[idx]->appendEntry(record, record_len, rdata);
		}
	}
	return true;
}


bool
AosDocidShuffler::shufflerU64(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	int record_len = 0;
	blob->resetRecordLoop();
	char *record;
	u64 distid;
	int idx;
	while ((record = blob->nextValue(record_len)))
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		distid = *(u64*)(record + mStartPos);
		idx = route(distid); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);

		cachers[idx]->appendEntry(record, record_len, rdata);
	}
	return true;
}


bool
AosDocidShuffler::shufflerU32(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	int record_len = 0;
	blob->resetRecordLoop();
	char *record;
	u64 distid;
	int idx;
	while ((record = blob->nextValue(record_len)))
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		distid = *(u32*)(record + mStartPos);
		idx = route(distid); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);

		cachers[idx]->appendEntry(record, record_len, rdata);
	}
	return true;
}


bool
AosDocidShuffler::appendEntry(const char *data, const int len, const AosRundataPtr &rdata)
{
	aos_assert_r(data, false);
	aos_assert_r(len > 0, false);

	aos_assert_r(mStartPos >= 0 && mStartPos < len, false);
	aos_assert_r(mLength > 0 && mLength <= len, false);

	char *key;
	u64 distid;
	switch (mDataType)
	{
	case AosDataType::eString:
		 key = (char *)&data[mStartPos];
		 aos_assert_r(AosStr2U64(key, mLength, true, distid), false);
		 break;
		
	case AosDataType::eU64:
		 distid = *(u64*)(data + mStartPos);
		 break;

	case AosDataType::eU32:
		 distid = *(u32*)(data + mStartPos);
		 break;

	default:
		AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
		return false;
	}

	int idx = route(distid); 
	aos_assert_r(idx >= 0 && (u32)idx < mDataCachers.size(), false);

	AosDataCacherPtr datacacher = mDataCachers[idx];
	aos_assert_r(datacacher, false);
	datacacher->appendEntry(data, len, rdata);
	return true;
}


int
AosDocidShuffler::route(const u64 &distid)
{
	if (mNumServers == 0 || mVirNumServers == 0) return distid; 

	int idx = mCrtServerNum;
	if (distid == 0)
	{
		mCrtServerNum = (mCrtServerNum + 1) % mNumServers;
	}
	else
	{
		idx = distid % mVirNumServers % mNumServers;
	}
	return idx;
}


bool 
AosDocidShuffler::shuffle(
		const AosDataCacherPtr &cacher,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	AosDataBlobPtr blob = cacher->getBlob();
	aos_assert_r(blob, false);
	switch (mDataType)
	{
	case AosDataType::eString:
		 return shufflerStr(blob, cachers, start_idx, num_entries, rdata);
		
	case AosDataType::eU64:
		 return shufflerU64(blob, cachers, start_idx, num_entries, rdata);

	case AosDataType::eU32:
		 return shufflerU32(blob, cachers, start_idx, num_entries, rdata);

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
	return false;

}

bool
AosDocidShuffler::shufflerStr(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	int64_t num = 0;
	char *key;
	u64 distid;
	int idx;
	int record_len;
	char *record = blob->firstRecordInRange(start_idx, record_len);
	while(record && num ++ < num_entries)
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		key = &record[mStartPos];
		if (AosStr2U64(key, mLength, true, distid))
		{
			idx = route(distid); 
			aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);
			cachers[idx]->appendEntry(record, record_len, rdata);
		}
		record = blob->nextRecordInRange(record_len);
	}
	return true;
}


bool
AosDocidShuffler::shufflerU64(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	int64_t num = 0;
	u64 distid;
	int idx;
	int record_len;
	char *record = blob->firstRecordInRange(start_idx, record_len);
	while(record && num ++ < num_entries)
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		distid = *(u64*)(record + mStartPos);
		idx = route(distid); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);
		cachers[idx]->appendEntry(record, record_len, rdata);

		record = blob->nextRecordInRange(record_len);
	}
	return true;
}


bool
AosDocidShuffler::shufflerU32(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	int64_t num = 0;
	u64 distid;
	int idx;
	int record_len;
	char *record = blob->firstRecordInRange(start_idx, record_len);
	while(record && num ++ < num_entries)
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		distid = *(u32*)(record + mStartPos);
		idx = route(distid); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);
		cachers[idx]->appendEntry(record, record_len, rdata);

		record = blob->nextRecordInRange(record_len);
	}
	return true;
}



AosDataShufflerPtr
AosDocidShuffler::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDocidShuffler(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}
