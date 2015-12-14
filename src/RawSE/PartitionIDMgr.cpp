////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-11-26 created by levi
////////////////////////////////////////////////////////////////////////////
#include "PartitionIDMgr.h"

#include <cstdlib>

#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "SEUtil/EnumFieldName.h"

AosPartitionIDMgr::AosPartitionIDMgr(AosXmlTagPtr	&conf)
{
    // TODO Auto-generated constructor stub
	mHomeDir= conf->getAttrStr("base_dir", "./RawSE");
}


AosPartitionIDMgr::~AosPartitionIDMgr()
{
    // TODO Auto-generated destructor stub
}


u32
AosPartitionIDMgr::pickPartition(
		AosRundata *rdata,
		AosMediaType::Type media_type)
{
	//this function picks a partition according to media type and some other policies
	//TODO:this is just a place holder for now
	switch (media_type)
	{
	case AosMediaType::eMemory:
		OmnNotImplementedYet;
		return -1;

	case AosMediaType::eNVRAM:

		return 1;

	case AosMediaType::eSSD:

		return 1;

	case AosMediaType::eDisk:

		return 1;

	default:
		break;
	}

	AosLogError(rdata, false, AosErrmsgId::eInternalError)
		<< AosEnumFieldName::eMediaType << media_type << enderr;
	return -1;
}


bool
AosPartitionIDMgr::getPathByPartitionID(
		AosRundata				*rdata,
		const u32				partition_id,
		const AosRawFile::Type	eType,
		OmnString				&path)
{
	//this function returns the physical mount point according to partition id
//	if (0 == partition_id)
//	{
//		path = "";
//		rdata->setErrmsg("0 == partition_id");
//		return false;
//	}

	//TODO:get this from configuration
	path = mHomeDir;
	switch (eType)
	{
	case AosRawFile::eHeader:
		path += "/header/";
		break;
	case AosRawFile::eAppendOnly:
	case AosRawFile::eModifyOnly:
	case AosRawFile::eReadOnly:
	case AosRawFile::eReadWrite:
	case AosRawFile::eReadWriteCreate:
	case AosRawFile::eSlab:
		path += "/test/";
		break;
	default:
		break;
	}
	return true;
}


OmnString AosPartitionIDMgr::getBaseDir()
{
	return mHomeDir;
}
