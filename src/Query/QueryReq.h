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
#ifndef Aos_Query_QueryReq_h
#define Aos_Query_QueryReq_h

#include "LogUtil/Ptrs.h"
#include "Query/Ptrs.h"
#include "Query/QueryTerm.h"
#include "QueryUtil/QrUtil.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryProcCallback.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEUtil/AsyncRespCaller.h"
#include "SEInterfaces/QueryReqCaller.h"
#include "Thread/Ptrs.h"
#include "Util/StrParser.h"
#include "XmlInterface/XmlRc.h"
#include "QueryClient/Ptrs.h"
#include <vector>

class AosQueryReq : public AosQueryReqObj, 
					public AosQueryReqCaller,
					public AosAsyncRespCaller,
					public AosQueryProcCallback
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxTags = 20,
		eMaxWords = 20,
		eMaxAssign = 50,
		eMaxDocs = 1000000,
		eMaxFields = 50,
		eMaxCondWords = 500,
		eMaxOrderNames = 30,
		eMaxClause = 30,
		eMaxCtnrs = 5,
		eMaxJoins = 5,
		eDftPsize = 20,
		AOS_ATTR_FLAG = '1',
		AOS_CDATA_FLAG = '2',
		AOS_TEXT_FLAG = '3'
	};

public:
	static AosWordNormPtr smWordNorm;

private:
	static OmnMutexPtr	  smIDLock;
	static i64			  smIDCur;

private:
	bool				mIsGood;
	bool				mIsNew;
	u64					mQueryId;
	OmnString			mOrderContainer;
	OmnString			mOrderFname;
	bool				mReverseOrder;
	i64					mPsize;
	i64					mCrtIdx;			// current index in the result
	i64					mStartIdx;
	i64					mNumDocs;
	i64					mTotal;
	AosQueryTermObjPtr	mOrTerm;
	vector<AosXmlTagPtr>	mXmls;

	OmnString			mSubopr;
	OmnMutexPtr			mLock;
	OmnMutexPtr			mLock1;
	OmnCondVarPtr       mCondVar;
	bool				mRemoveMeta;
	AosUpdatePtr 		mUpdate;
	bool				mGetTotal;
	vector<AosQrUtil::FieldDef>	mFieldDef;
	vector<AosQrUtil::JoinDef>	mJoinDefs;
	AosXmlLogPtr		mLog;		// Chen Ding, 2013/01/27

	u64					mQueryStartTime;
	AosRundataPtr		mRundata;

	//jackie
	AosBatchQueryPtr 	mBatchQuery;
	bool				mBatchQueryFlag;
	bool				mIsSmartQuery;		// Chen Ding, 2013/08/18
	AosXmlTagPtr        mOrigReq;

	AosQueryRsltObjPtr	mOrigQueryRslt;
	AosQueryRsltObjPtr	mQueryRawRslts;
	int					mBatchNum;
	AosXmlTagPtr		mConfig;
	u64					mTimestampBlockEnd;
	map<OmnString, AosBuffPtr>	mColumns;
	vector<AosBuffPtr>	mBatchData;
	int					mNumPendingReqs;
	AosQueryProcObjPtr	mQueryProc;
	// AosQueryProcCallbackPtr	mCallback;
	AosQueryCacherObjPtr	mQueryCacher;
	
	bool				mIsStatQuery;		// Ketty 2014/01/22
	vector<OmnString>	mTables;			// Ketty 2014/02/13
	vector<OmnString>	mGroupByFields;		// Ketty 2014/04/10
	//AosStatQueryUnitPtr mStatQueryUnit; 	// Ketty 2014/02/20

	// andy
	bool				mIsNeedSmartQuery;
