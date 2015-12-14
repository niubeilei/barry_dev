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
#ifndef Aos_Query_TermGroup_h
#define Aos_Query_TermGroup_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "CounterUtil/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"

class AosTermGroup : public AosTermIILType
{
public:
	enum Opr
	{
		eInvalid,
		eFirst,
		eLast,
		eSum,
		eGroupAll,
		eMinus,
		eDftPsize = 20,
		eMapContentMax = 1000 * 1000 * 100
	};

private:
	Opr											mOpr;
	i64               							mPsize;
	i64                 						mStartIdx;
	i64											mCrtIdx; 
	i64											mDocsNum;
	i64											mGroupKeyBegin;
	i64											mGroupKeyEnd;
	i64											mStep;
	bool                                        mDataLoaded;
	bool										mIsShortfall;
	bool										mIsSort;
	AosQueryTermObjPtr							mOrTerm;
	OmnString									mGroupValue;
	OmnString									mGroupKeyType;
	OmnString									mSep;
	OmnString									mDefValue;
	OmnString		  	                        mGroupCond;
	OmnString		  	                        mGroupAllType;
	vector<OmnString>							mStrValues;
	vector<OmnString> 							mKeyGroup;
	vector<OmnString> 							mKeyPathFrom;
	vector<OmnString> 							mKeyPathTo;
	map<OmnString, vector<AosXmlTagPtr> >       mDocMap; 
	
	bool										mReverseOrder;
	OmnString									mOrderContainer;
	OmnString									mOrderFname;

public:
	AosTermGroup(const bool regflag);
	AosTermGroup(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermGroup();

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

	bool			processStatistics(
					const AosQueryTermObjPtr &cond, 
					const AosXmlTagPtr &stat_tag,
	   				const AosRundataPtr &rdata);
	
	bool shortfallRecord(
					const AosRundataPtr &rdata,
					const OmnString &groupCond,
					map<OmnString, vector<AosXmlTagPtr> > &docsmap);

	bool shortfallRecord2(const AosRundataPtr &rdata);

	bool doGroup(	const AosRundataPtr &rdata,
					const i64 &tmpsize);

	bool getFirst(	const AosRundataPtr & rdata);
	bool getLast(	const AosRundataPtr & rdata);
	bool getSum(	const AosRundataPtr & rdata);
	bool getGroupAll(const AosRundataPtr & rdata);
	bool getMinus(	const AosRundataPtr & rdata);

	AosTermGroup::Opr toEnum(const OmnString &opr);
	
};
#endif

