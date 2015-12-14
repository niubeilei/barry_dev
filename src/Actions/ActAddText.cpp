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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 03/05/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActAddText.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "SEModules/ObjMgr.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

AosActAddText::AosActAddText(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDTEXT, AosActionType::eAddText, flag)
{
}

AosActAddText::~AosActAddText()
{
}


bool	
AosActAddText::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_XPATH="xxx">
	// 		<docselector .../>
	// 		<value .../>
	// 	</sdoc>
	// where "<docselector>" is a doc selector that selects a doc; AOSTAG_XPATH
	// specifies the attribute, "<value>" specifies how to retrieve the
	// value. If AOSTAG_VALUE_UNIQUE is not specified, it defaults to false.
	// If AOSTAG_DOCID_UNIQUE is not specified, it defaults to false.
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the source doc
	AosXmlTagPtr selector = sdoc->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
	 	AosSetError(rdata, "eSmartDocIncorrect");
	 	OmnAlarm << rdata->getErrmsg() << ". Def: " << sdoc->toString() << enderr;
	 	return false;
	}
	 
	AosXmlTagPtr targetDoc = AosRunDocSelector(rdata, selector);
	if (!targetDoc)
	{
		AosSetEntityError(rdata, "actaddtext_missing_source_doc", 
			"SmartDoc", sdoc->getAttrStr(AOSTAG_OBJID)) << sdoc->toString() << enderr;
	 	return false;
	}

	AosXmlTagPtr doc = targetDoc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_rr(doc, rdata, false);

	// Retrieve the value
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_VALUE_SELECTOR, rdata);
	aos_assert_rr(rslt, rdata, false);

	// The data type cannot be XML DOC
	if (!value.isNull())
	{
		AosSetError(rdata, "eValueIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	/*
	if (value.isXmlDoc())
	{
		AosSetError(rdata, "eValueIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}
	*/

	// Retrieve xpath, value_unique, and docid_unique
	OmnString xpath = sdoc->getAttrStr(AOSTAG_XPATH);
	if (xpath == "")
	{
		AosSetError(rdata, "eSmartDocIncorrect");
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	bool checkSecurity = sdoc->getAttrBool(AOSTAG_CHECK_ACCESS, true);
	OmnString newvalue = value.getStr();
	doc->setNodeText(xpath, newvalue, true);
	
	u64 userid = 0;
	if (!checkSecurity) userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));

	// Ready to modify the doc.
	rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "false", false);

	if (!checkSecurity) rdata->setUserid(userid);

	if (!rslt)
	{
		return false;
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActAddText::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddText(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