public:
	AosQueryReq();
	AosQueryReq(const i64 &qid, const AosRundataPtr &rdata);
	AosQueryReq(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosQueryReq();

	bool createRecord(
					OmnString &records,
					vector<AosXmlTagPtr> &xmls,
					OmnString &errmsg, 
					const AosRundataPtr &rdata)
	{
		return AosQrUtil::createRecord(rdata, records, xmls, mFieldDef);
	}
	
	// AosTransCallerObj interface
	virtual void callback(
					const AosTransPtr &trans,
				    const AosBuffPtr &resp,
					const bool svr_death);

	virtual AosQueryReqObjPtr createQuery(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);

	virtual u64		getQueryId() const {return mQueryId;}
	virtual void	setQueryId(const u64 &s) {mQueryId = s;}
	virtual bool	isGood() const {return mIsGood;}

	virtual bool	finished()const;
	virtual bool	procPublic(
						const AosXmlTagPtr &cmd,
						OmnString &contents,
						const AosRundataPtr &rdata);

	virtual void	setStartIdx(const i64 &idx)
					{
						// If s is empty or negative, ignore it
						mStartIdx = 0;
						if (idx <= 0) return;
						mStartIdx = idx;
					}
	//QueryProcCallback interface
	virtual bool queryFinished(
					const AosRundataPtr &rdata,
					const AosQueryRsltObjPtr &results,
					const AosBitmapObjPtr &bitmap,
					const AosQueryProcObjPtr &proc);
	virtual bool queryFailed(
					const AosRundataPtr &rdata,
					const OmnString &errmsg);


	virtual bool	runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents);

	// Chen Ding, 2013/08/18
	virtual bool	runQuery(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd,
		OmnString &contents, 
		const AosQueryProcCallbackPtr &callback);

	bool		isNew() const {return mIsNew;}
	void		setPsize(const OmnString &pp)
				{
					if (pp == "") return;
					mPsize = atoi(pp.data());
					if (mPsize <= 0) mPsize = eDftPsize;
					if (mPsize > eMaxDocs) mPsize = eMaxDocs;
				}
	void	setRemoveMeta(const OmnString &ss) {mRemoveMeta = (ss == "true");}
	void	setSubopr(const OmnString &subopr) {mSubopr = subopr;}
	void	setIsGood(const bool b) {mIsGood = b;}
	bool	parseSqlPublic(const AosRundataPtr &rdata, const AosXmlTagPtr &cmd);
	bool	parseSqlPublicVer2(const AosRundataPtr &rdata, const AosXmlTagPtr &cmd);
	void	setIdx(const i64 &i) {mCrtIdx = i;}
	bool 	setOrder(
					const OmnString &container,
					const OmnString &fname, 
					const OmnString &reverse, 
					const AosRundataPtr &rdata);
	bool	setJoins(const AosXmlTagPtr &joins, OmnString &errmsg);
	bool	setJoins(const OmnString &join, OmnString &errmsg);
	bool	getJoins(const AosXmlTagPtr &xml, const AosRundataPtr &rdata);
	bool 	appendJoins(const OmnString &joins, OmnString &errmsg);
	void 	setGetTotal(const bool b) {mGetTotal = b;}

private:
	bool parseConds(
		const AosRundataPtr rdata,
		const AosXmlTagPtr &conds);

	bool	procSQL(const AosRundataPtr &rdata, OmnString &contents);

	bool 	compatibilityCheck(OmnString &fielddef);

	bool retrieveVersions(
		const AosRundataPtr &rdata,
		OmnString &contents, 
		bool &finished);

	// Chen Ding, 08/18/2011
	// This function is no longer needed
	// AosXmlTagPtr	parseQuery(
	//	const AosRundataPtr &rdata,
	//	const OmnString &query);
	
	//bool normalQuery(const AosRundataPtr &rdata, OmnString &records, bool &finished);

	bool queryLogs(OmnString &records, bool &finished);
	bool addContainer(OmnString &cname, const OmnString &cid);
	
	bool processStatistics(
			const AosQueryTermObjPtr &cond, 
			const AosXmlTagPtr &stat_tag,
	        const AosRundataPtr &rdata);

	// Chen Ding, 01/02/2013
	//bool analyzeQuery(OmnString &records, bool &finished, const AosRundataPtr &rdata);
	//bool analyzeQuery( 
	//		OmnString &records, 
	//		bool &finished,
	//		bool &finish_up_query,
	//		const AosRundataPtr &rdata);
	bool finishUpQuery(
			OmnString &contents, 
			const OmnString &records, 
			bool &finished,
			const AosRundataPtr &rdata);
	bool finishUpQuerySmart(
			OmnString &contents, 
			const OmnString &records, 
			bool &finished,
			const AosRundataPtr &rdata);

	bool analyzeQuery(const AosRundataPtr &rdata);
	bool normalQuery(const AosRundataPtr &rdata);
	bool smartQuery(const AosRundataPtr &rdata);
	bool generateRslt(
			OmnString &records, 
			bool &finished,
			const AosRundataPtr &rdata); 

	bool generateRsltSmart(
			OmnString &records, 
			bool &finished,
			const AosRundataPtr &rdata); 

	i64	getNewQueryID();
	bool smartQueryPostProcess(OmnString &contents,
							   const AosRundataPtr &rdata);

