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
#include "Combiner/CombinerSum.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosCombinerSum::AosCombinerSum(const bool regflag)
:
AosCombiner(AOSCOMBINER_SUM, AosCombinerType::eSum, regflag)
{
}


AosCombinerSum::AosCombinerSum(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosCombiner(AOSCOMBINER_SUM, AosCombinerType::eSum, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosCombinerSum::~AosCombinerSum()
{
}


bool
AosCombinerSum::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosCombinerSum::combine(
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
AosCombinerSum::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosCombinerSum(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

