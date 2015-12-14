////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestVariData.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TestVariData_h
#define OMN_Tester_TestVariData_h

#include "Tester/TestVariDataType.h"
#include "Util/String.h"

class OmnTestVariData 
{
public:
	eTester_DataType		mType;
	int						mIntValue;
	bool					mBoolValue;
	OmnString				mStrValue;
	bool					mChanged;
	
public:
	OmnTestVariData();
	OmnTestVariData(const int 		&data);
	OmnTestVariData(const bool		&data);
	OmnTestVariData(const OmnString	&data);

	~OmnTestVariData();

};

#endif

