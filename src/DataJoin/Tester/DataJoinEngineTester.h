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
// 2012/11/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_Testers_DataJoinEngineTester_h
#define Aos_DataJoin_Testers_DataJoinEngineTester_h

#include "Tester/TestPkg.h"
#include "Rundata/Ptrs.h"


class AosDataJoinEngineTester : virtual public OmnTestPkg
{

public:
	AosDataJoinEngineTester();
	~AosDataJoinEngineTester();

	virtual bool	start();

private:	
	bool			createJobDoc(const AosRundataPtr &rdata);
	bool			createIILEntry(const AosRundataPtr &rdata);
	bool			checkRslt(const AosRundataPtr &rdata);
};
#endif
