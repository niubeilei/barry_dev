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
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Jimos/StructProcMin.h"

#include "Util/DataTypes.h"
#include "Rundata/Rundata.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStructProcMin_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStructProcMin(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosStructProcMin::AosStructProcMin(const int version)
:
AosStructProc(AOSSTRUCTPROC_MIN, version),
mInvalidStatValue(NULL)
{
	mVersion = version;	
	mDftValue = eMinDftValue;

	mInvalidStatValue = OmnNew char[sizeof(u64) + 10];
	memset(mInvalidStatValue, mDftValue, sizeof(u64) + 10);
}


AosStructProcMin::AosStructProcMin(const AosStructProcMin &proc)
:
AosStructProc(AOSSTRUCTPROC_MIN, proc.mVersion),
mVersion(proc.mVersion),
mInvalidStatValue(NULL)
{
	mDftValue = proc.mDftValue;
	mInvalidStatValue = OmnNew char[sizeof(u64) + 10];
	memset(mInvalidStatValue, mDftValue, sizeof(u64) + 10);
}


AosStructProcMin::~AosStructProcMin()
{
	if (mInvalidStatValue != NULL)
	{
		delete mInvalidStatValue;
	}
}


void
AosStructProcMin::config(const AosXmlTagPtr &conf)
{
}


bool
AosStructProcMin::setFieldValue(
		char *array,
		const int64_t data_len,
		const int pos,
		const int64_t &value,
		AosDataType::E data_type,
		const u64 &docid)
{
	// 2. Start of stat doc
	aos_assert_r(pos + AosDataType::getValueSize(data_type) <= data_len, false);
	switch (data_type)
	{
	case AosDataType::eU32:
		 {
			 u32 vv = *(u32*)&array[pos];
			 if (vv == *(u32*)mInvalidStatValue) 
			 {
				 *(u32*)&array[pos] = value;
			     AosValueRslt new_rslt;
				 new_rslt.setU32(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
				 break;
			 }

			 if (value < vv)
			 {
				 AosValueRslt old_rslt;
				 old_rslt.setU32(vv);
				 old_rslt.setDocid(docid);
				 //mOldValueRslt.push_back(old_rslt);

			     AosValueRslt new_rslt;
				 *(u32*)&array[pos] = value;
				 new_rslt.setU32(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
			 }
		 }
		 break;

	case AosDataType::eU64:
		 {
			 u64 vv = *(u64*)&array[pos];
			 if (vv == *(u64*)mInvalidStatValue) 
			 {
				 *(u64*)&array[pos] = value;
			     AosValueRslt new_rslt;
				 new_rslt.setValue(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
				 break;
			 }

			 if (value >= 0 && (u64)value < vv)
			 {
				 AosValueRslt old_rslt;
				 old_rslt.setValue(vv);
				 old_rslt.setDocid(docid);
				 //mOldValueRslt.push_back(old_rslt);

			     AosValueRslt new_rslt;
				 *(u64*)&array[pos] = value;
				 new_rslt.setValue(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
			 }
		 }
		 break;

	case AosDataType::eInt64:
		 {
			 int64_t vv = *(int64_t*)&array[pos];
			 if (vv == *(int64_t*)mInvalidStatValue) 
			 {
				 *(int64_t*)&array[pos] = value;
			     AosValueRslt new_rslt;
				 new_rslt.setValue(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
				 break;
			 }

			 if (value < vv)
			 {
				 AosValueRslt old_rslt;
				 old_rslt.setValue(vv);
				 old_rslt.setDocid(docid);
				 //mOldValueRslt.push_back(old_rslt);

				 *(int64_t*)&array[pos] = value;
			     AosValueRslt new_rslt;
				 new_rslt.setValue(value);
				 new_rslt.setDocid(docid);
				 //mNewValueRslt.push_back(new_rslt);
			 }
		 }
		 break;

	default:	
		 break;
	}

	return true;
}	


AosJimoPtr 
AosStructProcMin::cloneJimo()  const
{
	try
	{
		return OmnNew AosStructProcMin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


int64_t 
AosStructProcMin::calculateGeneralValue(const int64_t &new_value, const int64_t &old_value)
{
	if (old_value == 0) return new_value;
	return (new_value < old_value ? new_value:old_value);
}
