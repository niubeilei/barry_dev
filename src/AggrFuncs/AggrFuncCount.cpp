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
// 2014/08/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AggrFuncs/AggrFuncCount.h"

#include "API/AosApi.h"
#include "JimoProg/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAggrFuncCount_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAggrFuncCount(version);
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



AosAggrFuncCount::AosAggrFuncCount(const int version)
:
AosAggrFunc(version)
{
}


AosAggrFuncCount::~AosAggrFuncCount()
{
}


AosJimoPtr 
AosAggrFuncCount::cloneJimo() const
{
	return OmnNew AosAggrFuncCount(*this);
}


bool 
AosAggrFuncCount::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	/*	
	OmnString type = AosGetXmlAttrStr("data_type", worker_doc, jimo_doc, "");
	if (type == "")
	{
		AosSetErrorUser(rdata, "missing data type") << enderr;
		return false;
	}

	mDataType = AosDataType::toEnum(type);
	if (!isDataTypeValid(mDataType))
	{
		AosSetErrorUser(rdata, "invalid data type") << type << enderr;
		return false;
	}
	*/
	//OmnNotImplementedYet;
	return true;
}


bool
AosAggrFuncCount::updateData(AosRundata *rdata, 
		char *field_data, 
		const int field_len, 
		const char *input_data, 
		const u32 input_data_len, 
		const AosDataType::E input_data_type)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosAggrFuncCount::updateOutputData(
		AosRundata *rdata, 
		char *input_value, 
		const AosDataType::E input_value_type,
		char *output_value, 
		const AosDataType::E output_value_type)
{
	//OmnTagFuncInfo << endl;

	switch (input_value_type)
	{
	case AosDataType::eU64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(u64 *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value += *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value += *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			*(double *)output_value += *(u64 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eU32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(u32 *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value += *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value += *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			*(double *)output_value += *(u32 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(int64_t *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value += *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value += *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			*(double *)output_value += *(int64_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(int32_t *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value += *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value += *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			*(double *)output_value += *(int32_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eDouble:
	case AosDataType::eNumber:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(double *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value += *(double *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(double *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(double *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
		 case AosDataType::eNumber:
			*(double *)output_value += *(double *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	default:
		OmnNotImplementedYet;
		break;
	}
	
	return false;
}



//yang

bool
AosAggrFuncCount::updateOutputDataWithInsertDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type)
{
	//OmnTagFuncInfo << endl;

	switch (input_value_type)
	{
	case AosDataType::eU64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(u64 *)input_value;
		 	return true;
		 case AosDataType::eU32:
			*(u32*)output_value += *(u64 *)input_value;
		 	return true;
		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(u64 *)input_value;
		 	return true;
		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(u64 *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value += *(u64 *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value += *(u64 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eU32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(u32 *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value += *(u32 *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(u32 *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(u32 *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value += *(u32 *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value += *(u32 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value += *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value += *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value += *(int64_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value += *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value += *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value += *(int32_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eDouble:
	case AosDataType::eNumber:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value += *(double *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value += *(double *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value += *(double *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value += *(double *)input_value;
		 	return true;

		 case AosDataType::eDouble:
		 case AosDataType::eNumber:
			*(double *)output_value += *(double *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	default:
		OmnNotImplementedYet;
		break;
	}

	return false;
}

bool
AosAggrFuncCount::updateOutputDataWithDeleteDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type)
{
	//OmnTagFuncInfo << endl;

	switch (input_value_type)
	{
	case AosDataType::eU64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value -= *(u64 *)input_value;
		 	return true;
		 case AosDataType::eU32:
			*(u32*)output_value -= *(u64 *)input_value;
		 	return true;
		 case AosDataType::eInt64:
			*(int64_t*)output_value -= *(u64 *)input_value;
		 	return true;
		 case AosDataType::eInt32:
			*(int32_t*)output_value -= *(u64 *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value -= *(u64 *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value -= *(u64 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eU32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value -= *(u32 *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value -= *(u32 *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value -= *(u32 *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value -= *(u32 *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value -= *(u32 *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value -= *(u32 *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt64:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value -= *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value -= *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value -= *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value -= *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value -= *(int64_t *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value -= *(int64_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eInt32:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value -= *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value -= *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value -= *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value -= *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eDouble:
			*(double *)output_value -= *(int32_t *)input_value;
		 	return true;

		 case AosDataType::eNumber:
			*(double *)output_value -= *(int32_t *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eDouble:
	case AosDataType::eNumber:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value -= *(double *)input_value;
		 	return true;

		 case AosDataType::eU32:
			*(u32*)output_value -= *(double *)input_value;
		 	return true;

		 case AosDataType::eInt64:
			*(int64_t*)output_value -= *(double *)input_value;
		 	return true;

		 case AosDataType::eInt32:
			*(int32_t*)output_value -= *(double *)input_value;
		 	return true;

		 case AosDataType::eDouble:
		 case AosDataType::eNumber:
			*(double *)output_value -= *(double *)input_value;
		 	return true;

		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	default:
		OmnNotImplementedYet;
		break;
	}

	return false;
}

bool
AosAggrFuncCount::updateOutputDataWithUpdateDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type)
{
	OmnNotImplementedYet;
}

bool
AosAggrFuncCount::updateOutputDataWithDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type,
			AosAggrFuncObj::E deltatype)
{
	switch(deltatype)
	{
	case INSERT:
		updateOutputDataWithInsertDeltaData(rdata,input_value,input_value_type,
				output_value,output_value_type);
		break;
	case DELETE:
		updateOutputDataWithDeleteDeltaData(rdata,input_value,input_value_type,
				output_value,output_value_type);
		break;
	case UPDATE:
		updateOutputDataWithUpdateDeltaData(rdata,input_value,input_value_type,
				output_value,output_value_type);
		break;
	default:
		break;
	}

}



