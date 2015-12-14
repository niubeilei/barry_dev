////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/05/08	Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_JQLStatment_JqlStatQuery_h
#define AOS_JQLStatment_JqlStatQuery_h

#include "StatUtil/Ptrs.h"
#include "StatUtil/StatTimeUnit.h"
#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/StatCond.h"

#include "AosConf/DataRecord.h"
#include "AosConf/DataScanner.h"
#include "AosConf/DataFieldExpr.h" 
#include "AosConf/DataSchemaStatRecord.h"

#include "JQLStatement/JqlSelect.h"
#include "JQLExpr/ExprString.h"

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;
using namespace AosConf;

class AosJqlStatQuery
{
	struct MeasureGroup
	{
		AosStatisticPtr	mStat;	
		vector<AosJqlSelectFieldPtr> mMeasureFields;
		vector<AosJqlSelectFieldPtr> mGrpbyFields;
	};

private:
	vector<MeasureGroup>	mQryMeasureGrps;
	
	AosStatisticPtr			mBasicStat;
	vector<AosJqlSelectFieldPtr> mSelectFields;
	vector<AosJqlSelectFieldPtr> mGrpbyFields;
	vector<AosJqlSelectFieldPtr> mCubeGrpbyFields;
	vector< vector<AosJqlSelectFieldPtr> > mRollupGrpbyFieldsGrp;
	vector<AosJqlQueryWhereCondPtr> mQryConds;
	vector<AosJqlOrderByFieldPtr> mOrderbyFields;
	AosJqlLimitPtr 					mLimitPtr;
	AosJqlHavingPtr 				mHavingPtr;
//	OmnString						mTimeUnit;
	AosStatTimeUnit::E				mTimeUnit;
	vector<OmnString>				mMeasureCondFields;
	//yang
	//static vector<AosStatisticPtr> mAllStats;


public:

protected:
	AosJqlStatQuery();
	~AosJqlStatQuery();

	static bool checkIsStatQuery(
				const AosRundataPtr &rdata,
				const AosJqlSelectTablePtr &base_table,
				vector<AosXmlTagPtr> &all_stat_docs);

	bool 	init(
				const AosRundataPtr &rdata,
				const OmnString &stat_objid,
				vector<AosJqlSelectFieldPtr> &select_fields,
				vector<AosJqlSelectFieldPtr>  &grpby_fields,
				vector<AosJqlSelectFieldPtr>  &cube_grpby_fields,
				vector< vector<AosJqlSelectFieldPtr> > &rollup_grpby_fields_grp,
				vector<AosJqlQueryWhereCondPtr> &qry_conds,
				vector<AosJqlOrderByFieldPtr> &orderby_fields,
				AosJqlHavingPtr &having_conds,
				AosJqlLimitPtr &limitPtr,
				vector<AosXmlTagPtr> &all_stat_docs);
	
	OmnString generateTableConf(
				const AosRundataPtr &rdata,
				const OmnString &table_name,
				AosJqlSelectTablePtr &table);

private:
	static bool initStatistics(
				const AosRundataPtr &rdata,
				vector<AosXmlTagPtr> &all_stat_docs,
				vector<AosStatisticPtr> &all_stats);

	static bool analyzeQryFields(
				const AosRundataPtr &rdata,
				vector<AosJqlSelectFieldPtr> &raw_select_fields,
				vector<AosJqlSelectFieldPtr> &raw_grpby_fields,
				vector<AosJqlOrderByFieldPtr> &raw_orderby_fields,
				vector<AosJqlSelectFieldPtr> &norm_fields,
				vector<AosJqlSelectFieldPtr> &measure_fields,
				bool checkFlag = false);

	static bool analyzeField(
				const AosRundataPtr &rdata,
				const AosJqlSelectFieldPtr &raw_field,
				vector<AosJqlSelectFieldPtr> &norm_fields,
				vector<AosJqlSelectFieldPtr> &measure_fields,
				vector<AosJqlSelectFieldPtr> &complex_fields,
				const bool ignore_timeunit_field);

