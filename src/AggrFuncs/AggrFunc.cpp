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
// 2014/08/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AggrFuncs/AggrFunc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"


AosAggrFunc::AosAggrFunc(const int version)
:
AosAggrFuncObj(version)
{
}


AosAggrFunc::AosAggrFunc()
:
AosAggrFuncObj(1)
{
}

AosAggrFunc::~AosAggrFunc()
{
}


/*
AosJimoPtr 
AosAggrFunc::cloneJimo() const
{
	return OmnNew AosAggrFunc(*this);
}
*/


bool 
AosAggrFunc::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	OmnShouldNeverComeHere;
	return false;
}

AosAggrFuncObjPtr
AosAggrFunc::pickAggrFunc(
		const AosRundataPtr &rdata,
		const OmnString &func_name)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosAggrFunc::isFuncValid(
		const AosRundataPtr &rdata,
		const OmnString &func_name)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool
AosAggrFunc::updateData(AosRundata *rdata, 
		char *field_data, 
		const int field_len, 
		const char *input_data, 
		const u32 input_data_len, 
		const AosDataType::E input_data_type)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosAggrFunc::updateOutputData(
		AosRundata *rdata, 
		char *input_value, 
		const AosDataType::E input_value_type,
		char *output_value, 
		const AosDataType::E output_value_type)
{
	OmnShouldNeverComeHere;
	return false;
}



bool 
AosAggrFunc::isDataTypeValid(const AosDataType::E type)
{
	switch (type)
	{
	case AosDataType::eU64:
	case AosDataType::eU32:
	case AosDataType::eU16:
	case AosDataType::eU8:
	case AosDataType::eChar:
	case AosDataType::eInt64:
	case AosDataType::eInt32:
	case AosDataType::eInt16:
	case AosDataType::eBool:
	case AosDataType::eFloat:
	case AosDataType::eDouble:
		 return true;
	
	default: return false;
	}

	return false;
}


OmnString
AosAggrFunc::getAggrFuncName()
{
	OmnShouldNeverComeHere;
	return "";
}

