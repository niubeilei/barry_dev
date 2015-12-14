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
// 2013/03/20 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_SID_h
#define Aos_Datalet_SID_h

class AosSID
{
public:
	enum E
	{
		eInvalidEntry,

		eIILName, 

		eLastEntry
	};

	static bool	isValid(const E code) 
	{
		return code > eInvalidEntry && code < eLastEntry;
	}

	static bool addName(const E code, const OmnString &name);
};
#endif
