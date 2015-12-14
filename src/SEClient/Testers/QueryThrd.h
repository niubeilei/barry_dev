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
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_QueryThrd_h
#define Aos_SEClient_Testers_QueryThrd_h

#include "Database/Ptrs.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEClient/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "Util/StrHash.h"
#include "UtilComm/Ptrs.h"
#include "WordParser/Ptrs.h"



class AosQueryThrd : public OmnThreadedObj, public OmnTestPkg
{
	OmnDefineRCObject;

private:
	enum Opr
	{
		eEQ,
		eNE,
		eLT,
		eLE,
		eGT,
		eGE,
		eInRange,
		eWordAnd,
		eWordOr,
		eTag
	};

	enum QueryType
	{
		eQueryKeyword,
		eQueryTag,
		eQueryDocid,
		eQueryObjid,
		eQueryAttribute,
		eQueryContainer,
		eQueryAnyType
	};

	enum QueryTypeRate
	{
		eQueryKeywordRate = 100,
		eQueryTagRate = 100,
		eQueryDocidRate = 100,
		eQueryObjidRate = 100,
		eQueryAttributeRate = 100,
		eQueryContainerRate = 100,
	};

	enum
	{
		eMaxConds = 10,
		eMaxFields = 10,
		eDftWeight = 100,
		eMaxDocNum = 90000,
		eMaxAttrNum = 1000,
		eNumQueriesPerDoc = 10,
		eMaxTags = 2000
	};

	enum
	{
		eAttrQueryEqualValueRate = 50,
		eAttrQueryEqualValueEQOprRate = 50,
		eAttrQueryEqualValueGEOprRate = 25,
		eAttrQueryEqualValueLEOprRate = 25,
		eAttrQueryStringTotalChangeRate = 50,
		eAttrQueryStringMaxLen = 100,
		eAttrQueryGTValueGTOprRate = 50,
		eAttrQueryGTValueGEOprRate = 50,
		eAttrQueryLTValueLTOprRate = 50,
		eAttrQueryLTValueLEOprRate = 50,
	};
	struct Cond
	{
		OmnString	aname;
		OmnString	avalue;
		OmnString	value2;
		OmnString	opr;
		OmnString	rightOpr;
		bool		isDesc;
		Opr			type;
	};

	AosSearchEngTesterPtr	mTorturer;
	int					mTotal;
	int					mWeight;
	OmnIpAddr			mRemoteAddr;
	int					mRemotePort;
	AosSEClientPtr		mConn;
	OmnThreadPtr		mThread;
	int					mNumConds;
	Cond				mConds[eMaxConds];
	OmnString			mCondStr;
	OmnString			mLimit;
	OmnString			mOrder;
	bool				mNoMoreDocs;
	OmnDbTablePtr		mControllingTable;
	OmnString			mDir;
	int					mStartIdx;
	int					mPsize;
	bool				mDescending;
	int					mNumQueriesCreated;
	int					mNumFields;
	OmnString			mFields[eMaxFields];
	OmnString			mFnames;
	OmnString			mContainer;
	OmnString			mCnames[eMaxFields];
	OmnString			mOnames[eMaxFields];
	char				mFtypes[eMaxFields];
	OmnString			mSiteid;
	OmnString			mUname;
	OmnString			mAppname;
	OmnString			mCtnrs;

	OmnString			mAttrTname;
	OmnString			mDocwdTname;
	OmnString			mTagTname;
	OmnString			mStmt;
	AosXmlTagPtr		mCrtResp;
	OmnString			mOrigReq;

	int					mForceDocid;
	OmnString			mForceWord;
	QueryType			mForceType;
	OmnString			mForceName;
	OmnString			mForceValue;
	AosOpr			mForceOpr;


	OmnString			*mAttrNames;
	OmnString			*mAttrVals;
	AosWordParserPtr	mWordParser;

public:
	AosQueryThrd(
		AosSearchEngTesterPtr &torturer,
		const int total,
		const int weight, 
		const OmnIpAddr &raddr, 
		const int rport, 
		const OmnString &attrtname, 
		const OmnString &docwdtname, 
		const OmnString &tagtname);
	~AosQueryThrd();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state,
						const OmnThreadPtr &thread);
  	virtual bool	signal(const int threadLogicId);
   	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool 	stop();
	virtual bool	start();

private:
	bool 		verifyResp(const OmnString &resp);
	bool 		queryDocs();
	bool 		verifyQueryResp(const OmnConnBuffPtr &buff);
	bool 		createArith(const OmnString &opr, const OmnString &opr1, const Opr type);
	bool		createInRange();
	bool 		createLimit(OmnString &limit);
	bool 		createConds();
	bool		createWord();
	OmnString 	nextDocid();
	OmnString 	getNextDocid();
	bool 		isDocGood(const int idx, const OmnString &docid);
	bool 		createLimit();
	bool 		createOrder();
	bool 		createFnames();
	char 		getFtype();
	OmnString	getCname(const OmnString &oname);
	bool 		createTag();
	bool 		checkFnames(const OmnString &docid);




	bool			basicQuery();
	AosXmlTagPtr	pickDoc();
	bool 			collectWords(const AosXmlTagPtr &xml,
								 OmnStrHash<int, 0x3ff> &hash);
	AosQueryThrd::QueryType 	getQueryType();


	bool			queryKeyword(const AosXmlTagPtr &doc,
							OmnStrHash<int, 0x3ff> &hash);
	bool			queryTag(const AosXmlTagPtr &doc);
	bool			queryDocid(const AosXmlTagPtr &doc);
	bool			queryObjid(const AosXmlTagPtr &doc);
	bool			queryContainer(const AosXmlTagPtr &doc);
	bool			queryAttribute(const AosXmlTagPtr &doc);
	bool 			decideAttrQueryParam(const OmnString &origValue,
										 AosOpr &opr,
										 OmnString &finalValue);

};
#endif
