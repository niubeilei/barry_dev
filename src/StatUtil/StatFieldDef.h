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
// 2014/02/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatUtil_StatFieldDef_h
#define Aos_StatUtil_StatFieldDef_h

#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "StatUtil/StatTimeUnit.h"
#include "JQLExpr/ExprGenFunc.h"

struct AosStatFieldDef
{
	enum Type
	{
		eInvalid = 0,
	
		eTime,
		eStatKey,
		eMeasure,
	};

	OmnString	mFname;
	Type		mFieldType;
	AosDataType::E mDataType;
	int			mDataSize;
	int			mFieldPos;
	
	void	reset(const OmnString &fname, const Type &ftype,
				const AosDataType::E data_type, const int fpos)
	{
		mFname = fname;
		mFieldType = ftype;
		mDataType = data_type;
		mDataSize = AosDataType::getValueSize(mDataType);
		mFieldPos = fpos;
	}

	OmnString toXmlString()
	{
		OmnString str = "<field_def ";
		str << "field_name=\"" << mFname << "\" "
			<< "field_type=\"" << getFieldTypeStr() << "\" "
			<< "zky_data_type=\"" << AosDataType::getTypeStr(mDataType) << "\" "
			<< "zky_data_pos=\"" << mFieldPos << "\" "
			<< "/>";
		return str;
	}
	
	bool config(const AosXmlTagPtr &conf)
	{
		mFname = conf->getAttrStr("field_name", "");
		aos_assert_r(mFname != "", false);
		
		OmnString field_type_str = conf->getAttrStr("field_type", "");
		mFieldType = fieldTypeStrToEnum(field_type_str);
		aos_assert_r(mFieldType != eInvalid, false); 
		
		OmnString data_type_str = conf->getAttrStr("zky_data_type", "");
		mDataType = AosDataType::toEnum(data_type_str);
		mDataSize = AosDataType::getValueSize(mDataType);
		
		mFieldPos = conf->getAttrInt("zky_data_pos", -1);
		aos_assert_r(mFieldPos != -1, false);
		return true;
	}


	OmnString getFieldTypeStr()
	{
		switch(mFieldType)
		{
		case eTime:
			return "stat_time";
		
		case eStatKey:
			return "stat_key";

		case eMeasure:
			return "stat_measure";

		default:
			return "";
		}
		return "";
	}
	
	Type fieldTypeStrToEnum(const OmnString str)
	{
		if(str == "stat_time")	return eTime;
		if(str == "stat_key")	return eStatKey;
		if(str == "stat_measure")	return eMeasure;
		return eInvalid;
	}

};

/*
struct AosStatFieldDef
{
	enum Type
	{
		eInvalid = 0,
		eTime,
		eStatKey,
		eMeasure,
	};

	OmnString	mFname;
	u32			mFieldIdx;
	
	//u32			mStatFieldIdx;
	Type		mFieldType;
	int			mKeyIdx;
	int			mVt2dIdx;
	int			mMeasureValueIdx;

	AosDataType::E mDataType;
	int			mDataSize;
	int			mFieldPos;

	OmnString toXmlString()
	{
		OmnString str = "<field_def ";
		str << "field_name=\"" << mFname << "\" "
			<< "field_idx=\"" << mFieldIdx << "\" "
			//<< "stat_field_idx=\"" << mStatFieldIdx << "\" "
			<< "zky_data_type=\"" << AosDataType::getTypeStr(mDataType) << "\" "
			<< "field_type=\"" << getFieldTypeStr() << "\" ";
		if(mFieldType == eStatKey)
		{
			str << "key_idx=\"" << mKeyIdx << "\" ";
		}
		else if (mFieldType == eMeasure)
		{
			str << "vt2d_idx=\"" << mVt2dIdx << "\" "
				<< "measure_value_idx=\"" << mMeasureValueIdx << "\" ";
		}
		else
		{
			aos_assert_r(mFieldType == eTime, "");
		}

		str	<< "/>";
		return str;
	}

	bool config(const AosXmlTagPtr &conf)
	{
		mFname = conf->getAttrStr("field_name", "");
		aos_assert_r(mFname != "", false);
		
		mFieldIdx = conf->getAttrInt("field_idx", -1);	
		aos_assert_r(mFieldIdx >=0, false);

		OmnString field_type_str = conf->getAttrStr("field_type", "");
		mFieldType = fieldTypeStrToEnum(field_type_str);
		aos_assert_r(mFieldType != eInvalid, false); 
		
		mKeyIdx = conf->getAttrInt("key_idx", -1);	
		mVt2dIdx = conf->getAttrInt("vt2d_idx", -1);	
		mMeasureValueIdx = conf->getAttrInt("measure_value_idx", -1);

		
		OmnString data_type_str = conf->getAttrStr("zky_data_type", "");
		mDataType = AosDataType::toEnum(data_type_str);
		//mStatFieldIdx = conf->getAttrInt("stat_field_idx", -1);
		mDataSize = AosDataType::getValueSize(mDataType);
		mFieldPos = -1;

		return true;
	}

	OmnString getFieldTypeStr()
	{
		switch(mFieldType)
		{
		case eTime:
			return "stat_time";
		
		case eStatKey:
			return "stat_key";

		case eMeasure:
			return "stat_measure";

		default:
			return "";
		}
		return "";
	}
	
	Type fieldTypeStrToEnum(const OmnString str)
	{
		if(str == "stat_time")	return eTime;
		if(str == "stat_key")	return eStatKey;
		if(str == "stat_measure")	return eMeasure;
		return eInvalid;
	}
};
*/

#endif

