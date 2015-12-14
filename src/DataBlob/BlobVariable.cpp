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
// This data blob assumes records are in the following format:
// 		[key, value]
// where both key and value are stored in a fixed length memory record.
// Its value is normally arranged at the end of the memory record. The key
// part is null terminated by this class. 
//
// Modification History:
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/BlobVariable.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataBlob/Ptrs.h"
#include "DataRecord/DataRecord.h"
#include "DataBlob/DataBlobFullHandler.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/BuffArray.h"
#include "Util/Sort.h"
#include "Util/CompareFun.h"
#include "Util1/MemMgr.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/Ptrs.h"


AosBlobVariable::AosBlobVariable()
:
AosDataBlob(AOSDATABLOB_VARIABLE, AosDataBlobType::eVariable),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mCrtData(0),
mCrtIdx(-1)
{
}


AosBlobVariable::AosBlobVariable(
		const OmnString &sep,
		const AosRundataPtr &rdata)
:
AosDataBlob(AOSDATABLOB_VARIABLE, AosDataBlobType::eVariable),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mCrtData(0),
mCrtIdx(-1),
mSep(sep)
{
}


AosBlobVariable::AosBlobVariable(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
:
AosDataBlob(AOSDATABLOB_VARIABLE, AosDataBlobType::eVariable),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mCrtData(0),
mCrtIdx(-1)
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosBlobVariable::AosBlobVariable(const AosBlobVariable &rhs)
:
AosDataBlob(rhs),
mBuff(rhs.mBuff->clone(AosMemoryCheckerArgsBegin)),
mCrtData(0),
mCrtIdx(-1),
mSep(rhs.mSep)
{
}


AosBlobVariable::~AosBlobVariable()
{
	//OmnScreen << "blob vari deleted" << endl;	
}


bool
AosBlobVariable::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_rr(conf, rdata, false);

	AosDataBlob::config(conf, rdata);

	mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mSep = conf->getAttrStr(AOSTAG_SEPARATOR, "\n");
	return true;
}


void
AosBlobVariable::resetRecordLoop()
{
	OmnNotImplementedYet;
}


void
AosBlobVariable::resetRangeLoop()
{
	OmnNotImplementedYet;
}


bool
AosBlobVariable::sort()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobVariable::clearData()
{
	OmnNotImplementedYet;
	return false;
}



bool    
AosBlobVariable::setData(const char *record, const u64 &len)
{
	OmnNotImplementedYet;
	return false;
}


AosDataBlobObjPtr
AosBlobVariable::clone(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosBlobVariable(conf, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object: " << conf->toString() << enderr;
		return 0;
	}

	return 0;
}


AosDataBlobObjPtr
AosBlobVariable::clone()
{
	try
	{
		return OmnNew AosBlobVariable(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
	return 0;
}


bool 
AosBlobVariable::resetRewriteLoop()
{
	OmnNotImplementedYet;
	return false;
}


bool    
AosBlobVariable::setRewriteLoopIdx(const u64 idx)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBlobVariable::rewriteNextRecord(
		const AosDataRecordObjPtr &record, 
		const bool append_if_overflow,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBlobVariable::rewriteRecordByIdx(
		const u64 idx,
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


int 
AosBlobVariable::getRecordLen() const
{
	return 0;
}


u64     
AosBlobVariable::getRewriteLoopIdx() const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosBlobVariable::getRecord(
		const int64_t &idx,
		const AosDataRecordObjPtr &record) const
{
	OmnNotImplementedYet;
	return false;
}


const char *
AosBlobVariable::getMemory(
		const int64_t &start_idx,
		const int64_t &end_idx,
		int64_t &length,
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}


int
AosBlobVariable::getRecordLen(const int idx) const
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosBlobVariable::start(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBlobVariable::finish(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBlobVariable::runFullActions(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}



int64_t
AosBlobVariable::size() const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosBlobVariable::nextRecord(const AosDataRecordObjPtr &record)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBlobVariable::nextValue(
		AosValueRslt &value, 
		const bool copy_flag, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobVariable::nextValue(char **data, int &len, u64 &docid, AosBuffDataPtr &metaData, const AosRundataPtr &rdata)
{
	*data = 0;
	len = 0;
	docid = 0;
	metaData = 0;
	
	if (mSep == "")
	{
		if (!mCrtData)
		{
			return false;
		}
		
		len = *(u32 *)mCrtData;
		int slen = len + sizeof(u32) + sizeof(u64);
		docid = *(u64*)&mCrtData[4];
		mCrtIdx += slen;
		*data = &mCrtData[sizeof(u32) + sizeof(u64)];
		if (mCrtIdx >= mBuff->dataLen())
		{
			mCrtData = 0;
			mCrtIdx = -1;
		}
		else
		{
			mCrtData += slen;
		}

		metaData = mMetaData;
		return true;
	}

	aos_assert_r(mCrtData, false);
	char * sub = strstr(mCrtData, mSep.data());
	if(!sub)
	{
		return false;
	}

	len = (sub - mCrtData) + mSep.length();
	mCrtIdx += len;
	*data = mCrtData;
	// JACKIE-HADOOP
	int64_t offset = mMetaData->getOffset();
	mMetaData->setOffset(offset + len);

	if(mCrtIdx >= mBuff->dataLen())
	{
		mCrtData = 0;
		mCrtIdx = -1;
	}
	else
	{
		mCrtData += len;
	}
	metaData = mMetaData;
	return true;
}


bool
AosBlobVariable::firstRecordInRange(
		const int64_t &rcd_idx, 
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobVariable::nextRecordInRange(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


u64
AosBlobVariable::getMemSize() const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosBlobVariable::serializeTo(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

bool 
AosBlobVariable::serializeFrom(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobVariable::firstValueInRange(
		const int64_t &rcd_idx, 
		AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


const char *
AosBlobVariable::getMemory(
		const u64 &start_idx,
		const u64 &end_idx,
		int64_t &mem_size,
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return 0;
}
	

bool
AosBlobVariable::hasMoreData() const
{
	if(!mCrtData) return false;
	if(mCrtIdx == -1 || mCrtIdx == mBuff->dataLen()) return false;
	return true;
}


bool 
AosBlobVariable::setDataBlock(
		const AosBuffDataPtr &metaData,
		const AosRundataPtr &rdata)
{
	aos_assert_r(metaData, false);
	AosBuffPtr buff = metaData->getBuff();
	aos_assert_r(buff, false);
	mBuff = buff->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mBuff, false);
	mCrtData = mBuff->data();
	mCrtIdx = 0;
	mMetaData = metaData;
	return true;
}
	

bool
AosBlobVariable::nextRecord(
		AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

int64_t
AosBlobVariable::findFirstEntry(
		const int64_t &start_pos,
		const OmnString &key,
		const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return -1;
}
	

bool
AosBlobVariable::nextValueInRange(
		AosValueRslt &value, 
		const bool need_copy,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosBlobVariable::appendEntry(
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBlobVariable::appendRecord(
		const AosDataRecordObjPtr &record, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

