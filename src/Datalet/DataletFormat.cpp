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
//
// Modification History:
// 2014/12/04 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Datalet/DataletFormat.h"

#include "Util/String.h"


OmnString
AosDataletFormat::toString(const AosDataletFormat::E code)
{
 	// This function adds the pair [code, name] into this class. 
	switch (code)
	{
	case eXML: return "xml";
	case eJSON: return "json";
	case eCSV: return "csv";
	default: return "unrecognized";
	}
}
