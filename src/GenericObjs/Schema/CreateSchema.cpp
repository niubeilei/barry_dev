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
#include "GenericObjs/Schema/CreateSchema.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMethodCreateSchema_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodCreateSchema(version);
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



AosMethodCreateSchema::AosMethodCreateSchema(const int version)
:
AosGenericMethod(version)
{
}


AosMethodCreateSchema::AosMethodCreateSchema(const AosMethodCreateSchema &rhs)
:
AosGenericMethod(rhs.mJimoVersion)
{
}

AosMethodCreateSchema::~AosMethodCreateSchema()
{
}


bool
AosMethodCreateSchema::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodCreateSchema::cloneJimo() const
{
	return OmnNew AosMethodCreateSchema(*this);
}


bool 
AosMethodCreateSchema::proc(
		AosRundata *rdata,
		const OmnString &schema_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// This function creates a generic object. 
	// The JQL format:
	// 	create "schema" "schema-name" 
	// 	(
	// 	    schema_type:"fixed_length"
	// 	    record_length:134,
	// 	    record_terminate_chars:"\r\n",
	// 	    field:(name:"tpe", data_type:"str", max_length:2),
	// 	    field:(name:"value1", data_type:"str", max_length:4),
	// 	    field:(name:"value2", data_type:"str", max_length:9)
	// 	);
	// It creates a schema. If the schema already exists, it is an error.
	//
	// It converts it to an XML:
	// 	<schema ...>
	// 		<...>
	// 	</schema>

	aos_assert_r(false, false);
#if 0
	if (schema_name == "")
	{
		AosSetErrorUser(rdata, "schema_name_empty") << enderr;
		return false;
	}

	for (u32 i=0; i<parms.size(); i++)
	{
		OmnString name = parms[i]->getName();
		if (name == AOSPARM_SCHEMA_TYPE)
		{
		}

		if (name == AOSPARM_RECORD_LENGTH)
		{
		}

		if (name == AOSPARM_FIELD)
		{
		}
	}
#endif

	return true;
}

