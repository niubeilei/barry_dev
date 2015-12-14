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
// This action sets a value to rundata:
//
// Modification History:
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSetRundataValue.h"

#include "Alarm/Alarm.h"
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

AosActSetRundataValue::AosActSetRundataValue(const bool flag)
:
AosSdocAction(AOSACTTYPE_SETRUNDATA_VALUE, AosActionType::eSetRundataValue, flag)
{
}


AosActSetRundataValue::~AosActSetRundataValue()
{
}


bool	
AosActSetRundataValue::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_ZKY_TYPE=AOSACTTYPE_SETRUNDATA_VALUE
	// 		AOSTAG_RUNDATA_VAR_NAME="xxx"	
	// 		<AOSTAG_VALUE_SELECTOR .../>
	// 	</sdoc>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the var name
	OmnString varname = sdoc->getAttrStr(AOSTAG_RUNDATA_VAR_NAME);
	if (varname == "")
	{
		AosSetErrorU(rdata, "missing_varname") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the value
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_VALUE_SELECTOR, rdata);
	if (!rslt || !value.isNull())
	{
		AosSetError(rdata, AosErrmsgId::eValueIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	OmnString newvalue = value.getStr();
	//OmnString newvalue;
	//value.getStr();
	rdata->setArg1(varname, newvalue);
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSetRundataValue::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSetRundataValue(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


