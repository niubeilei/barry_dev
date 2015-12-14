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
// 05/15/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_TesterGetArcdTester_h
#define AosSengTester_TesterGetArcdTester_h

#include "SengTorturer/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosGetArcdTester : virtual public AosSengTester
{
public:
	AosGetArcdTester(const OmnString &weight_tagname, const bool);
	~AosGetArcdTester();

	virtual bool test();
};
#endif

