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
#include "AggrFuncs/AggrFuncDistCount.h"

#include "API/AosApi.h"
#include "JimoProg/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAggrFuncDistCount_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAggrFuncDistCount(version);
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



AosAggrFuncDistCount::AosAggrFuncDistCount(const int version)
:
AosAggrFunc(version)
{
}


AosAggrFuncDistCount::~AosAggrFuncDistCount()
{
}


AosJimoPtr 
AosAggrFuncDistCount::cloneJimo() const
{
	return OmnNew AosAggrFuncDistCount(*this);
}


bool 
AosAggrFuncDistCount::config(
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
AosAggrFuncDistCount::updateData(AosRundata *rdata, 
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
AosAggrFuncDistCount::updateOutputData(
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
			*(u64*)output_value = 1;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value = 1;
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
			*(u64*)output_value = 1;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value = 1;
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
			*(u64*)output_value = 1;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value = 1;
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
			*(u64*)output_value = 1;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value = 1;
		 	return true;
		
		 default:
			OmnNotImplementedYet;
		 	break;
		 }
		 return false;

	case AosDataType::eDouble:
		 switch(output_value_type)
		 {
		 case AosDataType::eU64:
			*(u64*)output_value = 1;
		 	return true;
		 
		 case AosDataType::eU32:
			*(u32*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt64:
			*(int64_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eInt32:
			*(int32_t*)output_value = 1;
		 	return true;
		
		 case AosDataType::eDouble:
			*(double *)output_value = 1;
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

