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
// 2013/01/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryTermObj_h
#define Aos_SEInterfaces_QueryTermObj_h

#include "QueryUtil/QrUtil.h"
#include "SEInterfaces/QueryType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
//#include "StatUtil/StatUtil.h"

struct AosHtmlCode;

class AosQueryTermObj : virtual public OmnRCObject
{
public:
	virtual AosQueryType::E	getType() const = 0;
	virtual OmnString getTermName() const = 0;		// Chen Ding, 2013/12/06
	virtual bool	isGood() const = 0;
	virtual bool	isReverse() const = 0;
	virtual bool	isOrdered() const = 0;
	virtual void	setOrdered(const bool order) = 0;
	virtual void	setWithValues(bool withvalue) = 0;
	virtual u64		getTotalInRslt() const = 0;
	virtual void	setTotalInRslt(const u64 &total) = 0;
	virtual OmnString	getIILName() const = 0;
	virtual AosCondInfo getCondInfo() const = 0;

	virtual bool	canUseBitmapQuery() const = 0;
	virtual bool	hasDoc() const = 0;
	virtual bool	needHandleNextDocid() const = 0;
	virtual AosQueryRsltObjPtr getQueryData() const = 0;
	virtual AosQueryContextObjPtr getQueryContext() const = 0;
	virtual void	setQueryData(const AosQueryRsltObjPtr &query_rslt) = 0;
	virtual void	setQueryContext(const AosQueryContextObjPtr &query_context) = 0;

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
						const AosRundataPtr &rdata) = 0;
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
						const AosRundataPtr &rdata) = 0;
	virtual void	reset(const AosRundataPtr &rdata) = 0; 
	virtual bool 	collectInfo(const AosRundataPtr &rdata) = 0;
	virtual void	toString(OmnString &str) = 0;
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr getDoc(
						const u64 &docid,
						const OmnString &value,
						const AosRundataPtr &rdata) = 0;
	virtual bool	addTerm(
						const AosQueryTermObjPtr &term,
						const AosRundataPtr &rdata) = 0;
	virtual AosQueryTermObjPtr	addTerm(
						const AosXmlTagPtr &term,
						const AosRundataPtr &rdata) = 0;
	virtual AosQueryTermObjPtr * getTerms(i64 &num) = 0;
	virtual i64		getNumTerms() const = 0;
	virtual bool	runQuery(const AosRundataPtr &rdata) = 0;

	virtual bool	setPagesize(const i64 &psize) = 0;
//	virtual bool	setExpectSize(const i64 &num) = 0;
	virtual bool	setStartIdx(const i64 &num) = 0;

	virtual bool	loadData(const AosRundataPtr &rdata) = 0;
	virtual bool	addStatTerm(
						const AosXmlTagPtr &statTerm,
						const AosRundataPtr &rdata) = 0;
	virtual void	setDocRetriever(const AosQueryTermObjPtr &tt) = 0;
	virtual void 	setNeedOrder(const bool flag) = 0;
	virtual AosQueryTermObjPtr clone(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) = 0;

	virtual bool	retrieveContainers(
						const AosXmlTagPtr &cond_def,
						AosHtmlCode &code,
						const AosRundataPtr &rdata) = 0;

	virtual bool	queryFinished() = 0;
	virtual bool	withValue() const = 0;
	virtual bool setNoMoreDocs() = 0;
	virtual bool isCompoundQuery() const = 0;

	// Chen Ding, 2014/01/29
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fname, 
						int &num_matched,
						int *start_days, 
						int *end_days)
	{
		num_matched = 0;
		return true;
	}
	
	// Ketty 2014/02/13
	//virtual bool findTermByStatKeyField(
	//			const OmnString &key_field, 
	//			bool &find,
	//			AosQueryTermObjPtr &term)
	//{
	//	return false;
	//}
	// Ketty 2014/02/13 End.
	
	virtual bool getCondTerms(vector<AosQueryTermObjPtr> &) { return false; };
	virtual bool addInternalStatIdTerm(
				const AosRundataPtr &rdata,
				const OmnString &iil_name,
				const u32 stat_internal_id)
	{
		return false;
	}
	virtual bool removeStatValueTerms(
				const AosRundataPtr &rdata,
				vector<AosQueryTermObjPtr> &value_terms)
	{
		return false;
	}
	
	virtual bool isStatValueTerm(){ return false;}; 
	virtual bool isTimeTerm(){ return false;}; 
	virtual OmnString getFieldName(){ return ""; };
	virtual bool setIILName(const OmnString &iil_name){ return false; };
	//virtual bool initStatTimeCond(const AosRundataPtr &rdata, AosStatQueryCondInfo &cond){ return false;};
	
};
#endif
