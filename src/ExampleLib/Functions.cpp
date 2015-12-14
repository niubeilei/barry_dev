////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 10/13/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "ExampleLib/Functions.h"

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Util/IpAddr.h"


int aos_test_func1(const OmnString &start_addr, 
				   const OmnString &end_addr, 
				   const OmnString &mask)
{
 	OmnTrace << "In test_func1: " << start_addr << ":" << end_addr << endl;
	if (!OmnIpAddr(start_addr).isValid()) 
	{
		OmnTrace << "start_addr is invalid: " << start_addr << endl;
		return 1;
	}

	if (!OmnIpAddr(end_addr).isValid())
	{
		OmnTrace << "end_addr is invalid: " << end_addr << endl;
		return 1;
	}

	if (!OmnIpAddr::isValidMask(mask))
	{
		OmnTrace << "Mask is invalid: " << mask << endl;
		return 1;
	}

 	return 0;
}


