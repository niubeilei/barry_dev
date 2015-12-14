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
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticRules_Tester_SR00028Tester_h
#define Aos_SemanticRules_Tester_SR00028Tester_h

#include "Tester/TestPkg.h"


class AosSR00028Tester : public OmnTestPkg
{
private:

public:
	AosSR00028Tester() {mName = "SR00028Tester";}
	~AosSR00028Tester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

