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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Interfaces/IfcExample/IfcExample.h"

#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosInterfacePtr AosCreateJimoFunc_AosIfcExample(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &worker_doc, 
 		const AosXmlTagPtr &jimo_doc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosIfcExample *ifc = OmnNew AosIfcExample();
		aos_assert_rr(ifc, rdata, 0);
		return ifc;
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


bool
AosIfcExample_method1(		
		const AosRundataPtr &rdata, 
		AosInterface *abs_ifc,
		AosJimoPtr &jimo,
		const AosXmlTagPtr &worker_doc)
{
	AosIfcExample *ifc = (AosIfcExample*)abs_ifc;

	if (!ifc) return true;

	AosJimoBasicFunc001 method = (AosJimoBasicFunc001)ifc;
	return (*method)(rdata, jimo.getPtr());
}


