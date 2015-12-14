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
#ifndef Omn_httpparser_Tester_ParserTorturer_h
#define Omn_httpparser_Tester_ParserTorturer_h


#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosParserTorturer : public OmnTestPkg
{
private:

public:
        AosParserTorturer();
	    virtual ~AosParserTorturer() {}
	    
	    bool test_req();
		bool test_res();

	    virtual bool            start();

private:
	bool specialTest1();

};
#endif
