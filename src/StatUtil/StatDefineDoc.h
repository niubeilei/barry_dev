////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/04/02	Created by Ketty 
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatUtil_StatDefineDoc_h
#define StatUtil_StatDefineDoc_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "StatUtil/StatTimeUnit.h"
#include "StatUtil/MeasureInfo.h"

#include <vector>
using namespace std;

class AosStatDefineDoc : public OmnRCObject 
{
	OmnDefineRCObject;

private:
	OmnString		mStatIdentifyKey;

	AosXmlTagPtr	mKeyFieldsConf;
	AosXmlTagPtr	mTimeFieldConf;
	AosXmlTagPtr	mKeyConnConf;
	AosXmlTagPtr	mVt2dConnConf;
	AosXmlTagPtr	mMeasuresConf;
	AosXmlTagPtr	mCondConf;
	AosXmlTagPtr	mShuffleFieldsConf;
	

public:
	AosStatDefineDoc();
	~AosStatDefineDoc();

	bool 	config(const AosXmlTagPtr &conf);

	OmnString getStatIdentifyKey(){ return mStatIdentifyKey; };

	AosXmlTagPtr getKeyFieldsConf(){ return mKeyFieldsConf; };

	AosXmlTagPtr getKeyConnConf(){ return mKeyConnConf; };

	AosXmlTagPtr getVt2dConnConf(){ return mVt2dConnConf; };

	AosXmlTagPtr getCondConf(){return mCondConf;}

	AosXmlTagPtr getShuffleFieldsConf(){return mShuffleFieldsConf;};

	OmnString getTimeFieldName()
	{
		if(!mTimeFieldConf)	return "";
		return mTimeFieldConf->getAttrStr("time_field_name", "");
	}

	OmnString getTimeFormat()
	{
		if(!mTimeFieldConf)	return "";
		return mTimeFieldConf->getAttrStr("time_format", "");
	}
	
	AosStatTimeUnit::E getGrpbyTimeUnit()
	{
		if(!mTimeFieldConf)	return AosStatTimeUnit::eInvalid;
		OmnString unit_str = mTimeFieldConf->getAttrStr("grpby_time_unit", "");
		return AosStatTimeUnit::getTimeUnit(unit_str);
	}

	bool	getMeasureInfo(const AosRundataPtr &rdata, vector<MeasureInfo> &m_infos);

};

#endif
