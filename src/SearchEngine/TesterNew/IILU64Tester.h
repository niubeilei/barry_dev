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
#ifndef Aos_SearchEngine_TesterNew_IILU64Tester_h
#define Aos_SearchEngine_TesterNew_IILU64Tester_h

#include "IILMgr/IILU64.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"


class AosIILU64Tester : public OmnTestPkg
{
public:
	enum 
	{
		eMaxCheckedIILs = 1000,
		eMaxIILLen = 5000
	};
//eMaxCheckedIILs = 10000
//eMaxIILs = 70000,
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
	AosIILU64Ptr	mStriil;
	OmnString		mIILName;
	int				mTotal;
	int				mAddOpr;
	int				mDelOpr;
	int				mModOpr;
	int				mVerifyOpr;
	AosRundataPtr	mRunData;

public:
	AosIILU64Tester();
	~AosIILU64Tester() {}

	virtual bool		start();

private:
	void	toString();
	bool	basicTest();
	bool	torture();
	bool 	verifyObjidIIL();
	bool 	verifyObjidDuplicated();
	bool 	verifyObjidDuplicatedEntries();
	bool 	verifyGetObjid();
	bool 	retrieveAllObjids();
	bool 	copyData(const OmnString &iilname);
	int  	u64cmp(u64 u1, u64 u2);
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
				const bool docid_unique, 
				u64*values, 
				const u64 *docids, 
				const int num_docs);
	bool 	searchGE(
				const u64 &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool 	searchEQ(
				const u64 &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchLT(
				const u64 &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool	searchLE(
				const u64 &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool	searchNormalLE(
				const u64 &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool	searchReverseLE(
				const u64 &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool 	searchNE(
				const u64 &value, 
				const bool reverse,
				const bool value_unique, 
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	searchReverseGE(
				const u64 &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool 	searchNormalGE(
				const u64 &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid, 
				const AosOpr opr,
				const bool value_valid);
	bool 	searchNormalGT(
				const u64 &value, 
				const bool value_unique,
				const bool docid_unique,
				const u64 &expected_docid);
	bool 	verifyStrIIL(const AosIILU64Ptr &iil);
	bool 	verifyAllIILS(const u64 &start_iilidx, const u64 &end_iilidx);
	bool 	loadStrIIL(const OmnString &iilname);

	//tank
	bool	addEntry();
	bool 	deleteEntry();
	bool	modifyEntry();
private:
	AosIILU64Ptr pickIIL(int &idx,const bool create_new);
	bool
	removeEntry(
		char **values,
		u64 *docids,
		const int idx,
		const int num_docs,
		const u64 &iilid);

public:
	bool	doesDocidExist(
				const AosIILU64Ptr &iil,
				u64 &value,
				const u64 &docid);
	bool	getUnique(
			u64 iilid, 
			bool &value_unique, 
			bool &docid_unique);

	int	findValueDocid(
			const u64 &value,
			const u64 &docid, 
			u64* values, 
			const u64 *docids,
			const int num_values, 
			bool &found);

	int findValue(
				const u64 &value, 
				u64* values, 
//				const u64 *docids,
				const int num_values, 
				bool &found);

	bool addValueToArray(
		const u64 &value, 
		const u64 &docid,
		u64* values,
		u64* docids,
		const int num_docs);

	bool removeValueFromArray(
		const u64 &value, 
		const u64 &docid,
		u64*	values,
		u64* docids,
		const int num_docs);

	bool modifyValueFromArray(
		const u64 &oldvalue, 
		const u64 &olddocid,
		const u64 &newvalue, 
		const u64 &newdocid,
		u64*	values,
		u64* docids,
		const int num_docs);
	bool compareValueDocid2(const int iilid, const int num_docs, const int iilpos);

//	bool addValueToArray(
//				const OmnString &value, 
//				const u64 &docid,
//				const u64 &iilid,
//				const bool value_unique, 
//				const bool docid_unique);
private:
	bool
	insertDoc(
			const u64 &iilid, 
			const OmnString &value, 
			const u64 &docid, 
			const u32 idx);

	bool
	verifyIIL(const AosIILU64Ptr &iil, const int iilpos);

	bool 
	verifyIIL(
		const u32 num_docs,
		const AosIILU64Ptr &iil, 
		u64 *values, 
		const u64 *docids);
	bool 
	verifyEntries(
		const u32 num_docs,
		const AosIILU64Ptr &iil, 
		u64 *values, 
		const u64 *docids);
	bool	compareValueDocid(const int iilid, const int num_docs, const int iilpos);
	int 
	compareValueDocid(
			const u64 str1,
			const u64 docid1,
			const u64 str2,
			const u64 docid2);
	void	Clean();

};


#endif

