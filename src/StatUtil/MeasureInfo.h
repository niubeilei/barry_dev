////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_MeasureInfo_h
#define AOS_StatUtil_MeasureInfo_h

#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/Number.h"
#include "Util/DataTypes.h"
#include "SEInterfaces/AggrFuncObj.h"

struct MeasureInfo
{
	OmnString				mName;
	OmnString				mFieldName;
	//if datatype is number, we may also need scale and precision
	AosDataType::E			mDataType;	
	//only when mDataType = eNumber, will mNumber be used
	AosNumber			    mNumber;	
	OmnString				mAggrFuncStr;
	AosAggrFuncObjPtr   	mAggrFunc;
	AosAggrFuncObj*     	mAggrFuncRaw; 
	//arvin 2015.07.29
	//JIMODB-114
	AosDataType::E 			mType;
	AosExprObjPtr			mCond;
	OmnString				mCondMeasureName;
	//jimodb-692 Rain 2015/09/11
	//for accumulate aggregration
	bool					mIsAccumulate;

	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
	{
		mIsAccumulate = false;

		aos_assert_r(conf, false);
		mName = conf->getAttrStr("zky_name", "");
		aos_assert_r(mName != "", false);
			
		mAggrFuncStr = conf->getAttrStr("agr_type", "");	
		aos_assert_r(mAggrFuncStr != "", false);
		
		mFieldName = conf->getAttrStr("field_name", "");
		aos_assert_r(mFieldName != "", false);
		//arvin 2015.07.29
		OmnString type_str = conf->getAttrStr("type","");
		mType = AosDataType::toEnum(type_str);

		OmnString data_type_str = conf->getAttrStr("zky_data_type", "");
		mDataType = AosDataType::toEnum(data_type_str);

		if (mDataType == AosDataType::eNumber)
		{
			OmnString str;
			u32 dft = 0;

			str = conf->getAttrStr("zky_data_total_digits", "18");
			mNumber.setTotalDigits(str.parseU32(dft));

			str = conf->getAttrStr("zky_data_small_digits", "2");
			mNumber.setSmallDigits(str.toInt());
		}
#if 0
		//handle accumulate measure
		mIsAccumulate= conf->getAttrBool("accumulate_flag", false);
		if (mAggrFuncStr.hasPrefix("accu_"))
		{
			mAggrFuncStr = mAggrFuncStr.substr(5, 0);
			mIsAccumulate = true;
		}
#endif
		
		return true;
	}

	bool configFunc(const AosRundataPtr &rdata)
	{
		OmnString aggrStr;

		aggrStr = mAggrFuncStr;
		mIsAccumulate = false;
		if (aggrStr.hasPrefix("accu_"))
		{
			aggrStr = aggrStr.substr(5, 0);
			mIsAccumulate = true;
		}

		mAggrFunc = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggrStr);
		mAggrFuncRaw = mAggrFunc.getPtr();

		return true;
	}
	
	OmnString toXmlString(const AosRundataPtr &rdata)
	{
		OmnString str = "<measure ";
		str << "zky_name=\"" << mName << "\" "
			<< "field_name=\"" << mFieldName << "\" "
			<< "agr_type=\"" << mAggrFuncStr << "\" "
			<< "accumulate_flag=\"" << mIsAccumulate << "\" "
			<< "zky_data_type=\"" << AosDataType::getTypeStr(mDataType) << "\" "
			<< "type=\"" << AosDataType::getTypeStr(mType) << "\" ";	
		if (mDataType == AosDataType::eNumber)
		{
			str << "zky_data_total_digits=\"" << mNumber.getTotalDigits() << "\" "
				<< "zky_data_small_digits=\"" << mNumber.getSmallDigits() << "\" ";
		}
		
		str << "></measure>";
		return str;
	}

};


#endif
