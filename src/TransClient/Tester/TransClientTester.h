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
#ifndef Aos_TransClient_Tester_TransClientTester_h
#define Aos_TransClient_Tester_TransClientTester_h 

#include "TransClient/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"

#include <map>
#include <vector>
#include <deque>

using namespace std;


class AosTransClientTester : public OmnTestPkg,
						virtual public OmnThreadedObj
{

public:
	enum 
	{
		eNodeName = 'n',
		eNodeText = 't'
	};

private:
	AosTransClientPtr		mTransClient;
	OmnMutexPtr				mLock;

public:
	AosTransClientTester();
	~AosTransClientTester();

	virtual bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	bool 	basicTest(u32 tid, u32 tries);
};
#endif
