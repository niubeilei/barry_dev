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
// 2013/05/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/JimoCallSysFunc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoCallSysFunc(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc) 
{
	rdata->setDLLObj(0);
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoCallSysFunc(rdata, sdoc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosJimoCallSysFunc::AosJimoCallSysFunc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc)
:
AosJimo(rdata, sdoc)
{
}


AosJimoCallSysFunc::~AosJimoCallSysFunc()
{
}


bool
AosJimoCallSysFunc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	if (!def) return true;
	return true;
}


OmnString
AosJimoCallSysFunc::toString() const
{
	OmnString ss = AosJimo::toString();
	return ss;
}


bool 
AosJimoCallSysFunc::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) const
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool 
AosJimoCallSysFunc::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


AosJimoPtr 
AosJimoCallSysFunc::clone(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc) const
{
	try
	{
		AosJimoCallSysFunc *obj = OmnNew AosJimoCallSysFunc(*this);
		bool rslt = obj->config(rdata, sdoc);
		aos_assert_rr(rslt, rdata, 0);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoCallSysFunc::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	int interface = rdata->getInterfaceId();
	switch (interface)
	{
	case eRunWithSmartDoc:
		 return runWithSmartdoc(rdata, sdoc);

	default:
		 AosSetErrorU(rdata, "unrecognized_interface") << interface << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosJimoCallSysFunc::supportInterface(
		const AosRundataPtr &rdata, 
		const OmnString &interface_objid) const
{
	AosXmlTagPtr doc = AosGetDocByObjid(interface_objid, rdata);
	if (!doc)
	{
		return false;
	}

	int id = doc->getAttrInt(AOSTAG_INTERFACE_ID, -1);
	return supportInterface(rdata, id);
}


bool 
AosJimoCallSysFunc::supportInterface(
		const AosRundataPtr &rdata, 
		const int interface_id) const
{
	switch (interface_id)
	{
	case eRunWithSmartDoc:
		 return true;

	default:
		 break;
	}

	return false;
}


bool
AosJimoCallSysFunc::runWithSmartdoc(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	OmnString funcname = sdoc->getAttrStr("func_name");
	if (funcname == "AosCheckIgnoredAlarms")
	{
	//	AosCheckIgnoredAlarms();
		return true;
	}

	AosSetErrorU(rdata, "function_not_recognized") << funcname << enderr;
	return false;
}

