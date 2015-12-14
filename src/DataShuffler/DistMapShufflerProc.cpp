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
#include "DataShuffler/DistMapShufflerProc.h"

#include "Util/DataTypes.h"
#include "Rundata/Rundata.h"

AosDistMapShuffler::AosDistMapShuffler(const bool flag)
:
AosDataShuffler(AOSDATASHUFFLER_DISTMAP, AosDataShufflerType::eDistMap, flag),
mStartPos(-1),
mLength(-1),
mMap(0)
{
}


AosDistMapShuffler::AosDistMapShuffler(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
:
AosDataShuffler(AOSDATASHUFFLER_DISTMAP, AosDataShufflerType::eDistMap, false),
mStartPos(-1),
mLength(-1),
mMap(0)
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


int
AosDistMapShuffler::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	if (!AosDataShuffler::config(conf, rdata)) 
	{
		return false;
	}

	// 1. Retrieve mStartPos
	mStartPos = conf->getAttrInt(AOSTAG_START_POS, 0);
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
		return false;
	}

	// 2. Retrieve mLength
	mLength = conf->getAttrInt(AOSTAG_LENGTH, -1);
	if (mLength <= 0)
	{
		AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
		return false;
	}

	OmnString datatype = conf->getAttrStr(AOSTAG_DATATYPE, "");
	mDataType = AosDataType::toEnum(datatype);
	if (!AosDataType::isValid(mDataType))
	{
		AosSetErrorU(rdata, "invalid_config") << ": " << conf->toString() << enderr;
		return false;
	}

	mSortMap = conf->getAttrBool("zky_sortmap", false);
	return true;
}


AosDistMapShuffler::~AosDistMapShuffler()
{
}


bool 
AosDistMapShuffler::shuffle(
		const AosDataCacherPtr &cacher,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMap, false);
	if (mSortMap) aos_assert_r(mMap->sort(), false);

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
		 OmnNotImplementedYet;
		 return false;

	case AosDataType::eU32:
		 OmnNotImplementedYet;
		 return false;

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
	return false;
}


bool
AosDistMapShuffler::appendEntry(const char *data, const int len, const AosRundataPtr &rdata)
{
	aos_assert_r(mMap, false);
	if (mSortMap) aos_assert_r(mMap->sort(), false);
	aos_assert_r(mDataCachers.size() == (u32)mNumServers, false);
	aos_assert_r(data, false);
	aos_assert_r(len > 0, false);

	aos_assert_r(mStartPos >= 0 && mStartPos < len, false);
	aos_assert_r(mLength > 0 && mLength <= len, false);

	int idx; 
	switch (mDataType)
	{
	case AosDataType::eString:
		 idx = routeByStr(data, len); 
		 break;

	case AosDataType::eU64:
		 OmnNotImplementedYet;
		 break;
	
	case AosDataType::eU32:
		 OmnNotImplementedYet;
		 break;

	default:
		 AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
		 return false;
	}

	aos_assert_r(idx >= 0 && (u32)idx < mDataCachers.size(), false);
	mDataCachers[idx]->appendEntry(data, len, rdata);
	return true;
}


bool 
AosDistMapShuffler::shuffle(
		const AosDataCacherPtr &cacher,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mMap, false);
	AosDataBlobPtr blob = cacher->getBlob();
	aos_assert_r(blob, false);
	switch (mDataType)
	{
	case AosDataType::eString:
		 return shufflerStr(blob, cachers, start_idx, num_entries, rdata); 

	case AosDataType::eU64:
		 OmnNotImplementedYet;
		 break;
	
	case AosDataType::eU32:
		 OmnNotImplementedYet;
		 break;

	default:
		 AosSetErrorU(rdata, "unrecognized_data_type") << ": " << AosDataType::getTypeStr(mDataType) << enderr;
		 return false;
	}
	return true;
}


AosDataShufflerPtr
AosDistMapShuffler::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDistMapShuffler(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}
}


bool
AosDistMapShuffler::shufflerStr(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const AosRundataPtr &rdata)
{
	int record_len = 0;
	blob->resetRecordLoop();
	char *record;
	while ((record = blob->nextValue(record_len)))
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		int idx = routeByStr(record, record_len); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);

		cachers[idx]->appendEntry(record, record_len, rdata);
	}
	return true;
}



bool
AosDistMapShuffler::shufflerStr(
		const AosDataBlobPtr &blob,
		vector<AosDataCacherPtr> &cachers,
		const int64_t &start_idx,
		const int64_t &num_entries,
		const AosRundataPtr &rdata)
{
	int record_len;
	char * record = blob->firstRecordInRange(start_idx, record_len);;
	int64_t num;
	while(record && num ++ < num_entries)
	{
		aos_assert_r(mStartPos >= 0 && mStartPos < record_len, false);
		aos_assert_r(mLength > 0 && mLength <= record_len, false);

		int idx = routeByStr(record, record_len); 
		aos_assert_r(idx >= 0 && (u32)idx < cachers.size(), false);

		cachers[idx]->appendEntry(record, record_len, rdata);
		record = blob->nextRecordInRange(record_len);
	}
	return true;
}


int 
AosDistMapShuffler::routeByStr(
		const char *record, 
		const int record_len)
{
	// This function retrieves the key from 'record' and finds
	// the index based on the map.
//u32 entries = mMap->getNumEntries();
//int len = 0;
//for (u32 i = 0; i< entries; i++)
//{
//    char *data = mMap->getRecord(i, len);
//	cout << data << " , " << i << endl ;
//}
	const char *key = &record[mStartPos];
	return mMap->findBoundary(key);
}

