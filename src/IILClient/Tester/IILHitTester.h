////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 2011/07/11	by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClient_Tester_IILHitTester_h
#define AOS_IILClient_Tester_IILHitTester_h

#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


class AosIILHitTester : public OmnTestPkg
{

public:
	AosIILHitTester();
	~AosIILHitTester();

	u64				mDocid;
	int				mTotal;
	int				mAdd;
	int				mDel;
	int				mCheck;
	int 			mNumDocs;

	virtual bool 	start();
	
private:
	bool			basicTest();
		
	OmnString		createIILName();
	
	bool			addDoc();

	bool			removeDoc();

	bool			checkDocid();
};
#endif

 
