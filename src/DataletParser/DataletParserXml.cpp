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
// A DataletParser is a Jimo that is responsible for processing one 
// datalet related request such as creating, deleting, modifying, 
// or reading datalets on the cubes. That is, all DataletParsers run
// on a Datalet Store Cluster.
//
// Modification History:
// 2015/06/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataletParser/DataletParserXml.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataletParserXml_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataletParserXml(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosLogError(rdata, false, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}



AosDataletParserXml::AosDataletParserXml(const int version)
{
	mJimoType = AosJimoType::eDataletParser;
	mJimoVersion = version;
}


AosDataletParserXml::~AosDataletParserXml()
{
}


AosXmlTagPtr 
AosDataletParserXml::parse(AosRundata *rdata, const OmnString &doc)
{
	return AosStr2Xml(rdata, doc AosMemoryCheckerArgs);
}


AosJimoPtr
AosDataletParserXml::cloneJimo() const
{
	return OmnNew AosDataletParserXml(mJimoVersion);
}

