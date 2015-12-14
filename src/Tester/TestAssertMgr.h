////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestAssertMgr.h
// Description:
//	OmnTestAssert maintains an array of Teset Assert results, one 
//  for each Test Assert. A Test Assert is identified by the enum
//  OmnTestAssert.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestAssertMgr_h
#define OMN_Tester_TestAssertMgr_h


class OmnTAId
{
public:
	enum E
	{
		eFirstValidEntry,

		eString_DefaultConstructor,
		eLastValidEntry
	};

	static bool isValid(const E e)
	{
		return (e > eFirstValidEntry && e < eLastValidEntry);
	}
};


class OmnTestAssertMgr
{
public:
	static void		set(const OmnTAId::E id, const bool rslt);
	static bool		check(const OmnTAId::E id);
};

#endif
