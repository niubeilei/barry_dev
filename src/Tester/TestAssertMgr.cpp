////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestAssertMgr.cpp
// Description:
//	OmnTestAssert maintains an array of Teset Assert results, one 
//  for each Test Assert. A Test Assert is identified by the enum
//  OmnTestAssert.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestAssertMgr.h"

static bool sgTestAsserts[OmnTAId::eLastValidEntry];

void
OmnTestAssertMgr::set(const OmnTAId::E id, const bool rslt)
{
	if (!OmnTAId::isValid(id))
	{
		return;
	}

	sgTestAsserts[id] = rslt;
}


bool
OmnTestAssertMgr::check(const OmnTAId::E id)
{
	if (!OmnTAId::isValid(id))
	{
		return false;
	}

	return sgTestAsserts[id];
}
