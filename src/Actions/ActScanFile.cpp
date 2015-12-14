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
// 2012/08/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActScanFile.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/FilterObj.h"

AosActScanFile::AosActScanFile(const bool flag)
:
AosSdocAction(AOSACTTYPE_SCAN_FILE, AosActionType::eScanFile, flag)
{
}


AosActScanFile::~AosActScanFile()
{
}


bool	
AosActScanFile::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Create the scanner
	AosXmlTagPtr tag = sdoc->getFirstChild(AOSTAG_DATA_SCANNER);
	// Chen Ding, 2013/11/23
	// AosDataScannerObjPtr scanner = AosDataScannerObj::createDataScanner(tag, rdata);
	AosDataScannerObjPtr scanner = AosCreateDataScanner(rdata.getPtr(), tag, 0);
	if (!scanner)
	{
		AosSetErrorU(rdata, "failed_creating_scanner") << ": " << sdoc->toString() << enderr;
		return false;
	}

	// 2. Create the filter
	tag = sdoc->getFirstChild(AOSTAG_FILTER);
	AosFilterObjPtr filter = AosFilterObj::createFilter(tag, rdata);
	if (!filter)
	{
		AosSetErrorU(rdata, "failed_creating_filter") << ": " << sdoc->toString() << enderr;
		return false;
	}

	// 3. Create data cacher
	tag = sdoc->getFirstChild(AOSTAG_DATA_CACHER);
	AosDataCacherObjPtr cacher = AosDataCacherObj::createDataCacher(tag, rdata);
	if (!cacher)
	{
		AosSetErrorU(rdata, "failed_creating_cacher") << ": " << sdoc->toString() << enderr;
		return false;
	}

	AosDataRecordObjPtr record;	
	while (1)
	{
		if (!scanner->getNextRecord(record, rdata) || !record) return true;

		if (!filter->filterData(record, rdata))
		{
			// The data are not filtered out. Need to put into the data cacher.
			cacher->appendRecord(record, rdata);
		}
	}

	return true;
}


AosActionObjPtr
AosActScanFile::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActScanFile(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


