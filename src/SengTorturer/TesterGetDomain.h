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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_GetDomainTester_h
#define AosSengTester_GetDomainTester_h

#include "SengTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/OmnNew.h"


class AosGetDomainTester : virtual public AosSengTester
{
private:
	enum
	{
		eGetDomainTryWeight = 5
	};


public:
	AosGetDomainTester(const bool);
	AosGetDomainTester();
	~AosGetDomainTester();

	virtual bool test();
	virtual AosSengTesterPtr clone()
			{
				return OmnNew AosGetDomainTester();
			}
private:
	bool checkDomain(const OmnString &domain);
};
#endif

