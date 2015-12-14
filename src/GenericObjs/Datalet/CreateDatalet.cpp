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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/Datalet/CreateDatalet.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGenMdCreateDatalet_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGenMdCreateDatalet(version);
		aos_assert_r(jimo, 0);
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



AosGenMdCreateDatalet::AosGenMdCreateDatalet(const int version)
:
AosGenericMethod(version)
{
}


AosGenMdCreateDatalet::~AosGenMdCreateDatalet()
{
}


bool
AosGenMdCreateDatalet::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosGenMdCreateDatalet::cloneJimo() const
{
	return OmnNew AosGenMdCreateDatalet(*this);
}


bool 
AosGenMdCreateDatalet::proc(
		AosRundata *rdata,
		const OmnString &obj_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	OmnNotImplementedYet;
	return true;
}


