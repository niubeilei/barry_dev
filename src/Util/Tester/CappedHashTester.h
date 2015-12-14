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
// 09/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_HashedObjTester_h
#define Omn_TestUtil_HashedObjTester_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "Util/CappedHash.h"
#include "XmlUtil/Ptrs.h"



class AosCappedHashTester : public OmnTestPkg
{
private:
	struct Entry
	{
		OmnString 		key;
		AosXmlTagPtr	value;
	};

	enum
	{
		eAddWeight = 55,
		eDeleteWeight = 10,
		eGetWeight = 20,
		eDeleteInvalid = 50,
		eGetValidDataWeight = 50,
		eStopAddingWeight = 10,

		eMinKeyLen = 5,
		eMaxKeyLen = 30,

		eArraySize = 3
	};

	AosCappedHash	mMap;
	Entry *			mArray[eArraySize];
	int				mNumEntries;
	bool			mStopAdding;

public:
	AosCappedHashTester();
	~AosCappedHashTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	addEntry(bool &added);
	bool	deleteEntry();
	bool	modifyEntry();
	bool	getEntry();
};
#endif

