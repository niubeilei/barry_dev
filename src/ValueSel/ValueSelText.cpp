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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelText.h"

#include "API/AosApi.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelText::AosValueSelText(const bool reg)
:
AosValueSel(AOSACTOPRID_TEXT, AosValueSelType::eText, reg)
{
}


AosValueSelText::AosValueSelText(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_TEXT, AosValueSelType::eText, false)
{
}


AosValueSelText::~AosValueSelText()
{
}

bool
AosValueSelText::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//<creator AOSTAG_VALUE_TYPE="text" path="xxx" AOSTAG_DATA_TYPE="xxx"/>
	//<docselector AOSTAG_DOCSELECTOR_TYPE="xxx"/>
	//</creator>
	aos_assert_r(item, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		rdata->setError() << "Datatype incorrect: " << datatype;
		return false;
	}

	// Retrieving the doc selector
	AosXmlTagPtr selector = item->getFirstChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
		rdata->setError() << "Missing Doc selector";
		return false;
	}

	// Retrieve the doc to be modified
	// AosDocSelObjPtr doc_selector = AosDocSelObj::getDocSelector();
	// aos_assert_r(doc_selector, false);
	// AosXmlTagPtr doc = doc_selector->doSelectDoc(selector, rdata);
	AosXmlTagPtr doc = AosRunDocSelector(rdata, selector);
	if (!doc)
	{
		rdata->setError() << "Failed to retrieve doc!";
		return false;
	}

	// Retrieve the xpath
	OmnString xpath = item->getAttrStr(AOSTAG_XPATH);
	if (xpath == "" || xpath == "_ROOT")
	{
		rdata->setError() << "Missing xpath!";
		return false;
	}
	
	// Retrieve the default
	OmnString dft = item->getNodeText();
	
	bool exist;
	OmnString str = xpath;
	str << "/_$text";
	aos_assert_r(str !="", "");
	OmnString value = doc->xpathQuery(str, exist, dft);

	valueRslt.setStr(value);
	return true;
}


OmnString 
AosValueSelText::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelText::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelText(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}







