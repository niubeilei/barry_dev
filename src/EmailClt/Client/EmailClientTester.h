////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransClient_Tester_LogClientTester_h
#define Aos_TransClient_Tester_LogClientTester_h 

#include "EmailClt/EmailClient.h"
#include "EmailClt/Ptrs.h"
#include "SEUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include <map>
#include <vector>
#include <deque>

using namespace std;


class AosEmailClientTester : public OmnTestPkg
{
private:
	AosEmailClientPtr           mEmailClient;
	OmnString 					mIndex;

public:
	
	AosEmailClientTester(const OmnString &idx);
	~AosEmailClientTester() {};

	virtual bool		start();

private:
	bool basicTest();
};
#endif
