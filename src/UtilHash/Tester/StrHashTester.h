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
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef UtilHash_StrHashTester_h
#define UtilHash_StrHashTester_h

#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/String.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/StrObjHash.h"
#include "UtilHash/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <map>

using namespace std;


class AosStrHashTester : public OmnTestPkg
{
private:
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

		eDefaultTries = 10000,
		eArraySize = 3
	};

	OmnMutexPtr 		mLock;
	map<OmnString, u64> mEntries;
	vector<OmnString>	mKeys;
	int					mNumEntries;
	AosStrObjHashPtr	mStrHashObj;
	bool				mStopAdding;
	bool				mIsGood;
	int					mNumAdds;
	int					mNumDeletes;
	int					mNumReads;
	int					mNumModifies;
	
public:
	AosStrHashTester();
	~AosStrHashTester() {}

	virtual bool		start();

private:
	bool	init();
	bool	basicTest();
	bool	strHashTest();
	bool	addData();
	bool	deleteData();
	bool	modifyData();
	bool	getData();
	bool	appOpr();
};

#endif

