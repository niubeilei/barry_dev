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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRunDLL.h"


#include "aosDLL/DllMgr.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
using namespace std;

AosActRunDLL::AosActRunDLL(const bool flag)
:
AosSdocAction(AOSACTTYPE_RUNDLL, AosActionType::eRunDLL, flag)
{
}


AosActRunDLL::~AosActRunDLL()
{
}


bool	
AosActRunDLL::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<dll .../>
	// 		<func .../>
	// 		<inputvalues .../>
	// 		<outvalues .../>
	// 	</action>
	
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the dll name
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, "dll", rdata);
	aos_assert_rr(rslt, rdata, false);

	OmnString dll = value.getStr();
	if (!rslt || dll == "")
	{
		AosSetError(rdata, AosErrmsgId::eValueIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	value.reset();
	rslt = AosValueSel::getValueStatic(value, sdoc, "func", rdata);
	aos_assert_rr(rslt, rdata, false);

	OmnString func = value.getStr();
	if (!rslt || func == "")
	{
		AosSetError(rdata, AosErrmsgId::eValueIncorrect);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << sdoc->toString() << enderr;
		return false;
	}

	map<string, string> parm;
	AosXmlTagPtr input_values = sdoc->getFirstChild("inputvalues");
	AosXmlTagPtr entry = input_values->getFirstChild();
	OmnString key;
	while(entry)
	{
		value.reset();
		AosValueSel::getValueStatic(value, entry, rdata);
		OmnString args = value.getStr();
		key = entry->getAttrStr("namekey", "");
		if (args !="" && key != "")
		{
			parm[key.data()] = args.data();
		}
		entry = input_values->getNextChild();
	}
	
/*
	rslt = AosDllMgr::getSelf()->callFunc(rdata, dll, func);
	aos_assert_r(rslt, false);
*/

	AosXmlTagPtr out_values = sdoc->getFirstChild("outvalues");
	entry = out_values->getFirstChild(); 
	while(entry)
	{
		key = entry->getAttrStr("namekey", "");
		OmnString result(parm[key.data()]);
		if (result != "")rdata->setArg1(key, result);
		entry = out_values->getNextChild();
	}
	
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActRunDLL::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRunDLL(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


