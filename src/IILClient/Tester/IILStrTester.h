////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 2011/09/05	by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClient_Tester_IILStrTester_h
#define AOS_IILClient_Tester_IILStrTester_h

#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"


class AosIILStrTester : public OmnTestPkg
{

public:
	AosIILStrTester();
	~AosIILStrTester();

	virtual bool 	start();
	
private:
	bool			basicTest();
		
	bool			getUnique(
						const u64 &iilid,
						bool &value_unique, 
						bool &docid_unique);

	OmnString		getValue(
						const int &idx,
						const bool value_unique);
	
	u64 			getDocid(
						const int &idx,
						const bool docid_unique);

	int				compareValueDocid(
						const char * v1,
						const OmnString &v2,
						const u64 &d1,
						const u64 &d2);

	int				findValueDocid(
						const int idx,
						const OmnString &value,
						const u64 &docid);

	bool			addValueToArray(
						const int idx,
						const OmnString &value,
						const u64 &docid);
	
	bool			removeValueFromArray(
						const int idx,
						const int vidx);

	bool			needCreateNew(const bool unique);

	bool			addEntry();

	bool			removeEntry();

	bool			checkEntry();

	bool			getOprAndStr(
						const int idx,
						AosOpr &opr,
						OmnString &str);

	bool			checkOprValue(
						const AosOpr opr,
						const char * c,
						const OmnString &value);

};
#endif

 
