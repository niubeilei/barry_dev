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
// This valueset bases on XML. It assumes the following:
// 	<xml ...>
// 		<root_tag>
// 			<entry ... attrname="xxx" ../>
// 			<entry ... attrname="xxx" ../>
// 			...
// 		</root_tag>
// 	</xml>
// The value is stored in <root_tag>. Each subtag has 'attrname'. 
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/ValuesetXml.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Value/Value.h"
#include "XmlUtil/XmlTag.h"



AosValuesetXml::AosValuesetXml(const OmnString &version)
:
AosValueset(AOS_VALUESET_TYPE_XML, version)
{
	init();
}


AosValuesetXml::~AosValuesetXml()
{
}


void
AosValuesetXml::init()
{
	mAttrname = "";
	mRootTagname = "";
	mValue = 0;
	mValueRaw = 0;
	mXmlDoc = 0;
	mReadLoop = 0;
	mWriteLoop = 0;
	mRootTag = 0;
	mRootTagRaw = 0;
}


bool
AosValuesetXml::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	init();

	if (!worker_doc)
	{
		AosSetErrorUser(rdata, "valuesetxml_missing_worker_doc") << enderr;
		return false;
	}

	mAttrname = worker_doc->getAttrStr("value_attrname");
	if (mAttrname == "")
	{
		AosSetErrorUser(rdata, "valuesetxml_missing_attrname") << enderr;
		return false;
	}

	mRootTagname = worker_doc->getAttrStr("root_tagname");
	mRootTagname = worker_doc->getAttrStr("entry_tagname");

	AosXmlTagPtr tag = worker_doc->getFirstChild("value_def");
	if (!tag)
	{
		AosSetErrorUser(rdata, "valuesetxml_missing_value_def") << enderr;
		return false;
	}

	mValue = AosCreateValue(rdata, tag);
	if (!mValue)
	{
		AosSetErrorUser(rdata, "valuesetxml_failed_creating_value") 
			<< rdata->getErrmsg() << enderr;
		return false;
	}
	mValueRaw = mValue.getPtr();

	return true;	
}


AosValue *
AosValuesetXml::nextValue(const AosRundataPtr &rdata)
{
	// It retrieves the next value.
	// 1. If mReadLoop is null, it is to get the first child.
	//    Otherwise, it is to get the next child
	// 2. If the next child tag is null, return null.
	// 3. Otherwise, retrieve the value
	aos_assert_rr(mValueRaw, rdata, 0);
	AosXmlTagPtr tag;
	if (!mReadLoop)
	{
		if (!mRootTagRaw) return 0;
		mReadLoop = mRootTagRaw;
		tag = getFirstChild();
	}
	else
	{
		tag = getNextChild();
	}

	if (!tag) return 0;
	mValueRaw->retrieveValue(rdata, tag, mAttrname);
	return mValueRaw;
}


bool
AosValuesetXml::resetReadLoop()
{
	mReadLoop = 0;
	return true;
}


bool
AosValuesetXml::setData(
		const AosRundataPtr &rdata, 
		const AosValuePtr &data)
{
	aos_assert_rr(data, rdata, false);
	mXmlDoc = data->getXml();
	aos_assert_rr(mXmlDoc, rdata, false);

	mRootTag = mXmlDoc->xpathGetFirstChild(mRootTagname);
	mRootTagRaw = mRootTag.getPtr();
	mReadLoop = 0;
	mWriteLoop = 0;
	return true;
}


AosJimoPtr
AosValuesetXml::cloneJimo() const
{
	try
	{
		return OmnNew AosValuesetXml(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

