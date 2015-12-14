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
// 2014/04/02	Created by Ketty 
//
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatDefineDoc.h"

#include "API/AosApi.h"

AosStatDefineDoc::AosStatDefineDoc()
{
	mStatIdentifyKey = NULL;
	mKeyFieldsConf = NULL;
	mTimeFieldConf = NULL;
	mKeyConnConf = NULL;
	mVt2dConnConf = NULL;
	mMeasuresConf = NULL;
	mCondConf = NULL;
	mShuffleFieldsConf = NULL;
}

AosStatDefineDoc::~AosStatDefineDoc()
{
}


bool
AosStatDefineDoc::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	
	mStatIdentifyKey = conf->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(mStatIdentifyKey != "", false);

	mKeyFieldsConf = conf->getFirstChild("key_fields"); 
	aos_assert_r(mKeyFieldsConf, false); 

	// maybe no time_field.
	mTimeFieldConf = conf->getFirstChild("time_field"); 
	//aos_assert_r(mTimeFieldConf, false); 

	mKeyConnConf = conf->getFirstChild("stat_key_conn");
	aos_assert_r(mKeyConnConf, false); 
	
	mVt2dConnConf = conf->getFirstChild("vt2d_conn");
	aos_assert_r(mVt2dConnConf, false); 

	mMeasuresConf = conf->getFirstChild("measures");
	aos_assert_r(mMeasuresConf, false); 

	mCondConf = conf->getFirstChild("cond");
	
	mShuffleFieldsConf = conf->getFirstChild("shuffle_fields");
	return true;
}


bool
AosStatDefineDoc::getMeasureInfo(const AosRundataPtr &rdata, vector<MeasureInfo> &m_infos)
{
	aos_assert_r(mMeasuresConf, false);
	
	bool rslt;
	MeasureInfo info;	
	AosXmlTagPtr each_conf = mMeasuresConf->getFirstChild();
	while(each_conf)
	{
		rslt = info.config(rdata, each_conf);
		aos_assert_r(rslt, false);

		m_infos.push_back(info);	
		each_conf = mMeasuresConf->getNextChild();
	}
	return true;
}


