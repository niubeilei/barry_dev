////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: RandomStrTorturer.h  
//// Description:
////   
////
//// Modification History:
//// 11/27/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_Tester_RandomStrTorturer_h
#define Omn_Random_Tester_RandomStrTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosRandomStrTorturer : public OmnTestPkg
{
private:

public:
    AosRandomStrTorturer();
    ~AosRandomStrTorturer() {}

    virtual bool            start();

private:
	bool testRandomStr(const u32 tries);
};
#endif
