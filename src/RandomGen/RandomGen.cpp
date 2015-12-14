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
// 2014/08/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RandomGen/RandomGen.h"

#include "Alarm/Alarm.h"
//#include "RandomGen/RandomGenInt.h"
#include "Rundata/Rundata.h"
#include "RandomGen/RandomGenStr.h"
//#include "RandomGen/RandomGenU64.h"

AosRandomGen::AosRandomGen(const int version)
:
AosRandomGenObj(version)
{
}


AosRandomGen::~AosRandomGen()
{
}


AosRandomGenPtr
AosRandomGen::createRandomGen(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)//get the type
{
	// 'conf' is in the following format:
	// 	<conf type="xxx" .../>
	OmnString type = conf->getAttrStr("type");
	if (type == "string") return OmnNew AosRandomGenStr(rdata, conf);
//	if (type == "int") return OmnNew AosRandomGenInt(rdata, conf);
//	if (type == "u64") return OmnNew AosRandomGenU64(rdata, conf);
	//
	AosLogError(rdata, true, "unrecognized_type") << type << enderr;
	return 0;
}

