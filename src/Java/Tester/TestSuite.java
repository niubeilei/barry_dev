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
// Modification History:
// 2014/10/29 Created by Bryant Zhou
////////////////////////////////////////////////////////////////////////////



bool	
AosTestSuite::addTestSuite()
{
	return true;
}





AosTestSuite::toString() const 
{
	OmnString str = "OmnTestSuite: ";
	str << "\n    SuiteId:    " << mSuiteName
		<< "\n    Desc:       " << mDesc
		<< "\n    NumEntries: " << mPackages.entries();
	return str;
}


int
AosTestSuite::start(OmnTestMgr testMgr)
{
	for (...)
	{
		mTestSuite[i]->start(testMgr);
	}
}

