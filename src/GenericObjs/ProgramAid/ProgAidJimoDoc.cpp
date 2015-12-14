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
// 2015/02/02 Created by <put your name>
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/RandDataGen/ProgAidJimoDoc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/Ptrs.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosProgAidJimoDoc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosProgAidJimoDoc(version);
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


AosProgAidJimoDoc::AosProgAidJimoDoc(const int version)
:
AosGenericMethod(version)
{
}


AosProgAidJimoDoc::~AosProgAidJimoDoc()
{
}


bool
AosProgAidJimoDoc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosProgAidJimoDoc::cloneJimo() const
{
	return OmnNew AosProgAidJimoDoc(*this);
}


bool
AosProgAidJimoDoc::create(...)
{
	// 	Create JimoDoc <name>
	// 	{
	// 	    jimo_type:"xxx",
	// 		classname:"xxx",
	// 		libname:"xxx",
	// 		version:ddd,
	// 		description:"xxx",
	// 		tags:"xxx,...",
	// 		dirname:"xxx"
	// 	};
	//
	// This function creates the source code for the jimo 
	// "jimo_type". If the type is defined, there shall be
	// a template. It fills in the blanks. 
	OmnNotImplementedYet;
}

