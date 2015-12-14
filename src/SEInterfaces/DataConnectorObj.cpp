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
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataConnectorObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataConnectorObj::AosDataConnectorObj()
:
AosJimo(AosJimoType::eDataConnector, 1)
{
}


AosDataConnectorObj::AosDataConnectorObj(const u32 version)
:
AosJimo(AosJimoType::eDataConnector, version)
{
}


AosDataConnectorObj::~AosDataConnectorObj()
{
}


AosDataConnectorObjPtr 
AosDataConnectorObj::createDataConnectorStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_data_cube") << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eDataConnector)
	{
		AosSetErrorUser(rdata, "not_a_data_cube_jimo") << enderr;
		return 0;
	}

	AosDataConnectorObjPtr cube = dynamic_cast<AosDataConnectorObj*>(jimo.getPtr());
	if (!cube)
	{
		AosSetErrorUser(rdata, "invalid_data_cube") << enderr;
		return 0;
	}

	return cube;
}

AosDataConnectorObjPtr 
AosDataConnectorObj::createDataConnectorStatic(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_creating_data_cube") << enderr;
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eDataConnector)
	{
		AosSetErrorUser(rdata, "not_a_data_cube_jimo") << enderr;
		return 0;
	}

	AosDataConnectorObjPtr cube = dynamic_cast<AosDataConnectorObj*>(jimo.getPtr());
	if (!cube)
	{
		AosSetErrorUser(rdata, "invalid_data_cube") << enderr;
		return 0;
	}

	return cube;
}

