////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmartIndexValListTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_SmartIndexValListTester_h
#define Omn_TestUtil_SmartIndexValListTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/SmtIdxVList.h"


class OmnTestMgr;
class OmnSIVTE
{
public:
	OmnString	mKey;
	int			mValue;

	OmnString	getKey() const {return mKey;}
	bool operator == (const OmnSIVTE &rhs) const
	{
		return mKey == rhs.mKey && mValue == rhs.mValue;
	}

	OmnString toString() const
	{
		OmnString str;
		str << "Key = " << mKey << ", Value = " << mValue;
		return str;
	}
};


typedef OmnSmtIdxVList<OmnString, OmnSIVTE> OmnTestSlEntry;


class OmnSIVTester : public OmnTestPkg
{
private:
	OmnTestSlEntry mList;

public:
	OmnSIVTester() {mName = "OmnSIVTester";}
	~OmnSIVTester() {}

	virtual bool		start();

private:
	void	createList(const int howMany);
};


#endif

