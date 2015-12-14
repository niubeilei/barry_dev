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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILTransMap_Testers_IILTransTester_h
#define Aos_IILTransMap_Testers_IILTransTester_h

#include "Util/Opr.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"


class AosIILTransTester : public OmnTestPkg
{
private:
	enum
	{
		mNumThreads = 2
	};

public:
	AosIILTransTester();
	~AosIILTransTester();

	virtual bool		start();
};


#endif

