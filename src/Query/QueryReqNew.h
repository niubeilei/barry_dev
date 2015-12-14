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
// 2014/04/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_QueryReqNew_h
#define Aos_Query_QueryReqNew_h

#include "Query/Ptrs.h"
#include "Query/QueryTerm.h"
#include "QueryUtil/QrUtil.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/QueryProcObj.h"
//#include "StatQueryAnalyzer/StatQueryUnit.h"
#include "Thread/Ptrs.h"

#include <map>

using namespace std;


class AosQueryReqNew : public AosQueryReqObj,
					   public AosQueryProcCallback
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftPsize = 20,
		eMaxPsize = 10000000
	};

private:
	static OmnMutexPtr	  smIDLock;
	static i64			  smIDCur;

	OmnMutexPtr			mLock;
	OmnMutexPtr			mLock1;
	OmnCondVarPtr       mCondVar;
	bool				mIsGood;
	u64					mQueryId;

	bool				mGetTotal;
	i64					mTotal;
	i64					mNumValues;
	i64					mStartIdx;
	i64					mPsize;

	OmnString			mOrderContainer;
	OmnString			mOrderFname;
	bool				mReverseOrder;

	AosXmlTagPtr        mOrigReq;
	i64					mCrtIdx;
	AosRundataPtr		mRundata;
	AosQueryTermObjPtr	mOrTerm;

	bool				mIsSmartQuery;

	AosQueryProcObjPtr	mQueryProc;
	AosQueryCacherObjPtr	mQueryCacher;
	AosQueryRsltObjPtr	mOrigQueryRslt;
	AosQueryRsltObjPtr	mQueryRawRslts;
	map<OmnString, AosBuffPtr>	mColumns;
	
public:
	AosQueryReqNew();
	~AosQueryReqNew();

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	i64		getNewQueryID();

	bool	parseSql(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cmd);

	bool	parseConds(
				const AosRundataPtr rdata,
				const AosXmlTagPtr &conds);

	bool	analyzeQuery(const AosRundataPtr &rdata);

	bool	normalQuery(const AosRundataPtr &rdata);

	bool	smartQuery(const AosRundataPtr &rdata);

	bool	willRunSmartQuery(const AosRundataPtr &rdata);

public:
	virtual AosQueryReqObjPtr createQuery(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual void	setStartIdx(const i64 &idx) {mStartIdx = 0; if (idx > 0) mStartIdx = idx;}
	virtual void	setPsize(const i64 &psize) {mPsize = eDftPsize; if (psize > 0 && psize <= eMaxPsize) mPsize = psize;}
	virtual void	setQueryId(const u64 &s) {mQueryId = s;}
	virtual bool	setNoMoreDocs(){return mOrTerm->setNoMoreDocs();}

	virtual bool	isGood() const {return mIsGood;}
	virtual bool	finished() const {if(!mOrTerm) return true; return mOrTerm->queryFinished();}
	virtual i64		getPageSize() const {return mPsize;}
	virtual i64		getStartIdx() const {return mStartIdx;}
	virtual u64		getQueryId() const {return mQueryId;}
	virtual i64		getTotal() {return mTotal;}
	virtual OmnString toString() const;
	virtual AosXmlTagPtr getOrigReq() const {return mOrigReq;}

	virtual bool	runQuery(const AosRundataPtr &rdata);

	virtual bool	generateRslt(
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata); 

	virtual bool	getRsltInfor(
						AosXmlTagPtr &infor,
						const AosRundataPtr &rdata);

	virtual bool	getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fieldname, 
						int &num_matched, 
						int *start_days, 
						int *end_days);

private:
	//QueryProcCallback interface
	virtual bool queryFinished(
					const AosRundataPtr &rdata,
					const AosQueryRsltObjPtr &results,
					const AosBitmapObjPtr &bitmap,
					const AosQueryProcObjPtr &proc);
	virtual bool queryFailed(
					const AosRundataPtr &rdata,
					const OmnString &errmsg);

public:

	virtual AosQueryTermObjPtr	getQueryTerm() const {return mOrTerm;}
	virtual vector<AosBuffPtr>	getBatchData() {vector<AosBuffPtr> dd; return dd;}
	virtual void	setQueryData(const AosQueryRsltObjPtr &query_rslt) {}
	virtual void	setQueryContext(const AosQueryContextObjPtr &query_context) {}
	virtual int		getBatchNum() {return 0;}
	virtual bool	procPublic(
						const AosXmlTagPtr &cmd,
						OmnString &contents,
						const AosRundataPtr &rdata) {return false;}
	virtual bool	runQuery(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &cmd,
						OmnString &contents) {return false;}
	virtual bool	runQuery(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &cmd,
						OmnString &contents, 
						const AosQueryProcCallbackPtr &callback) {return false;}
	virtual bool	generateResults(
						const AosRundataPtr &rdata, 
						const AosQueryRsltObjPtr &query_rslt,
						OmnString &records) {return false;}
	virtual bool	setQueryProc(const AosQueryProcObjPtr &query_proc) {return false;}
	virtual AosQueryProcObjPtr getQueryProc() const {return 0;}
	
	//virtual AosQueryTermObjPtr getOrTerm() {return 0;}
	//virtual vector<OmnString> & getTables() {static vector<OmnString> dd; return dd;}
	//virtual vector<AosQrUtil::FieldDef> & getSelectedFields() {static vector<AosQrUtil::FieldDef> dd; return dd;}
	//virtual vector<OmnString> & getGroupByFields() {static vector<OmnString> dd; return dd;}
	//virtual void setStatQueryUnit(const AosStatQueryUnitPtr &) {}
	//virtual AosStatQueryUnitPtr getStatQueryUnit() {return 0;}

};

#endif

