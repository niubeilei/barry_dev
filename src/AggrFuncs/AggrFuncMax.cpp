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
#include "AggrFuncs/AggrFuncMax.h"

#include "API/AosApi.h"
#include "JimoProg/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosAggrFuncMax_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAggrFuncMax(version);
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



AosAggrFuncMax::AosAggrFuncMax(const int version)
:
AosAggrFunc(version)
{
}


AosAggrFuncMax::~AosAggrFuncMax()
{
}


AosJimoPtr 
AosAggrFuncMax::cloneJimo() const
{
	return OmnNew AosAggrFuncMax(*this);
}


bool 
AosAggrFuncMax::config(
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
AosAggrFuncMax::updateData(AosRundata *rdata, 
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
AosAggrFuncMax::updateOutputData(
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
			if (*(u64*)output_value < *(u64 *)input_value)
				*(u64*)output_value = *(u64 *)input_value;

		 	return true;
		 
		 case AosDataType::eU32:
			if (*(u32*)output_value < *(u64 *)input_value)
				*(u32*)output_value = *(u64 *)input_value;

		 	return true;
		
		 case AosDataType::eInt64:
			if (*(int64_t*)output_value < *(u64 *)input_value)
				*(int64_t*)output_value = *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			if (*(int32_t*)output_value < *(u64 *)input_value)
				*(int32_t*)output_value = *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			if (*(double*)output_value < *(u64 *)input_value)
				*(double*)output_value = *(u64 *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			if (*(double*)output_value < *(u64 *)input_value)
				*(double*)output_value = *(u64 *)input_value;
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
			if (*(u64*)output_value < *(u32 *)input_value)
				*(u64*)output_value = *(u32 *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			if (*(u32*)output_value < *(u32 *)input_value)
				*(u32*)output_value = *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			if (*(int64_t*)output_value < *(u32 *)input_value)
				*(int64_t*)output_value = *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			if (*(int32_t*)output_value < *(u32 *)input_value)
				*(int32_t*)output_value = *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			if (*(double*)output_value < *(u32 *)input_value)
				*(double*)output_value = *(u32 *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			if (*(double*)output_value < *(u32 *)input_value)
				*(double*)output_value = *(u32 *)input_value;
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
			if (*(u64*)output_value < *(int64_t *)input_value)
				*(u64*)output_value = *(int64_t *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			if (*(u32*)output_value < *(int64_t *)input_value)
				*(u32*)output_value = *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			if (*(int64_t*)output_value < *(int64_t *)input_value)
				*(int64_t*)output_value = *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			if (*(int32_t*)output_value < *(int64_t *)input_value)
				*(int32_t*)output_value = *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			if (*(double*)output_value < *(int64_t *)input_value)
				*(double*)output_value = *(int64_t *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			if (*(double*)output_value < *(int64_t *)input_value)
				*(double*)output_value = *(int64_t *)input_value;
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
			if (*(u64*)output_value < *(int32_t *)input_value)
				*(u64*)output_value = *(int32_t *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			if (*(u32*)output_value < *(int32_t *)input_value)
				*(u32*)output_value = *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			if (*(int64_t*)output_value < *(int32_t *)input_value)
				*(int64_t*)output_value = *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			if (*(int32_t*)output_value < *(int32_t *)input_value)
				*(int32_t*)output_value = *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
			if (*(double*)output_value < *(int32_t *)input_value)
				*(double*)output_value = *(int32_t *)input_value;
		 	return true;
		
		 case AosDataType::eNumber:
			if (*(double*)output_value < *(int32_t *)input_value)
				*(double*)output_value = *(int32_t *)input_value;
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
			if (*(u64*)output_value < *(double *)input_value)
				*(u64*)output_value = *(double *)input_value;
		 	return true;
		 
		 case AosDataType::eU32:
			if (*(u32*)output_value < *(double *)input_value)
				*(u32*)output_value = *(double *)input_value;
		 	return true;
		
		 case AosDataType::eInt64:
			if (*(int64_t*)output_value < *(double *)input_value)
				*(int64_t*)output_value = *(double *)input_value;
		 	return true;
		
		 case AosDataType::eInt32:
			if (*(int32_t*)output_value < *(double *)input_value)
				*(int32_t*)output_value = *(double *)input_value;
		 	return true;
		
		 case AosDataType::eDouble:
		 case AosDataType::eNumber:
			if (*(double*)output_value < *(double *)input_value)
				*(double*)output_value = *(double *)input_value;
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
AosAggrFuncMax::updateOutputDataWithInsertDeltaData(
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
		{
			u64 inputValue = *(u64 *)input_value;
			switch(output_value_type)
			{
				case AosDataType::eU64:
					if(*(u64*)output_value < inputValue)
						*(u64*)output_value = inputValue;
					return true;

				case AosDataType::eU32:
					if(*(u32*)output_value < inputValue)
						*(u32*)output_value = inputValue;
					return true;

				case AosDataType::eInt64:
					if(*(int64_t*)output_value < (int64_t)inputValue)
						*(int64_t*)output_value = inputValue;
					return true;

				case AosDataType::eInt32:
					if(*(int32_t*)output_value < (int32_t)inputValue)
						*(int32_t*)output_value = inputValue;
					return true;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					if(*(double*)output_value < inputValue)
						*(double *)output_value += inputValue;
					return true;

				default:
					OmnNotImplementedYet;
					break;
			}
			return false;
		}
	case AosDataType::eU32:
		{
			u32 inputValue = *(u32 *)input_value;
			switch(output_value_type)
			{
				case AosDataType::eU64:
					if(*(u64*)output_value < inputValue)
						*(u64*)output_value = inputValue;
					return true;

				case AosDataType::eU32:
					if(*(u32*)output_value < inputValue)
						*(u32*)output_value = inputValue;
					return true;

				case AosDataType::eInt64:
					if(*(int64_t*)output_value < inputValue)
						*(int64_t*)output_value = inputValue;
					return true;

				case AosDataType::eInt32:
					if(*(int32_t*)output_value < (int32_t)inputValue)
						*(int32_t*)output_value = inputValue;
					return true;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					if(*(double*)output_value < inputValue)
						*(double*)output_value = inputValue;
					return true;

				default:
					OmnNotImplementedYet;
					break;
			}
			return false;
		}
	case AosDataType::eInt64:
		{
			int64_t inputValue = *(int64_t *)input_value;
			switch(output_value_type)
			{
				case AosDataType::eU64:
					if(*(u64*)output_value < (u64)inputValue)
						*(u64*)output_value = inputValue;
					return true;

				case AosDataType::eU32:
					if(*(u32*)output_value < inputValue)
						*(u32*)output_value = inputValue;
					return true;

				case AosDataType::eInt64:
					if(*(int64_t*)output_value < inputValue)
						*(int64_t*)output_value = inputValue;
					return true;

				case AosDataType::eInt32:
					if(*(int32_t*)output_value < inputValue)
						*(int32_t*)output_value = inputValue;
					return true;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					if(*(double*)output_value < inputValue)
						*(double *)output_value = inputValue;
					return true;

				default:
					OmnNotImplementedYet;
					break;
			}
			return false;
		}
	case AosDataType::eInt32:
		{
			int32_t inputValue = *(int32_t *)input_value;
			switch(output_value_type)
			{
				case AosDataType::eU64:
					if(*(u64*)output_value < inputValue)
						*(u64*)output_value = inputValue;
					return true;

				case AosDataType::eU32:
					if(*(u32*)output_value < (u32)inputValue)
						*(u32*)output_value = inputValue;
					return true;

				case AosDataType::eInt64:
					if(*(int64_t*)output_value < inputValue)
						*(int64_t*)output_value = inputValue;
					return true;

				case AosDataType::eInt32:
					if(*(int32_t*)output_value < inputValue)
						*(int32_t*)output_value = inputValue;
					return true;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					if(*(double*)output_value < inputValue)
						*(double *)output_value = inputValue;
					return true;

				default:
					OmnNotImplementedYet;
					break;
			}
			return false;
		}

	case AosDataType::eDouble:
	case AosDataType::eNumber:
		{
			double inputValue = *(double *)input_value;
			switch(output_value_type)
			{
				case AosDataType::eU64:
					if(*(u64*)output_value < inputValue)
						*(u64*)output_value = inputValue;
					return true;

				case AosDataType::eU32:
					if(*(u32*)output_value < inputValue)
						*(u32*)output_value = inputValue;
					return true;

				case AosDataType::eInt64:
					if(*(int64_t*)output_value < inputValue)
						*(int64_t*)output_value = inputValue;
					return true;

				case AosDataType::eInt32:
					if(*(int32_t*)output_value < inputValue)
						*(int32_t*)output_value = inputValue;
					return true;

				case AosDataType::eDouble:
				case AosDataType::eNumber:
					if(*(double*)output_value < inputValue)
						*(double *)output_value = inputValue;
					return true;

				default:
					OmnNotImplementedYet;
					break;
			}
			return false;
		}


	default:
		OmnNotImplementedYet;
		break;
	}

	return false;
}


bool
AosAggrFuncMax::updateOutputDataWithDeleteDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type)
{
	OmnAlarm << "Now We don't support max delete delta !" << enderr;
	return false;
}

bool
AosAggrFuncMax::updateOutputDataWithUpdateDeltaData(
			AosRundata *rdata,
			char *input_value,
			const AosDataType::E input_value_type,
			char *output_value,
			const AosDataType::E output_value_type)
{
	OmnNotImplementedYet;
	return false;
}



bool
AosAggrFuncMax::updateOutputDataWithDeltaData(
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
	return true;
}

