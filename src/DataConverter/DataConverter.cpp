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
// 2013/08/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataConverter/DataConverter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"


AosDataConverter::AosDataConverter(
		const OmnString &type,
		const OmnString &version)
:
AosDataConverterObj(version),
mConverterType(type)
{
}

AosDataConverter::~AosDataConverter()
{
}


bool 
AosDataConverter::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (!worker_doc) return true;

	mRootTagName = worker_doc->getAttrStr("root_tag_name");

	return true;
}


AosXmlTagPtr 
AosDataConverter::getRootTag(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &xml)
{
	// This function retrieves the input root tag. 
	if (mRootTagName == "") return xml;
	return xml->xpathGetFirstChild(mRootTagName);
}


AosXmlTagPtr
AosDataConverter::nextValueTag(
		const AosRundataPtr &rdata, 
		const OmnString &member_tagname,
		const AosXmlTagPtr &input_xml,
		const AosXmlTagPtr &root_tag)
{
	aos_assert_rr(root_tag, rdata, 0);

	AosXmlTagPtr tag;
	if (member_tagname != "")
	{
		while ((tag = root_tag->getNextChild(member_tagname)))
		{
			if (!mInputTagSelector) return tag;
			
			// It uses the selector to select the tag.
			rdata->setArg1("root_tag", root_tag);
			rdata->setArg1("input_xml", input_xml);
			bool rslt = mInputTagSelector->run(rdata);
			aos_assert_rr(rslt, rdata, false);
			if (rdata->getArg1("expr_result") == "true") return tag;
		}
		return 0;
	}
	
	while ((tag = root_tag->getNextChild()))
	{
		if (!mInputTagSelector) return tag;
			
		// It uses the selector to select the tag.
		rdata->setArg1("root_tag", root_tag);
		rdata->setArg1("input_xml", input_xml);
		bool rslt = mInputTagSelector->run(rdata);
		aos_assert_rr(rslt, rdata, false);
		if (rdata->getArg1("expr_result") == "true") return tag;
	}

	return 0;
}


bool
AosDataConverter::getInputValue(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &tag, 
		bool &is_missing)
{
}

