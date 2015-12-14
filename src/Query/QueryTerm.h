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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_QueryTerm_h
#define Aos_Query_QueryTerm_h

#include "Alarm/Alarm.h"
#include "Query/Ptrs.h"
#include "Query/TermTypes.h"
#include "QueryCond/Ptrs.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryTermObj.h"
#include "XmlInterface/XmlRc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"


struct AosHtmlCode;

class AosQueryTerm : public AosQueryTermObj 
{
	OmnDefineRCObject;
public:
	enum 
	{
		eMaxConds = 30
	};

protected:
	AosQueryType::E	mTermType;
	bool			mIsGood;
	bool			mReverse;
	bool			mOrder;		
	bool			mNoMoreDocs;
	bool			mWithValues;
	u64				mTotal;
	u64				mTotalInRslt;
	u64				mIILId;
	OmnString		mIILName;
	AosQueryCondPtr	mCond;
	AosCondInfo		mCondInfo;

private:

	i64					mCrtIdx;

public:
	AosQueryTerm(
		const OmnString &name,
		const AosQueryType::E type,
		const bool flag);
	~AosQueryTerm();
	
	virtual AosQueryType::E	getType() const {return mTermType;}
	virtual bool	isGood() const {return mIsGood;}
	virtual OmnString getTermName() const;     // Chen Ding, 2013/12/06
	virtual bool	isReverse() const {return mReverse;}
	virtual bool	isOrdered() const {return mOrder;}
	virtual bool	withValue() const {return mWithValues;}
	virtual void	setOrdered(const bool order) {mOrder = order;}
	virtual void	setWithValues(bool withvalue){mWithValues = withvalue;}
	virtual u64		getTotalInRslt()const{return mTotalInRslt;}
	virtual void	setTotalInRslt(const u64 &total){mTotalInRslt = total;}
	virtual OmnString	getIILName() const {return mIILName;}
	virtual AosCondInfo getCondInfo() const {return mCondInfo;}

	virtual bool	canUseBitmapQuery() const {return false;}
	virtual bool	hasDoc() const {return !mNoMoreDocs;}
	virtual bool	needHandleNextDocid() const {return false;}
	virtual AosQueryRsltObjPtr getQueryData() const {return 0;}
	virtual AosQueryContextObjPtr getQueryContext() const {return 0;}
	virtual void	setQueryData(const AosQueryRsltObjPtr &query_rslt){}
	virtual void	setQueryContext(const AosQueryContextObjPtr &query_context){}

	// AosQueryTerm Interface
	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &and_term,
						u64 &docid, 
						bool &finished, 
						const AosRundataPtr &rdata) = 0;
	virtual bool 	nextDocid(
						const AosQueryTermObjPtr &and_term,
						u64 &docid, 
						OmnString &value, 
						bool &finished, 
						const AosRundataPtr &rdata);
	virtual bool	checkDocid(
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	getRsltSizeFromIIL(const AosRundataPtr &rdata) = 0;
	virtual bool 	moveTo(
						const i64 &startidx,
						const AosRundataPtr &rdata) = 0;
	virtual i64		getTotal(const AosRundataPtr &rdata) = 0;
	virtual bool 	setOrder(
						const OmnString &container,
						const OmnString &name, 
						const bool order, 
						const AosRundataPtr &) = 0;
	virtual bool 	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt, 
						const AosBitmapObjPtr &query_bitmap, 
						const AosQueryContextObjPtr &query_context, 
						const AosRundataPtr &rdata) = 0;
	virtual bool	getDocidsFromIIL(
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap, 
						const AosOpr opr, 
						const OmnString &value,
						const AosRundataPtr &rdata);
	virtual void	reset(const AosRundataPtr &rdata) = 0; 
	virtual bool 	collectInfo(const AosRundataPtr &rdata) = 0;
	virtual void	toString(OmnString &str) = 0;
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata);
	static AosXmlTagPtr getDocStatic(
						const u64 &docid,
						const AosRundataPtr &rdata);
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &key,
						const AosRundataPtr &rdata);

	virtual bool	addTerm(
						const AosQueryTermObjPtr &term,
						const AosRundataPtr &rdata){return false;}
	virtual AosQueryTermObjPtr	addTerm(
						const AosXmlTagPtr &term,
						const AosRundataPtr &rdata){return 0;}
	virtual AosQueryTermObjPtr * getTerms(i64 &num){return 0;}
	virtual i64		getNumTerms() const {return 0;}
	virtual bool	setPagesize(const i64 &psize){return false;}
	virtual bool	setStartIdx(const i64 &num){return false;}

	virtual bool	loadData(const AosRundataPtr &rdata){return false;}
	virtual bool	addStatTerm(
						const AosXmlTagPtr &statTerm,
						const AosRundataPtr &rdata){return false;}
	virtual void	setDocRetriever(const AosQueryTermObjPtr &term){}
	virtual bool	runQuery(const AosRundataPtr &rdata){return false;}
	virtual void 	setNeedOrder(const bool flag){}

	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) = 0;

	// Ice, 08/12/2011
	static AosQueryTermObjPtr getTerm(
						const AosQueryType::E type,
						const AosXmlTagPtr &tag, 
						const AosRundataPtr &rdata);

	// Chen Ding, 09/24/2011
	virtual bool	retrieveContainers(
						const AosXmlTagPtr &cond_def,
						AosHtmlCode &code,
						const AosRundataPtr &rdata) {return true;}
	static bool		retrieveContainersStatic(
						const AosXmlTagPtr &query_def,
						AosHtmlCode &code,
						const AosRundataPtr &rdata);

	virtual bool	queryFinished();
	virtual bool setNoMoreDocs()
	{
		return true;
	}

private:
	bool	registerTerm(
				const OmnSPtr<AosQueryTerm> &term,
				const OmnString &name,
				OmnString &errmsg);

public:
	AosQueryRsltObjPtr nextQueryRslt(i64 &size, bool &islast, const AosRundataPtr &rdata);
	virtual bool isCompoundQuery() const;

};
#endif

