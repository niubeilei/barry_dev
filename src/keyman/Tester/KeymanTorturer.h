////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: KeymanTorturer.h
//// Description:
////   
////
//// Modification History:
//// 11/21/2006      Created by Harry Long
////
//////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KeymanTorturer_h
#define Omn_KeymanTorturer_h

#include "Tester/TestPkg.h"


class AosKeymanTorturer : public OmnTestPkg
{
	enum
	{
		eMaxKeys = 5000
	};

private:
	OmnString	mNames[eMaxKeys];
	OmnString	mKeys[eMaxKeys];
	int			mNumKeys;

public:
	AosKeymanTorturer();
	~AosKeymanTorturer() {}

	virtual bool            start();

private:
	bool testKeyManagement();
	int  keyExist(const char *name);
	bool addKey();
	bool removeKey();
	bool getKey();
	bool addKey(char *name, char *key);
	bool testDataEncDec();
	bool encodeData();
	bool decodeData();
	bool testAdd_Get();
	bool compareData();
	bool testGenerateMac();
};
#endif

