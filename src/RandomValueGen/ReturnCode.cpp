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
// 05/06/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Parms/ReturnCode.h"


static OmnString sgNames[] = 
{
	"Success", 
	"ProgramError", 
	"ExistInTable", 
	"NotFoundInTable", 
	"NotInRange", 
	"Incorrect", 
	"TooShort", 
	"TooLong", 
	"NullValue", 
	"FromInvalidSet", 
	"SelectFromRecord", 
	"SelectFromTable", 
	"UsedByOtherTables", 
	"NotInList",
	"LastEntry"
};

OmnString AosRVGRc_toStr(const AosRVGReturnCode code)
{
	if (code < 0 || code >= eAosRVGRc_LastEntry)
	{
		return "Invalid";
	}

	return sgNames[code];
}


