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
#ifndef Omn_QoS_Tester_CliTorturer_h
#define Omn_QoS_Tester_CliTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosQoSCliTorturer : public OmnTestPkg
{
private:

public:
    AosQoSCliTorturer();
    ~AosQoSCliTorturer() {}

    virtual bool            start();
};
#endif
