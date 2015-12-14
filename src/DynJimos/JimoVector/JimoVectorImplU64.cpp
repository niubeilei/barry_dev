////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/JimoVector/JimoVectorImplU64.h"

#include "Rundata/Rundata.h"
#include "Util/DataTypes.h"


AosJimoVectorImplU64::AosJimoVectorImplU64()
{
}


AosJimoVectorImplU64::~AosJimoVectorImplU64()
{
}


bool 
AosJimoVectorImplU64::run(
		const AosRundataPtr &rdata, 
		const AosMethod::E method,
		const AosXmlTagPtr &sdoc)
{
	AosDataType::E data_type = rdata->getDataType();
	switch (method)
	{
	case AosMethod::ePushBack:
		 switch (data_type)
		 {
		 case AosDataType::eU64:
		 case AosDataType::eU32:
		 case AosDataType::eU16:
		 case AosDataType::eU8:
			  mVector.push_back(rdata->getU64());
			  return true;

		 case AosDataType::eInt64:
		 case AosDataType::eInt32:
		 case AosDataType::eInt16:
		 case AosDataType::eInt8:
			  {
				  int64_t vv = rdata->getInt64();
				  if (vv >= 0)
				  {
					  mVector.push_back((u64)vv);
					  return true;
				  }

				  switch (mErrorHandling)
				  {
				  case eIgnore:
					   return true;

				  case eTreatAsZero:
					   mVector.push_back(0);
					   return true;

				  case eReportError:
					   AosSetErrorU(rdata, mErrorMsg) << "Value: " << vv << enderr;
					   return false;

				  default:
					   break;
				  }
				  AosSetErrorU(rdata, "internal_error") << mErrorHandling << enderr;
				  return false;
			  }

		 case AosDataType::eFloat:
		 case AosDataType::eDouble:
			  switch (mFloatHandler)
			  {
			  case eTruncate:
				   mVector.push_back((u64)rdata->getDouble());
				   return true;

			  case eRoundUp:
				   mVector.push_back((u64)(rdata->getDouble()+0.5));
				   return true;

			  case eRoundDown:
				   {
					   double vv = rdata->getDouble()-0.5;
					   if (vv < 0.0)
					   {
						   mVector.push_back(0);
					   }
					   else
					   {
						   mVector.push_back((u64)vv);
					   }
				   }
				   return true;

			  case eTreatAsError:
				   AosSetErrorU(rdata, mTypeErrorFloat) << rdata->getDouble() << enderr;
				   return false;
			 
			  default:
				   break;
			  }
			  AosSetErrorUser(rdata, "u64_vector_but_found_float") 
				  << rdata->getDouble() << enderr;
			  return false;

		 default:
			  break;
		 }
		 AosSetErrorU(rdata, "data_type_not_supported") << data_type << enderr;
		 return false;

	case AosMethod::eReset:
		 mVector.clear();
		 rdata->setOk();
		 return true;

	case AosMethod::eSize:
		 if (mOutputFormatter)
		 {
			 mOutputFormatter->run(rdata, AosMethod::eSet, 
					 AosDataType::eU64, mSizeName, mVector.size());
			 return true;
		 }
		 else
		 {
		 	 rdata->setU64(mVector.size());
		 }
		 return true;

	case AosMethod::eCapacity:
		 if (mOutputFormatter)
		 {
			 mOutputFormatter->run(rdata, 
					 AosMethod::eSet, AosDataType::eU64, mCapacityName, mVector.size());
			 return true;
		 }
		 else
		 {
		 	 rdata->setU64(mVector.size());
		 }
		 return true;

	case AosMethod::eRBegin:
		 mItr = mVector.rbegin();
		 rdata->setOk();
		 return true;

	case AosMethod::eREnd:
		 mItr = mVector.rend();
		 rdata->setOk();
		 return true;

	case AosMethod::eBegin:
		 mItr = mVector.begin();
		 rdata->setOk();
		 return true;

	case AosMethod::eEnd:
		 mItr = mVector.end();
		 rdata->setOk();
		 return true;

	case AosMethod::eNext:
		 {
			 u64 vv;
		 	 if (mItr == mVector.end())
		 	 {
				 vv = mNextDefaultValue;
			 }
			 else
			 {
				 vv = *itr;
				 itr++;
			 }

		 	 if (mOutputFormatter)
		  	 {
			 	 mOutputFormatter->run(rdata, 
					 AosMethod::eSet, AosDataType::eU64, mNextName, vv);
			 	 return true;
		 	 }
		 	 else
		 	 {
		 	 	 rdata->setU64(vv);
		 	 }
		 }
		 return true;

	case AosMethod::eSwap:
	case AosMethod::ePopBack:
	case AosMethod::eInsertOne:
	case AosMethod::eInsertRange:
	case AosMethod::eErase:
	case AosMethod::eEraseRange:
		 AosSetErrorU(rdata, "not_implemented_yet") << method << enderr;
		 return false;

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecognized_method") << method << enderr;
	return false;
}

