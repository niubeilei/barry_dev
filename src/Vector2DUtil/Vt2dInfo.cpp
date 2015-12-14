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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "Vector2DUtil/Vt2dInfo.h"
#include "Debug/Debug.h"


AosVt2dInfo::AosVt2dInfo()
:
mGrpbyTimeUnit(AosStatTimeUnit::eInvalid),
mHasValidFlag(true)
{
}

int
AosVt2dInfo::getMeasureIdx(const OmnString &name)
{
	for(u32 i=0; i<mMeasures.size(); i++)
	{
		if(mMeasures[i].mName == name)	return i;
	}
	return -1;
}


void
AosVt2dInfo::initVt2dValueDef(AosMeasureValueDef &value_def)
{
	value_def.mHasValidFlag = mHasValidFlag; 
	for(u32 i=0; i<mMeasures.size(); i++)
	{
		value_def.add(mMeasures[i].mDataType);
	}
}


bool
AosVt2dInfo::config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	mVt2dName = conf->getAttrStr("vt2d_name", "");
	aos_assert_r(mVt2dName != "", false);
	
	mTimeFieldName = conf->getAttrStr("time_field_name", "");
	if(mTimeFieldName != "")
	{
		OmnString unit_str = conf->getAttrStr("grpby_time_unit", "");
		mGrpbyTimeUnit = AosStatTimeUnit::getTimeUnit(unit_str);
		aos_assert_r(mGrpbyTimeUnit != AosStatTimeUnit::eInvalid, false);	
	}

	mHasValidFlag = conf->getAttrBool("has_valid_flag", true);
	mValueDef.mHasValidFlag = mHasValidFlag;

	AosXmlTagPtr measures_conf = conf->getFirstChild("measures", true);
	aos_assert_r(measures_conf, false);

	MeasureInfo m_info;
	AosXmlTagPtr entry_conf = measures_conf->getFirstChild(true);
	while(entry_conf)
	{
		m_info.config(rdata, entry_conf);
		
		mMeasures.push_back(m_info);
		mValueDef.add(m_info.mDataType);

		entry_conf = measures_conf->getNextChild();
	}
	
	//AosXmlTagPtr value_def_conf = conf->getFirstChild("m_value_def", true);
	//aos_assert_r(value_def_conf, false);
	//bool rslt = mValueDef.config(rdata, value_def_conf);
	//aos_assert_r(rslt, false);

	return true;
}

OmnString
AosVt2dInfo::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<vt2d_info ";
	str << "vt2d_name=\"" << mVt2dName << "\" ";
	if(mTimeFieldName != "")
	{
		str << "time_field_name=\"" << mTimeFieldName << "\" "
			<< "grpby_time_unit=\"" << AosStatTimeUnit::toStr(mGrpbyTimeUnit) << "\" ";
	}
	str	<< "has_valid_flag=\"" << (mHasValidFlag ? "true" : "false") << "\" "
		<< ">"
		<< "<measures>";
	for(u32 i=0; i<mMeasures.size(); i++)
	{
		str << mMeasures[i].toXmlString(rdata);
	}
	str << "</measures>"
		<< "</vt2d_info>";
	return str;
}


u64
AosVt2dInfo::getDistCountMeasureNum()
{
	u64 num = 0;
	for(int i=0;i<mMeasures.size();i++)
	{
		if(mMeasures[i].mName.hasPrefix("dist_count"))
			num++;
	}
	return num;
}
