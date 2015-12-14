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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoExample/JimoExample.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoExample(
 		const AosRundataPtr &rdata, 
		const OmnString &objid,
 		const AosXmlTagPtr &sdoc) 
{
	rdata->setDLLObj(0);
	try
	{
		OmnScreen << "To create Jimo: " << objid << endl;
		AosJimoPtr jimo = OmnNew AosJimoExample(rdata, objid, sdoc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << objid << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosJimoExample::AosJimoExample(
		const AosRundataPtr &rdata,
		const OmnString &objid,
		const AosXmlTagPtr &sdoc)
:
AosJimo(rdata, objid, sdoc),
mData1(-1),
mData2("no contents")
{
}


AosJimoExample::~AosJimoExample()
{
}


bool
AosJimoExample::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	if (!def) return true;
	mData1 = def->getAttrInt("data1", -1);
	mData2 = def->getAttrStr("data2", "");
	return true;
}


OmnString
AosJimoExample::toString() const
{
	OmnString ss = AosJimo::toString();
	ss << ". Data1=" << mData1 
		<< ". Data2=" << mData2;
	return ss;
}


bool 
AosJimoExample::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) const
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	buff->setU64(mData1);
	buff->setOmnStr(mData2);
	return true;
}


bool 
AosJimoExample::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	mData1 = buff->getU64(0);
	mData2 = buff->getOmnStr("");
	return true;
}


AosJimoPtr 
AosJimoExample::clone(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc) const
{
	try
	{
		AosJimoExample *obj = OmnNew AosJimoExample(*this);
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
AosJimoExample::run(		
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
AosJimoExample::supportInterface(
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
AosJimoExample::supportInterface(
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
AosJimoExample::runWithSmartdoc(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	OmnScreen << "Running jimo example: " << toString() << endl;
	return true;
}


