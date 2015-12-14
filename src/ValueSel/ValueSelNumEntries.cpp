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
#include "ValueSel/ValueSelNumEntries.h"

#include "API/AosApi.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Conds/Condition.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocSelObj.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#if 0
AosValueSelNumEntries::AosValueSelNumEntries(const bool reg)
:
AosValueSel(AOSACTOPRID_NUMENTRIES, AosValueSelType::eNumEntries, reg)
{
}


AosValueSelNumEntries::AosValueSelNumEntries(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_NUMENTRIES, AosValueSelType::eNumEntries, false)
{
}


AosValueSelNumEntries::~AosValueSelNumEntries()
{
}

bool
AosValueSelNumEntries::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	//It retrieves the number of entries of a given subtag of a given doc:
	//<objselector AOSTAG_VALUE_TYPE = "numentries" xpath="xxx"  AOSTAG_DOCSELECTOR_TYPE="createdoc/docid/localvar/objid"/>
	//<entries>
	//		<entry …/>
	//		<entry …/>
	//		<entry …/>
	//		<entry …/>
	//</entries>
	//
	OmnString value;
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

	// AosDocSelObjPtr doc_selector = AosDocSelObj::getDocSelector();
	// aos_assert_r(doc_selector, false);
	// AosXmlTagPtr doc = doc_selector->doSelectDoc(selector, rdata);
	AosXmlTagPtr doc = AosRunDocSelector(rdata, selector);
	if (!doc)
	{
		rdata->setError() << "Failed to retrieve the target doc";
		return false;
	}
	if (doc->isRootTag()) doc = doc->getFirstChild();
	aos_assert_r(doc, false);

	OmnString path = item->getAttrStr(AOSTAG_XPATH);
	if (path != "") 
	{
		OmnStrParser1 parser(path, "/", false, false);
		OmnString word;
		while ((word = parser.nextWord()) != "")
		{
			doc = doc->getFirstChild(word);
			if (!doc)
			{
				rdata->setError() << "Missing path";
				return false;
			}
		}
		aos_assert_r(doc, false);
	}
	value = "";
	AosXmlTagPtr condition = item->getFirstChild(AOSTAG_CONDITION);
	if (!condition)
	{
		value <<doc->getNumSubtags();
		return valueRslt.setValue(datatype, value, rdata.getPtrNoLock());
	}
	int sum = 0;
	AosXmlTagPtr entry = doc->getFirstChild(); 
	AosXmlTagPtr sourcedoc = rdata->getSourceDoc();
	while(entry)
	{
		rdata->setSourceDoc(entry, false);
		bool rslt = AosCondition::evalCondStatic(condition, rdata);
		if (rslt) sum = sum +1; 
		entry = doc->getNextChild();
	}
	value << sum;
	rdata->setSourceDoc(sourcedoc, false);
	return valueRslt.setValue(datatype, value, rdata.getPtrNoLock());
}


OmnString 
AosValueSelNumEntries::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelNumEntries::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelNumEntries(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
