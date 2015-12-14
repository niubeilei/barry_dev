////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appoint64_ted officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/03/24 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "MetaData/DocidMetaData.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"


AosDocidMetaData::AosDocidMetaData(
		vector<u64> &docids,
		vector<u64> &schemaids)
:
mDocids(docids),
mSchemaIds(schemaids),
mIdx(-1)
{
	if (docids.size() != schemaids.size())
	{
		OmnScreen << "Warning : mayby error." << endl;
	}
}


AosDocidMetaData::~AosDocidMetaData()
{
}
	

void 
AosDocidMetaData::moveToNext()
{
	mIdx++;
}
	

OmnString 
AosDocidMetaData::getAttribute(const OmnString &key)
{
	OmnString ss = "";
	if (key == "length") 
	{
		ss << mRecordLength;
		return ss;
	}
	else if (key == "docid")
	{
		ss << mDocids[mIdx];
	}
	else if (key == "schemaid")
	{
		ss << mSchemaIds[mIdx];
	}
	else
	{
		OmnAlarm << "error ." << enderr;
	}
	return ss;
}

