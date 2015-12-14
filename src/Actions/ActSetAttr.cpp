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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSetAttr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
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
#if 0

AosActSetAttr::AosActSetAttr(const bool flag)
:
AosSdocAction(AOSACTTYPE_SETATTR, AosActionType::eSetAttr, flag)
{
}


AosActSetAttr::~AosActSetAttr()
{
}


bool	
AosActSetAttr::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_ZKY_TYPE=AOSACTTYPE_SETATTR
	// 		AOSTAG_XPATH="xxx"
	// 		<AOSTAG_DOCSELECTOR .../>
	// 		<AOSTAG_VALUE_SELECTOR .../>
	// 	</sdoc>
	// where "<docselector>" is a doc selector that selects a doc; AOSTAG_XPATH
	// specifies the attribute, "<value>" specifies how to retrieve the
	// value. 
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the source doc
	AosXmlTagPtr targetDoc = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCSELECTOR);
	if (!targetDoc)
	{
		AosSetErrorU(rdata, "failed_retrieve_doc");
	 	return false;
	}

	// Retrieve the value
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_VALUE_SELECTOR, rdata);
	aos_assert_rr(rslt, rdata, false);

	// The data type cannot be XML DOC
	if (!value.isNull())
	{
		AosSetError(rdata, AosErrmsgId::eValueIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	if (value.isXmlDoc())
	{
		AosSetError(rdata, AosErrmsgId::eValueIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	// Retrieve xpath, value_unique, and docid_unique
	OmnString xpath = sdoc->getAttrStr(AOSTAG_XPATH);
	if (xpath == "")
	{
		//modify by Jozhi
		AosValueRslt path_value;
		bool rs = AosValueSel::getValueStatic(path_value, sdoc, "zky_varpath", rdata);
		aos_assert_rr(rs, rdata, false);
		if (!path_value.isNull())
		{
			AosSetError(rdata, AosErrmsgId::eValueIncorrect);
			OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
			return false;
		}

		if (path_value.isXmlDoc())
		{
			AosSetError(rdata, AosErrmsgId::eValueIncorrect);
			OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
			return false;
		}

		xpath = path_value.getStr();
		///////////////end///////////////////////
		if (xpath == "")
		{
			AosSetError(rdata, AosErrmsgId::eSmartDocIncorrect);
			OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
			return false;
		}
	}

	OmnString newvalue = value.getStr();
	rslt = targetDoc->xpathSetAttr(xpath, newvalue);
	if (!rslt)
	{
		AosSetError(rdata, "failed_set_value") << ": " << xpath << ":" << newvalue;
		OmnAlarm << rdata->getErrmsg() << ": " << targetDoc->toString() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSetAttr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSetAttr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


#endif
