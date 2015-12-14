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
#ifndef AOS_StatUtil_StatCond_h
#define AOS_StatUtil_StatCond_h


#include "Util/Opr.h"
#include "Util/String.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatField.h"

struct AosStatCond
{
	AosStatFieldPtr mStatField;
	AosOpr		mCondOpr;
	OmnString	mCondValue1;
	OmnString	mCondValue2;
	OmnString	mFiltersConf;
	AosXmlTagPtr	mFiltersXml;

	bool	config(const AosXmlTagPtr &conf)
	{
		aos_assert_r(conf, false);

		OmnString cond_opr_str = conf->getNodeText("cond_opr");
		mCondOpr = AosOpr_toEnum(cond_opr_str);
		
		mCondValue1 = conf->getNodeText("cond_value1");
		mCondValue2 = conf->getNodeText("cond_value2");
		
		AosXmlTagPtr field_conf = conf->getFirstChild("field");
		aos_assert_r(field_conf, false);
		mStatField = AosStatField::configStatic(field_conf);
		aos_assert_r(mStatField, false);
		mFiltersXml = conf->getFirstChild("filters");	
		return true;	
	}

	OmnString toXmlString()
	{
		OmnString str;
		str << "<cond>"
			<< "<cond_opr><![CDATA[" << AosOpr_toStr(mCondOpr) << "]]></cond_opr>"
			<< "<cond_value1><![CDATA[" << mCondValue1 << "]]></cond_value1>"
			<< "<cond_value2><![CDATA[" << mCondValue2 << "]]></cond_value2>"
			<< mStatField->toXmlString()
			<< "<filters>" <<  mFiltersConf << "</filters>"
			<< "</cond>";
		return str;
	}
	
	bool	equal(AosStatCond &rhs)
	{
		if(mStatField->getFieldName() != rhs.mStatField->getFieldName())	return false;	
		if(mCondOpr != rhs.mCondOpr)	return false;
		if(mCondValue1 != rhs.mCondValue1)	return false;
		if(mCondValue2 != rhs.mCondValue2)	return false;
		if(mFiltersConf != rhs.mFiltersConf)    return false;
		return true;
	}

};

#endif
