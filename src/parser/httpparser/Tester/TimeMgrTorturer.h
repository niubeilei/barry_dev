////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ApplicationProxyTester.h
//// Description:
////   
////
//// Modification History:
//// 11/16/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TimeMgr_Tester_TimeMgrTorturer_h
#define Omn_TimeMgr_Tester_TimeMgrTorturer_h


#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosTimeMgrTorturer : public OmnTestPkg
{
private:

public:
        AosTimeMgrTorturer();
	    virtual ~AosTimeMgrTorturer() {}
	    
	    bool test();
//		bool test_res();

	    virtual bool            start();

private:
	bool specialTest1();

};
#endif
