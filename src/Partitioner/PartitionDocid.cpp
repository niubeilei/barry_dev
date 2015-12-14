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
#include "Partitioner/PartitionDocid.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosPartitionDocid::AosPartitionDocid(const bool regflag)
:
AosPartitioner(AOSPARTITIONER_DOCID, AosPartitionerType::eDocid, regflag)
{
}


AosPartitionDocid::AosPartitionDocid(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosPartitioner(AOSPARTITIONER_DOCID, AosPartitionerType::eDocid, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosPartitionDocid::~AosPartitionDocid()
{
}


bool
AosPartitionDocid::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosPartitionDocid::partition(
		const AosDataTablePtr &input, 
		vector<AosDataTablePtr> &outputs, 
		const AosRundataPtr &rdata)
{
	// This function combines the records in 'input' by combining
	// records with the same key into one.
	OmnNotImplementedYet;
	return false;
}


AosPartitionerObjPtr 
AosPartitionDocid::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosPartitionDocid(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

