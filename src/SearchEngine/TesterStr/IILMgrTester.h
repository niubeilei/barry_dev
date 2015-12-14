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
#ifndef Aos_SearchEngine_TesterStr_IILMgrTester_h
#define Aos_SearchEngine_TesterStr_IILMgrTester_h

#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"


class AosIILMgrTester : public OmnTestPkg
{
public:
	enum
	{
		eMaxDocs = 1000000
	};

	enum Operation
	{
		eVerifyGetObjid,
		eAddEntry,
		eDeleteEntry,
		eModifyEntry,

		eOperationMax
	};

private:
	int				mNumDocs;
	int				mVerifyGetObjidWeight;
	AosIILStrPtr	mStriil;
	OmnString		mIILName;

public:
	AosIILMgrTester();
	~AosIILMgrTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool 	verifyObjidIIL();
	bool 	verifyObjidDuplicated();
	bool 	verifyObjidDuplicatedEntries();
	bool 	verifyGetObjid();
	bool 	retrieveAllObjids();
	bool 	copyData(const OmnString &iilname);
	bool 	verifyNextDocidSafe(
				const bool reverse, 
				const bool value_unique); 
	bool 	verifyValues(
				const OmnString &iilname, 
				const bool value_unique,
				const bool docid_unique);
	AosOpr pickOperator();
	bool 	tortureNextDocidSafe(
				const int tries,
				const bool value_unique, 
				const bool docid_unique);
	bool 	searchGE(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchEQ(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchGT(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchLT(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchLE(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchNE(
				const OmnString &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchReverseGE(
				const OmnString &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchNormalGE(
				const OmnString &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchReverseGT(
				const OmnString &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchNormalGT(
				const OmnString &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	verifyStrIIL(const AosIILStrPtr &iil);
	bool 	verifyAllIILS(const u64 &start_iilidx, const u64 &end_iilidx);
	bool 	loadStrIIL(const OmnString &iilname);
	bool	tortureAddEntry();
	bool	tortureDeleteEntry();
	bool    tortureModifyEntry();
};


#endif

