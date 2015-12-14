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
// 08/06/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_StrHashFixedTester_h
#define Omn_TestUtil_StrHashFixedTester_h

#include "Tester/TestPkg.h"
#include "Util/RCObjImp.h"
#include "Util/StrHashFixed.h"


class OmnTestMgr;


class AosStrHashFixedTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxEntries = 10000,
		eBucketLen = 30
	};

	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnStrHashFixed<u64, 260, eBucketLen> mHash;
	map_t	mMap;
	itr_t   mMapItr;

public:
	AosStrHashFixedTester() {mName = "AosStrHashFixedTester";}
	~AosStrHashFixedTester() {}

	virtual bool		start();

private:
	bool	torturer();
	bool	readEntry(OmnStrHashFixed<u64, 260, eBucketLen> &hash);
	bool	addEntries(OmnStrHashFixed<u64, 260, eBucketLen> &hash);
	bool	nextEntries(OmnStrHashFixed<u64, 260, eBucketLen> &hash);
};


#endif

