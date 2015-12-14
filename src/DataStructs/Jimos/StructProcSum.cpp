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
#include "DataStructs/Jimos/StructProcSum.h"

#include "Util/DataTypes.h"
#include "Rundata/Rundata.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStructProcSum_0(const AosRundataPtr &rdata, const u32 version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStructProcSum(version);
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


AosStructProcSum::AosStructProcSum(const int version)
:
AosStructProc(AOSSTRUCTPROC_SUM, version)
{
	mDftValue = 0;
}


AosStructProcSum::~AosStructProcSum()
{
}


void
AosStructProcSum::config(const AosXmlTagPtr &conf)
{
}


bool
AosStructProcSum::setFieldValue(
		char *array,
		const int64_t data_len,
		const int pos,
		const int64_t &value,
		AosDataType::E data_type,
		const u64 &docid)
{
	// 2. Start of stat doc
	aos_assert_r(pos + AosDataType::getValueSize(data_type) <= data_len, false);
	aos_assert_r(mDftValue >= 0, false);

	AosValueRslt new_rslt;
	switch (data_type)
	{
	case AosDataType::eU32:
		 {
			 u32 vv = *(u32*)&array[pos];
			 if (vv != (u32)mDftValue) 
			 {
				AosValueRslt old_rslt;
				old_rslt.setU32(vv);
				old_rslt.setDocid(docid);
				//mOldValueRslt.push_back(old_rslt);
			 }
			 *(u32*)&array[pos] += value;
			 vv = *(u32*)&array[pos];
			 new_rslt.setU32(vv);
		 }
		 break;

	case AosDataType::eU64:
		 {
			 u64 vv = *(u64*)&array[pos];
			 if (vv != (u64)mDftValue) 
			 {
				AosValueRslt old_rslt;
				old_rslt.setValue(vv);
				old_rslt.setDocid(docid);
				//mOldValueRslt.push_back(old_rslt);
			 }
			 *(u64*)&array[pos] += value;
			 vv = *(u64*)&array[pos];
			 new_rslt.setValue(vv);
		 }
		 break;

	case AosDataType::eInt64:
		 {
			 int64_t vv = *(int64_t*)&array[pos];
			 if (vv != (int64_t)mDftValue) 
			 {
				AosValueRslt old_rslt;
				old_rslt.setValue(vv);
				old_rslt.setDocid(docid);
				//mOldValueRslt.push_back(old_rslt);
			 }
			 *(int64_t*)&array[pos] += value;
			 vv = *(int64_t*)&array[pos];
			 new_rslt.setValue(vv);
//OmnScreen << "AosStructProcSum statid:" << docid << ";vv:" << vv << ";" << endl;
		 }
		 break;

	default:	
		 break;
	}
	new_rslt.setDocid(docid);
	//mNewValueRslt.push_back(new_rslt);
	return true;
}	


//bool 
//AosStructProcSum::getFieldValue(
//		char *array,
//		const int64_t data_len,
//		const int pos,
//		AosDataType::E data_type,
//		AosValueRslt &value_rslt) 
//{
//	aos_assert_r(pos + AosDataType::getValueSize(data_type) <= data_len, false);
//	AosValueRslt new_rslt;
//	switch (data_type)
//	{
//	case AosDataType::eU32:
//		 {
//			 u32 vv = *(u32*)&array[pos];
//		 	 value_rslt.setU32(vv);
//		 }
//		 break;
//
//	case AosDataType::eU64:
//		 {
//			 u64 vv = *(u64*)&array[pos];
//			 value_rslt.setValue(vv);
//		 }
//		 break;
//
//	case AosDataType::eInt64:
//		 {
//			 int64_t vv = *(int64_t*)&array[pos];
//			 value_rslt.setValue(vv);
//		 }
//		 break;
//
//	default:	
//		 break;
//	}
//	return true;
//}


AosJimoPtr 
AosStructProcSum::cloneJimo()  const
{
	try
	{
		return OmnNew AosStructProcSum(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


int64_t 
AosStructProcSum::calculateGeneralValue(const int64_t &new_value, const int64_t &old_value)
{
	return new_value + old_value;
}
