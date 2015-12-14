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
// This valueset is similar to AosValuesetXmlTagSel except that it has
// a tag selector. 
//
// Modification History:
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Valueset/ValuesetXmlTagSel.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "Value/Value.h"
#include "XmlUtil/XmlTag.h"



AosValuesetXmlTagSel::AosValuesetXmlTagSel(const OmnString &version)
:
AosValueset(AOS_VALUESETTYPE_XML_TAGSELECTOR, version)
{
	init();
}


AosValuesetXmlTagSel::~AosValuesetXmlTagSel()
{
}


void
AosValuesetXmlTagSel::init()
{
	AosValuesetXml::init();
}


bool
AosValuesetXmlTagSel::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	init();

	if (!AosValuesetXml::config(rdata, worker_doc, jimo_doc)) return false;

	AosXmlTagPtr expr_tag = worker_doc->getFirstChild("tag_selector");
	if (!expr_tag)
	{
		AosSetErrorUser(rdata, "valuesetxmltagsel_missing_tag_selector") << enderr;
		return false;
	}

	mExpr = AosCreateExpr(rdata, expr_tag);
	if (!mExpr)
	{
		AosSetErrorUser(rdata, "valuesetxmltagsel_failed_creating_expr") << enderr;
		return false;
	}

	mEntryTagname = worker_doc->getAttrStr("entry_tagname");

	return true;	
}


AosValue *
AosValuesetXmlTagSel::nextValue(const AosRundataPtr &rdata)
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

	while (tag)
	{
		AosJimoPtr thisptr(this, false);
		rdata->setCaller(thisptr);
		bool vv;
		if (!mExpr->getValueBool(rdata, vv))
		{
			AosSetErrorUser(rdata, "valuesetxmltagsel_interr_001") << enderr;
			return false;
		}

		if (mWhitelist)
		{
			if (vv) 
			{
				if (mValueRaw->retrieveValue(rdata, tag, mAttrname)) return mValueRaw;
			}
		}
		else
		{
			if (!vv)
			{
				if (mValueRaw->retrieveValue(rdata, tag, mAttrname)) return mValueRaw;
			}
		}

		tag = getNextChild();
	}
	return 0;
}


bool
AosValuesetXmlTagSel::resetReadLoop()
{
	mReadLoop = 0;
	return true;
}


bool
AosValuesetXmlTagSel::setData(
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
AosValuesetXmlTagSel::cloneJimo() const
{
	try
	{
		return OmnNew AosValuesetXmlTagSel(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

