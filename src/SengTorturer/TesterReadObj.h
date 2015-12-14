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
#ifndef AosSengTester_ReadObjTester_h
#define AosSengTester_ReadObjTester_h

#include "SengTorUtil/SengTester.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosReadObjTester : virtual public AosSengTester
{
public:
	/*
	AosReadObjTester(const OmnString &weight_tagname);
	~AosReadObjTester();

	virtual bool test();
	bool	ReadObj(
				const AosSengTestThrdPtr &thread,
				const AosSengTesterMgrPtr &mgr,
				const OmnString &ldocid);
				*/
};
#endif