public:
	// Chen Ding, 2013/01/27
	AosXmlLogPtr getLog();
	void appendLog(const OmnString &str);

	virtual u64					getQueryStartTime() const {return mQueryStartTime;}
	virtual AosRundataPtr		getRundata() const {return mRundata;}
	virtual AosQueryTermObjPtr	getQueryTerm() const {return mOrTerm;}
	virtual void setQueryData(const AosQueryRsltObjPtr &query_rslt) 
	{
		mOrTerm->setQueryData(query_rslt);
	}
	i64	getTotal(){return mTotal;}
	virtual void setQueryContext(const AosQueryContextObjPtr &query_context)
	{
		mOrTerm->setQueryContext(query_context);
	}

	virtual OmnString toString() const;
	virtual AosXmlTagPtr getOrigReq() const;
	virtual i64 getPageSize() const {return mPsize;}
	virtual i64 getStartIdx() const {return mStartIdx;}
	virtual bool generateResults(
						const AosRundataPtr &rdata, 
						const AosQueryRsltObjPtr &query_rslt,
						OmnString &records);
	bool fetchFieldsFinished(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff);
	vector<AosBuffPtr>	getBatchData(){return mBatchData;}
	int	getBatchNum(){return mBatchNum;}
	bool setNoMoreDocs(){return mOrTerm->setNoMoreDocs();}
	virtual bool setQueryProc(const AosQueryProcObjPtr &query_proc);
	virtual AosQueryProcObjPtr getQueryProc() const;

	// Chen Ding, 2014/01/31
	virtual bool getTimeCond(
						const AosRundataPtr &rdata, 
						const OmnString &time_fieldname, 
						int &num_matched, 
						int *start_days, 
						int *end_days);

private:
	// Chen Ding, 2013/09/14
	bool addLogEntry(const AosRundataPtr &rdata, const OmnString &msg, const int64_t pos);
	bool procQueryRawRslts(const AosRundataPtr &rdata);
	bool fetchFields(const AosRundataPtr &rdata);
	bool callerQueryFailed(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg);
	bool procJoins(const AosRundataPtr &rdata);
	bool generateRecords(const AosRundataPtr &rdata);
	bool willRunSmartQuery(const AosRundataPtr &rdata);

	// Ketty 2014/01/22
	bool 	parseQueryTables(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cmd);
	bool 	parseGroupByFields(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cmd);
	bool 	checkIsStatQuery(const AosRundataPtr &rdata);
	bool 	runStatQuery(const AosRundataPtr &rdata);

public:
	// Ketty 2014/02/13
	virtual AosQueryTermObjPtr getOrTerm(){ return mOrTerm; };
	virtual vector<OmnString> & getTables(){ return mTables;};
	virtual vector<OmnString> & getGroupByFields(){ return mGroupByFields; };
	virtual vector<AosQrUtil::FieldDef> & getSelectedFields();
	//virtual void setStatQueryUnit(const AosStatQueryUnitPtr &);
	//virtual AosStatQueryUnitPtr getStatQueryUnit();


	virtual bool	runQuery(const AosRundataPtr &rdata) {return false;}

	virtual bool	generateRslt(
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata) {return false;}

	virtual bool	getRsltInfor(
						AosXmlTagPtr &infor,
						const AosRundataPtr &rdata) {return false;}

};

#endif

