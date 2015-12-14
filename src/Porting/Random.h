////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Random_h
#define Omn_Porting_Random_h


//-------------------------------------------------------------
// Linux Portion
//-------------------------------------------------------------
#ifdef	OMN_PLATFORM_UNIX

#include "Porting/TimeOfDay.h"
#include <stdlib.h>

inline void OmnInitRandom()
{
	srandom(OmnGetUsec());	
}

	
inline unsigned int OmnRandom()
{
	//
	// It generates a random number.
	//
	return random();
}


inline unsigned int OmnNewRandom()
{
	srandom(OmnGetUsec());
	return random();
}


//-------------------------------------------------------------
// Microsoft Portion
//-------------------------------------------------------------
#elif OMN_PLATFORM_MICROSOFT

#endif

#endif

