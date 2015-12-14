////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: RandomIntegerTorturer.h  
//// Description:
////   
////
//// Modification History:
//// 11/27/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_Tester_RandomIntegerTorturer_h
#define Omn_Random_Tester_RandomIntegerTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosRandomIntegerTorturer : public OmnTestPkg
{
private:

public:
    AosRandomIntegerTorturer();
    ~AosRandomIntegerTorturer() {}

    virtual bool            start();

private:
	bool test1();
	bool test2();
	bool test(const OmnString &example, int tries,
		   int *low, int *high, u16 *weight, int numElems);	
};
#endif
