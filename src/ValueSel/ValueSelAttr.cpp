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
#include "ValueSel/ValueSelAttr.h"

#include "Actions/ActUtil.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

static AosDocSelObjPtr sgDocSelector;


AosValueSelAttr::AosValueSelAttr(const bool reg)
:
AosValueSel(AOSACTOPRID_ATTR, AosValueSelType::eAttr, reg)
{
}


AosValueSelAttr::AosValueSelAttr(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_ATTR, AosValueSelType::eAttr, false)
{
}


AosValueSelAttr::AosValueSelAttr(
		const AosDataType::E datatype,
		const AosXmlTagPtr &doc_selector,
		const OmnString &xpath,
		const OmnString &dft)
:
AosValueSel(AOSACTOPRID_ATTR, AosValueSelType::eAttr, false),
mDataType(datatype),
mDocSelector(doc_selector),
mXPath(xpath),
mDefault(dft)
{
}


AosValueSelAttr::~AosValueSelAttr()
{
}


bool
AosValueSelAttr::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// The XML format is:
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_ATTR
	// 		AOSTAG_XPATH="xxx"
	// 		AOSTAG_DATA_TYPE="xxx">
	// 		<AOSTAG_DOCSELECTOR .../>
	// 		<default_value>xxxxx</default_value>
	// 	</valuesel>
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "valsel_attr_001") << enderr;
		return false;
	}

	// Retrieving the doc selector
	OmnString value;
	AosXmlTagPtr selector = sdoc->getFirstChild();
	if (!selector)
	{
		AosSetErrorUser(rdata, "valsel_attr_002") << sdoc->toString() << enderr;
		return false;
	}

	// Chen Ding, 04/11/2012
	if (selector->getTagname() == "default_value")
	{
		selector = sdoc->getNextChild();
		if (!selector)
		{
			AosSetErrorUser(rdata, "valsel_attr_002") << sdoc->toString() << enderr;
			return false;
		}
	}

	// if (!sgDocSelector) sgDocSelector = AosDocSelObj::getDocSelector();
	// aos_assert_r(sgDocSelector, false);
	// AosXmlTagPtr doc = sgDocSelector->doSelectDoc(selector, rdata);
	AosXmlTagPtr doc = AosRunDocSelector(rdata, selector);
	if (!doc)
	{
		AosSetErrorUser(rdata, "valsel_attr_002") << sdoc->toString() << enderr;
		return false;
	}

	// Retrieve the xpath
	OmnString xpath = sdoc->getAttrStr(AOSTAG_XPATH);
	if (xpath == "")
	{
		AosSetErrorUser(rdata, "valsel_attr_003") << sdoc->toString() << enderr;
		return false;
	}

	// Retrieve the default
	AosXmlTagPtr dft_doc = sdoc->getFirstChild("default_value");
	OmnString dft = "";
	if (dft_doc)
	{
		dft = dft_doc->getNodeText();
	}
	
	bool exist;
	value = doc->xpathQuery(xpath, exist, dft);

	// Chen Ding, 04/12/2012
	rdata->setValue(valueRslt, sdoc);
	valueRslt.setStr(value);
	return true;
}


OmnString 
AosValueSelAttr::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


OmnString
AosValueSelAttr::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &datatype,
		const OmnString &xpath,
		const OmnString &docsel,
		const OmnString &dft,
		const AosRundataPtr &rdata)
{
	// The XML format is:
	// 	<valuesel AOSTAG_VALUE_TYPE=AOSACTOPRID_ATTR
	// 		AOSTAG_XPATH="xxx"
	// 		AOSTAG_DATA_TYPE="xxx">
	// 		the default value
	// 		<docselector .../>
	// 	</valuesel>
	aos_assert_r(tagname != "", "");
	aos_assert_r(xpath != "", "");
	aos_assert_r(docsel != "", "");

	OmnString docstr = "<";
	docstr << tagname << " " << AOSTAG_VALUE_TYPE
		<< "=\"" << AOSACTOPRID_ATTR << "\""
		<< " " << AOSTAG_XPATH << "=\"" << xpath << "\"";

	if (datatype != "")
	{
		docstr << " " << AOSTAG_DATA_TYPE << "=\"" << datatype << "\"";
	}

	docstr << ">" << dft << docsel << "</" << tagname << ">";
	return docstr;
}


AosValueSelObjPtr
AosValueSelAttr::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelAttr(sdoc, rdata);
	}

	catch (...)
	{
		AosSetError(rdata, "failed_clone_object") << enderr;
		return 0;
	}
}

