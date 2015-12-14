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
#ifndef Aos_Datalet_DataletFormat_h
#define Aos_Datalet_DataletFormat_h

#include "Util/String.h"



class AosDataletFormat
{
public:
	enum E
	{
		eInvalidEntry,

		eXML, 
		eJSON,
		eBSON,
		eCSV,

		eLastEntry
	};

	static bool	isValid(const E code) 
	{
		return code > eInvalidEntry && code < eLastEntry;
	}

	static OmnString toString(const E code);
};
#endif
