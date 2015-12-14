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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSender/DataSenderNorm.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosDataSenderNorm::AosDataSenderNorm(const bool regflag)
:
AosDataSender(AOSDATASENDER_NORM, AosDataSenderType::eNorm, regflag)
{
}


AosDataSenderNorm::AosDataSenderNorm(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosDataSender(AOSDATASENDER_NORM, AosDataSenderType::eNorm, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDataSenderNorm::~AosDataSenderNorm()
{
}


bool
AosDataSenderNorm::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosDataSenderNorm::send(
		const AosBuffPtr &input, 
		const AosRundataPtr &rdata)
{
	// This sorter sorts records by some fields in the records.
	OmnNotImplementedYet;
	return false;
}


AosDataSenderObjPtr 
AosDataSenderNorm::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataSenderNorm(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

