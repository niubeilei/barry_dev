////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CompAlg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_CompAlg_h
#define Omn_UtilComm_CompAlg_h


class OmnCompAlg
{
public:
	enum E
	{
		eFirstValidEntry,

		eUnknown,
		eNoComp,

		eLastValidEntry
	};

	static bool isValid(const E c)
	{
		return (c > eFirstValidEntry && c < eLastValidEntry);
	}
};


#endif

