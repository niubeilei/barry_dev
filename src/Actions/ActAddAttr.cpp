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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActAddAttr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

AosActAddAttr::AosActAddAttr(const bool flag)
:
AosSdocAction(AOSACTTYPE_ADDATTR, AosActionType::eAddAttr, flag)
{
}


AosActAddAttr::~AosActAddAttr()
{
}


bool	
AosActAddAttr::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_XPATH="xxx"
	// 		  AOSTAG_VALUE_UNIQUE="true|false"
	// 		  AOSTAG_DOCID_UNIQUE="true|false">
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
		AosSetErrorU(rdata, "missing_source_doc") << enderr;
	 	return false;
	}

	// Retrieve the value
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_VALUE_SELECTOR, rdata);
	aos_assert_r(rslt, false);

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

	bool value_unique = sdoc->getAttrBool(AOSTAG_VALUE_UNIQUE, false);
	bool docid_unique = sdoc->getAttrBool(AOSTAG_DOCID_UNIQUE, false);
	bool checkSecurity = sdoc->getAttrBool(AOSTAG_CHECK_ACCESS, true);

	// Ready to modify the doc.
	//  Ice, 09/14/2011
	OmnString newvalue = value.getStr();
	if (!AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
				targetDoc->getAttrU64(AOSTAG_DOCID, 0), targetDoc->getAttrStr(AOSTAG_OBJID, ""),
				xpath, newvalue, "", value_unique, docid_unique, checkSecurity))
	{
		return false;
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActAddAttr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActAddAttr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

