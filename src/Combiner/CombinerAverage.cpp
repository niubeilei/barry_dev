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
#include "Combiner/CombinerAverage.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosCombinerAverage::AosCombinerAverage(const bool regflag)
:
AosCombiner(AOSCOMBINER_SUM, AosCombinerType::eAverage, regflag)
{
}


AosCombinerAverage::AosCombinerAverage(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosCombiner(AOSCOMBINER_SUM, AosCombinerType::eAverage, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosCombinerAverage::~AosCombinerAverage()
{
}


bool
AosCombinerAverage::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCombinerAverage::combine(
		const AosDataTablePtr &input, 
		AosDataTablePtr &output, 
		const AosRundataPtr &rdata)
{
	// This function combines the records in 'input' by combining
	// records with the same key into one.
	OmnNotImplementedYet;
	return false;
}


AosCombinerObjPtr 
AosCombinerAverage::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosCombinerAverage(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

