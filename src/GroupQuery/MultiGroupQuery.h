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
// 2013/03/28 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GroupQuery_MultiGroupQuery_h
#define Aos_GroupQuery_MultiGroupQuery_h

#include "GroupQuery/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/GroupQueryObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"

#include <vector>
using namespace std;

class AosMultiGroupQueryPhase1Thrd: virtual public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosRundataPtr 			mRundata;
	AosMultiGroupQueryPtr	mQuery;
	OmnString				mIILName;
	AosQueryContextObjPtr	mQueryContext;
	int						mKeyOrder;
	int						mCondOrder;
public:	
	AosMultiGroupQueryPhase1Thrd(
			const AosRundataPtr &rdata,
			const AosMultiGroupQueryPtr	&query,
			const OmnString &iilname,
			const AosQueryContextObjPtr	&query_context,
			const int &key_order,
			const int &cond_order)
	:
	OmnThrdShellProc("AosMultiGroupQueryPhase1Thrd"),
	mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
	mQuery(query),
	mIILName(iilname),
	mQueryContext(query_context),
	mKeyOrder(key_order),
	mCondOrder(cond_order)
	{
	}
	
	~AosMultiGroupQueryPhase1Thrd(){}
	
	virtual bool			run();
	virtual bool			procFinished();
	
};

class AosMultiGroupQueryPhase2Thrd: virtual public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosRundataPtr 				mRundata;
	AosMultiGroupQueryPtr		mQuery;
	u64							mCubeID; 
	vector<OmnString>			mIILNames;
	vector<AosQueryRsltObjPtr>	mBitmapIDLists;
	vector<AosBitmapObjPtr>		mPartialBitmaps;

public:	
	AosMultiGroupQueryPhase2Thrd(
			const AosRundataPtr &rdata,
			const AosMultiGroupQueryPtr	&query,
			const int &cube_id,
			const vector<OmnString> &iilnames,
			const vector<AosQueryRsltObjPtr> idlists,
			const vector<AosBitmapObjPtr> bitmaps)
	:
	OmnThrdShellProc("AosMultiGroupQueryPhase2Thrd"),
	mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
	mQuery(query),
	mCubeID(cube_id),
	mIILNames(iilnames),
	mBitmapIDLists(idlists),
	mPartialBitmaps(bitmaps)
	{
	}
	
	~AosMultiGroupQueryPhase2Thrd(){}

	virtual bool			run();
	virtual bool			procFinished();
};

class AosMultiGroupQuery : virtual public AosGroupQueryObj
{
	OmnDefineRCObject;

	enum Type
	{
		eNoGroup,
		eSingleGroup,
		eMultiGroup
	};
	
	vector<OmnString>				mIILNames;
	vector<AosQueryContextObjPtr>	mQueryContexts;

	OmnString						mF1IILName;
	AosQueryContextObjPtr			mF1QueryContext;	
	bool							mF1Reverse;
	OmnString						mF2IILName;
	AosQueryContextObjPtr			mF2QueryContext;	
	bool							mF2Reverse;

	AosRundataPtr					mRundata;
	u64								mPageSize;

	OmnMutexPtr						mLock;
	
	AosBitmapObjPtr					mRsltBitmap;
	
	vector<AosQueryRsltObjPtr>		mQueryRslts;
	vector<AosBitmapObjPtr>			mBitmaps;
	
	AosQueryRsltObjPtr				mF1QueryRslt;
	AosBitmapObjPtr					mF1Bitmap;

	AosQueryRsltObjPtr				mF2QueryRslt;
	AosBitmapObjPtr					mF2Bitmap;

	Type							mQueryType;
	bool							mNoGroupQueried;
	AosMultiGroupQuery(const AosRundataPtr &rdata);
	~AosMultiGroupQuery();


public:
	virtual bool addCond(
				const OmnString &iilname,
				const AosOpr	&opr,
				const OmnString	&value1,
				const OmnString	&value2);
	virtual bool addCond(
				const OmnString &iilname,
				const AosQueryContextObjPtr &context);
	
	virtual bool setGroupFields(
				const OmnString &iilname_f1,
				const bool &f1_reverse,
				const OmnString &iilname_f2,
				const bool &f2_reverse);

	virtual bool setGroupFields(
				const OmnString &iilname_f2,
				const bool &f2_reverse);

	virtual void setPageSize(const u64 &page_size){mPageSize = page_size;}

	virtual bool nextBlock(AosBitmapObjPtr &bitmap);


	bool setPhase1Rslt(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const int key_order,
		const int cond_order);

	bool setPhase2Rslt(const AosBitmapObjPtr &query_bitmap);

	virtual AosGroupQueryObjPtr createQuery(const AosRundataPtr &rdata) const;

private:

	bool nextBlock1(AosBitmapObjPtr &bitmap, bool &finished);


	
};
#endif

