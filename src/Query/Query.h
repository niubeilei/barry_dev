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
// Modification History:
// 10/24/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_Query_h
#define Aos_Query_Query_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrSplit.h"
#include "XmlUtil/Ptrs.h"


struct AosQuery
{
	enum
	{
		eMaxTerms = 10,
		eMaxWords = 20,
		eMaxPageSize = 200,
		eMaxFields = 100,
		eNumDigitsPsize = 3,
		eNumDigitsIdx = 8,
		eAttr = '1',
		eSubtag = '2'
	};

	AosWebRequestPtr	mReq;
	AosXmlTagPtr		mRoot;
	AosXmlTagPtr		mQuery;
	AosXmlTagPtr		mTerms[eMaxTerms];
	OmnString			mWords[eMaxWords];
	i64					mNumWords;
	AosIILPtr			mIIL;
	i64					mPagesize;
	i64					mTotalDocs;

	i64					mNumMatchedWords;
	AosIILPtr			mIILs[eMaxWords];
	AosIILPtr			mSortedIILs[eMaxWords];
	i64					mNumIILs;
	i64					mIILIdx[eMaxWords];
	OmnString			mDocsFound[eMaxPageSize];
	i64					mNumDocsFound;
	i64					mCrtIIL;
	timeval				mStartTime;
	bool				mIsGood;

	static i64			mDftPageSize;
	static i64			mMaxPageSize;

	AosQuery(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &query, 
		OmnString &errmsg);
	~AosQuery();

	bool	isGood() const {return mIsGood;}
	bool 	singleWordQuery (
				OmnString &contents, 
				OmnString &errmsg);

	bool 	multiWordQuery (
				OmnString &contents, 
				OmnString &errmsg);
	bool	form1Body(
				OmnString &contents, 
				OmnString &errmsg); 
	bool	form2Body(
				OmnString &contents, 
				OmnString &errmsg); 
	bool	form3Body(
				OmnString &contents, 
				OmnString &errmsg); 
	bool 	setTime(OmnString &contents, const i64 &timeIdx);
	bool 	constructBody(
				OmnString &contents, 
				OmnString &errmsg);
	void	setWords(AosStrSplit &);
	bool 	retrieveIILIdx();
	bool 	retrieveIILs();
	u64 	getNextDoc();
	bool 	process(
				OmnString &contents, 
				OmnString &errmsg);
};
#endif