	static bool addToFieldVector(
				vector<AosJqlSelectFieldPtr> &vt_field,
				const AosJqlSelectFieldPtr &crt_field);

	static bool checkIsPickStat(
				const AosRundataPtr &rdata,
				const AosStatisticPtr &stat,
				const AosJqlSelectFieldPtr &measure_field,
				vector<AosJqlSelectFieldPtr> &norm_fields,
				vector<AosJqlQueryWhereCondPtr> &qry_conds);

	static bool checkIsPickOneStat(
				const AosRundataPtr &rdata,
				const AosStatisticPtr &stat,
				const vector<AosJqlSelectFieldPtr> &measure_fields,
				const vector<AosJqlSelectFieldPtr> &norm_fields,
				const vector<AosJqlQueryWhereCondPtr> &qry_conds,
				AosStatTimeUnit::E 	&time_unit);		


	static AosStatFieldPtr getStatField(
				const AosRundataPtr &rdata,
				const AosJqlSelectFieldPtr &field);

	AosStatisticPtr getBasicStat(vector<AosStatisticPtr> &all_stats);

	bool 	pickPerfectStat(
				const AosRundataPtr &rdata,
				vector<AosStatisticPtr> &all_stats,
				vector<AosJqlSelectFieldPtr> &select_fields,
				vector<AosJqlSelectFieldPtr> &grpby_fields,
				vector<AosJqlQueryWhereCondPtr> &qry_conds,
				vector<AosJqlOrderByFieldPtr> &orderby_fields,
				vector<AosXmlTagPtr> &all_stat_docs);

	bool 	addToMeasureGrps(
				const AosStatisticPtr &stat,
				const AosJqlSelectFieldPtr &m_field,
				vector<AosJqlSelectFieldPtr> &grpby_fields);

	bool 	generateStatQryInfoConf(const AosRundataPtr &rdata, OmnString &str);

	bool statCondExprProc(
				AosStatCond &stat_cond,
				const OmnString &opr_str);
	
		OmnString generateDataScannerConf(const AosRundataPtr &rdata);

	OmnString generateInputRecordConf(const AosRundataPtr &rdata, const OmnString& tableName);

	bool 	appendNormFieldPriv(
				const AosRundataPtr &rdata,
				boost::shared_ptr<DataRecord> &drec,
				const OmnString& tableName,
				const AosJqlSelectFieldPtr &field);

	bool 	appendExprFieldPriv(
				const AosRundataPtr &rdata,
				boost::shared_ptr<DataRecord> &drec,
				//const OmnString &field_name,
				const AosJqlSelectFieldPtr &field);

	static AosJqlSelectFieldPtr genCountField(const AosRundataPtr &rdata);

	bool	analyzeMeasureFields(
			const AosRundataPtr &rdata,
			const vector<AosJqlSelectFieldPtr> &measure_fields,
			vector<OmnString> &mea_grpby_fields);

	OmnString getStrTimeUnit(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &statDoc);
	
	bool syntaxCheck(const AosRundataPtr &rdata);

	bool groupBySyntaxCheck(
			const AosRundataPtr rdata,
			OmnString &errmsg,
			vector<AosJqlSelectFieldPtr> &measureFields);

	bool orderBySyntaxCheck(
			const AosRundataPtr &rdata,
			OmnString &errmsg);

	bool havingConditionSyntaxCheck(
			const AosRundataPtr &rdata,
			OmnString &errmsg,
			const vector<AosJqlSelectFieldPtr> &measureFields);

	bool isInGrpbyFields(const AosRundataPtr &rdata,const OmnString &orderByField);

	static bool isInGrpbyFields(
			const AosRundataPtr &rdata,
			const AosJqlSelectFieldPtr &selectField,
			const vector<AosJqlSelectFieldPtr> &groupByFields);

	bool convertTimeFormat(const AosRundataPtr& rdata,string &format);

	static OmnString getFieldName(const OmnString &field);

	bool hitShuffleFields(const AosRundataPtr &rdata);
};

#endif
