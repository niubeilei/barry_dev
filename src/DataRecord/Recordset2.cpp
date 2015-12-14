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
// A record set holds a number of records. It is used by a consumer. 
// For performance considerations, record sets are not thread safe. 
// If the caller wants to make a record set thread safe, it must be
// protected using locks before using the record set.
//
// Record sets returns raw pointers instead of smart pointers. Users
// should never delete the pointers. 
//
// Records are saved in the form:
// 	[AosDataRecordObj *, data, len]
// 	[AosDataRecordObj *, data, len]
// 	...
// 	[AosDataRecordObj *, data, len]
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/Recordset2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "MetaData/MetaData.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/Buff.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"
#include <limits.h>


//int64_t AosRecordset2::smMaxRecordsetSize = AosRecordset2::eDftMaxRecordsetSize;

AosRecordset2::AosRecordset2()
:
mMetaDataRaw(0),
mSchemaRaw(0),
mRawRecords(0)
{
	mCount = 0;
}


AosRecordset2::AosRecordset2(const AosRecordset2 &rhs)
:
mMetaDataRaw(0),
mSchemaRaw(0),
mRawRecords(0)
{
	mCount = 0;
}


AosRecordset2::~AosRecordset2()
{
}


AosRecordsetObjPtr
AosRecordset2::clone(AosRundata *rdata) const
{
	try
	{
		return OmnNew AosRecordset2(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosRecordsetObjPtr 
AosRecordset2::createRecordset(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	AosRecordset2 * recordset = OmnNew AosRecordset2();
	return recordset;
}

bool
AosRecordset2::setData(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	aos_assert_rr(buff, rdata, false);
	mDataBuff = buff;
	mDataBuffRaw = mDataBuff.getPtrNoLock();
	return true;
}


vector<AosDataRecordObjPtr>
AosRecordset2::getRecords()
{
	vector<AosDataRecordObjPtr> records;
	if (mSchemaRaw)
	{
		mSchemaRaw->getRecords(records);
		mRecords = records;
	}

	//if (mRecords.empty()) return 0;
	return mRecords;
}

AosDataRecordObjPtr
AosRecordset2::getRecord()
{
	if (mRecords.empty()) return 0;
	return mRecords[0];
}

AosDataRecordObj *
AosRecordset2::getRawRecord(const int idx)
{
	return getRecord().getPtr();
}

bool
AosRecordset2::holdRecord(
		AosRundata *rdata, 
		const AosDataRecordObjPtr &record)
{
	return false;
}

	
bool
AosRecordset2::holdRecords(
		AosRundata *rdata, 
		const vector<AosDataRecordObjPtr> &records)
{
	return false;
}


void
AosRecordset2::setMetaData(
		AosRundata *rdata,  
		const AosMetaDataPtr &metadata)
{
	mMetaData = metadata;
	mMetaDataRaw = metadata.getPtr();
}


bool
AosRecordset2::nextRecord(
		AosRundata* rdata, 
		AosDataRecordObj * &record)
{
	mCount ++;
	mIncomplete = false;
	int status = -2;
	
	aos_assert_r(mSchemaRaw,false);
	aos_assert_r(mDataBuffRaw,false);

	while(1)
	{
		if ( mDataBuffRaw->dataLen() ==  mDataBuffRaw->getCrtIdx())
		{
			record = 0;
			return true;
		}
		char *data =  mDataBuffRaw->data() + mDataBuffRaw->getCrtIdx();
		int64_t parsed_len = 0;
		
		status = mSchemaRaw->nextRecord(rdata,data, mDataBuffRaw->dataLen()-mDataBuffRaw->getCrtIdx(), mMetaDataRaw, record, (int&)parsed_len);

		if (mMetaDataRaw && status != -1)
		{
			mMetaDataRaw->moveToNext();
			mMetaDataRaw->setRecordLength(parsed_len);
		}
		
		switch(status)
		{
			case 0:   		//valid
				//mDataBuffRaw->setCrtIdx(mDataBuffRaw->getCrtIdx() + parsed_len);
				if (mBuffDataRaw->getCrtIdx()+parsed_len > mBuffDataRaw->dataLen())
				{
					OmnScreen << "$$$$$$$$$$$$$$$$$$ " << endl;
					OmnScreen << " len is " <<  mDataBuffRaw->dataLen()-mDataBuffRaw->getCrtIdx() 
						<< " parsed len is " << parsed_len << endl;	

				}
				mBuffDataRaw->setCrtIdx(mBuffDataRaw->getCrtIdx() + parsed_len);
				aos_assert_r(mBuffDataRaw->getCrtIdx()<= mBuffDataRaw->dataLen(), false);
				return true;
			case -1:       //incomplete
				record = 0;
				aos_assert_r(mBuffDataRaw->getCrtIdx()<= mBuffDataRaw->dataLen(), false);
				if (mBuffDataRaw->isEOF())
				{
					status = -2;
					mIncomplete = false;
					mBuffDataRaw->setCrtIdx(mBuffDataRaw->dataLen());
					continue;
					//return true;
				}
				else
				{
					mIncomplete = true;
					return true;
				}

			case -2:	   //invalid
				mBuffDataRaw->setCrtIdx(mBuffDataRaw->getCrtIdx() + parsed_len);
				mIncomplete = false;

				aos_assert_r(mBuffDataRaw->getCrtIdx()<= mBuffDataRaw->dataLen(), false);
				//mDataBuffRaw->setCrtIdx(mDataBuffRaw->getCrtIdx() + parsed_len);
				continue;

			default:
				OmnShouldNeverComeHere;
				return false;
		}
	/*
	if (status == 0) //valid
	{
		mDataBuffRaw->setCrtIdx(mDataBuffRaw->getCrtIdx() + parsed_len);
		break;
	}
	if (status == -1) //invalid
	{
		mDataBuffRaw->setCrtIdx(mDataBuffRaw->getCrtIdx() + parsed_len);
		continue;
	}
	if (status == -2) //incomplete
	{
		record = 0;
		break;
	}
	*/
	}
	return true;
}


bool
AosRecordset2::appendRecord(
		AosRundata *rdata, 
		const int index,
		const int64_t offset, 
		const int len,
		const AosMetaDataPtr &metaData)
{
	return false;
}



bool
AosRecordset2::reset()
{
	mDataBuff = 0; 
	mDataBuffRaw = NULL;
	mBuffData = 0;
	mBuffDataRaw = NULL;
	mMetaData = 0;
	mMetaDataRaw = NULL;
	mRecords.clear();
	mRawRecords.clear();
	mIncomplete = false;
	return true;
}

bool 
AosRecordset2::setSchema(
			AosRundata *rdata,    
			AosSchemaObjPtr schema)
{
	mSchema = schema;
	mSchemaRaw = mSchema.getPtr();
	return  true;
}

int64_t	
AosRecordset2::size() const 
{
	if (mDataBuffRaw)
		return mDataBuffRaw->dataLen();
}

bool 
AosRecordset2::setBuffData(
			AosRundata *rdata,    
			AosBuffDataPtr buffdata)
{
	mBuffData = buffdata;
	mBuffDataRaw = buffdata.getPtr();

	if (mBuffDataRaw)
	{
		mMetaData = mBuffDataRaw->getMetadata();
		mMetaDataRaw = mMetaData.getPtr();

		mDataBuff = mBuffDataRaw->getBuff();
		mDataBuffRaw = mDataBuff.getPtr();
	}
}


bool
AosRecordset2::getBuffData(
		AosBuffDataPtr &buffdata)
{
	buffdata = mBuffData;
	return true;
}

