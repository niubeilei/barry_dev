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
#include "ValueSel/ValueSelConst.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Random/CommonValues.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#if 0
static AosDocSelObjPtr sgDocSelector;



AosValueSelConst::AosValueSelConst(const bool reg)
:
AosValueSel(AOSACTOPRID_CONST, AosValueSelType::eConst, reg)
{
}


AosValueSelConst::AosValueSelConst(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_CONST, AosValueSelType::eConst, false)
{
}


AosValueSelConst::AosValueSelConst(
		const OmnString &value, 
		const OmnString &xpath, 
		const bool copydoc,
		const AosXmlTagPtr &doc_selector)
:
AosValueSel(AOSACTOPRID_CONST, AosValueSelType::eConst, false),
mValue(value),
mXPath(xpath),
mCopyDoc(copydoc),
mDocSelector(doc_selector)
{
}


AosValueSelConst::~AosValueSelConst()
{
}


bool
AosValueSelConst::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	// 	<valuesel 
	// 		AOSTAG_VALUE_TYPE="const" 
	// 		AOSTAG_DATA_TYPE="xxx">the-attribute-name</name>
	//
	// If AOSTAG_DATA_TYPE is not specified, it defaults to string.
	aos_assert_r(item, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(item->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		// 	<sdoc zky_xpath="xxx" zky_copy="true|false" AOSTAG_DATA_TYPE="XmlDoc">
		// 		<docselector .../>
		// 	</sdoc>
		OmnString xpath = item->getAttrStr(AOSTAG_XPATH, "");
		// Retrieving the doc selector
		AosXmlTagPtr selector = item->getFirstChild(AOSTAG_DOCSELECTOR);
		if (!selector)
		{
			rdata->setError() << "Missing Doc selector";
			return false;
		}

		// if (!sgDocSelector) sgDocSelector = AosDocSelObj::getDocSelector();
		// aos_assert_r(sgDocSelector, false);
		// AosXmlTagPtr target_doc = sgDocSelector->doSelectDoc(selector, rdata);
		AosXmlTagPtr target_doc = AosRunDocSelector(rdata, selector);
		if (!target_doc)
		{
			rdata->setError() << "Failed to retrieve the target doc";
			return false;
		}
		
		AosXmlTagPtr child;
		if (xpath == "")
		{
		    child = target_doc;
		}
		else
		{
			child = target_doc->xpathGetChild(xpath);
		}
		if (!child)
		{
			rdata->setError() << "Failed to retrieve the child: " << xpath;
			return false;
		}

		bool copydoc = item->getAttrBool(AOSTAG_COPYFLAG, true);
		if (copydoc)
		{
			OmnString docstr = child->toString();
			AosXmlParser parser;
			child = parser.parse(docstr, "" AosMemoryCheckerArgs);
		}
		valueRslt.setXml(child);
		return true;
	}

	OmnString vv = item->getNodeText();
	return valueRslt.setValue(datatype, vv, rdata.getPtrNoLock());
}


OmnString 
AosValueSelConst::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString xml = "<";
	OmnString tname = getTagnameStatic(tagname, rule);
	xml << tname << " ";
	addHeader(xml, tname, AOSACTOPRID_CONST, rule);

	xml << ">" << AosCommonValues::pickAttrValue()
		<< "</" << tname << ">";
	return  xml;
}


OmnString
AosValueSelConst::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &values,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(tagname != "", rdata, "");
	OmnString docstr = "<";
	docstr << tagname << " " << AOSTAG_VALUE_TYPE << "=\"" << AOSACTOPRID_CONST
		<< "\" " << AOSTAG_DATA_TYPE << "=\"" << AosValueRslt::getStringDataType() 
		<< "\"><![CDATA[" << values << "]]></" << tagname << ">";
	return docstr;
}


AosValueSelObjPtr
AosValueSelConst::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelConst(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
