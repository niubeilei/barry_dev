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
// 2010/10/23	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_TesterNew_IILStrUniqueTester_h
#define Aos_SearchEngine_TesterNew_IILStrUniqueTester_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"


class AosIILStrUniqueTester : public OmnTestPkg
{
public:
	enum Operation
	{
		eAddEntry,
		eDeleteEntry,
		eModifyEntry,
		eVerifyEntry,

		eOperationMax
	};
	
	AosIILStrUniqueTester();
	~AosIILStrUniqueTester() {}

	virtual bool		start();

private:
	int				mNumDocs;
	u64				mDocid;
	AosIILStrPtr	mIIL;
	int				mTotal;
	int				mAddOpr;
	int				mDelOpr;
	int				mModOpr;
	int 			mVerifyOpr;
	AosRundataPtr	mRunData;
	bool			torture();

	bool			addEntry();
	bool 			deleteEntry();
	bool			modifyEntry();
	bool			verifyEntry();
	
	OmnString 		createNewString();
	
	bool 			addValueToArray(
						const OmnString &value, 
						const u64 &docid);

	bool 			removeValueFromArray(
						const OmnString &value, 
						const u64 &docid);

	bool 			modifyValueFromArray(
						const OmnString &oldvalue, 
						const OmnString &newvalue, 
						const u64 &docid);

	int				findValueDocid(
						const OmnString &value,
						const u64 &docid);
	
	int				findValue(
						const OmnString &value,
						const u64 &docid);

	OmnString 		getOprValue(const AosOpr opr, const OmnString &value);
	
	bool 			checkOprValue(const AosOpr opr, const char * c, const OmnString &value);

	bool 			verifyIIL(const AosOpr opr, const OmnString &value);
	bool 			queryIIL(const AosOpr opr, const OmnString &value);
};


#endif

