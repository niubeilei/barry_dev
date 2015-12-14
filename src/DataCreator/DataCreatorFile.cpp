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
#include "Sorter/SorterRecord.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosSorterRecord::AosSorterRecord(const bool regflag)
:
AosSorter(AOSSORTER_RECORD, AosSorterType::eRecord, regflag)
{
}


AosSorterRecord::AosSorterRecord(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSorter(AOSSORTER_RECORD, AosSorterType::eRecord, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosSorterRecord::~AosSorterRecord()
{
}


bool
AosSorterRecord::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosSorterRecord::sort(
		const AosDataTablePtr &input, 
		const AosRundataPtr &rdata)
{
	// This sorter sorts records by some fields in the records.
	OmnNotImplementedYet;
	return false;
}


AosSorterObjPtr 
AosSorterRecord::clone(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosSorterRecord(def, rdata);
	}

	catch (...)
	{
		return 0;
	}
}

