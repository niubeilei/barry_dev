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
// 2013/08/12  Andy Zhang     
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermCounterOfUWDivOrder_h
#define Aos_Query_TermCounterOfUWDivOrder_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "Util/Opr.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"
#include <vector>

class AosTermCounterOfUWDivOrder : public AosTermIILType
{
public:
	enum Opr
	{
		eInvalid,
		eAdd,
		eMinus,
		eDivide,
		eMultiply,
		eDftPsize = 20,
		eDocsMax = 2000,
		eMapContentMax = 1000 * 1000 * 2
	};

	bool                        mReverseOrder;
	OmnString 					mLName;
	OmnString					mRName;
	Opr							mOpr;
	i64							mDecimals;
	OmnString					mValueName;
	multimap<double, AosXmlTagPtr>   mDocsMap;
	vector<AosXmlTagPtr>		mDocs;

	AosQueryTermObjPtr   		mOrTerm;
	i64                         mPsize;
	i64                         mStartIdx;
	i64                         mCrtIdx;     
	OmnString                   mOrderContainer;
	OmnString                   mOrderFname;


private:

public:
	AosTermCounterOfUWDivOrder(const bool regflag);
	AosTermCounterOfUWDivOrder(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermCounterOfUWDivOrder();

	virtual bool 	nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);

	virtual bool 	getDocidsFromIIL(
					const AosQueryRsltObjPtr &query_rslt,
					const AosBitmapObjPtr &query_bitmap, 
					const AosQueryContextObjPtr &query_context, 
					const AosRundataPtr &rdata);

	virtual i64		getTotal(const AosRundataPtr &rdata);

	virtual void	reset(const AosRundataPtr &rdata);

	virtual bool 	moveTo(
					const i64 &pos, 
					const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);

	virtual AosXmlTagPtr    getDoc(
					const u64 &docid,
					const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	bool loadData(const AosRundataPtr &rdata);

private:
	bool 			parse(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	AosTermCounterOfUWDivOrder::Opr toEnum(
		        	const OmnString &opr,
					const AosRundataPtr &rdata);

	bool			processStatistics(
					const AosQueryTermObjPtr &cond, 
					const AosXmlTagPtr &stat_tag,
	   				const AosRundataPtr &rdata);
	
};
#endif

