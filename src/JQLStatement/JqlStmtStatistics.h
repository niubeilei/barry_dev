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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtStatistics_H
#define AOS_JQLStatement_JqlStmtStatistics_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"

class AosJqlStmtStatistics : public AosJqlStatement
{
public:
		struct AosMeasure 
		{
			AosExprObj *mExpr;
			AosExprList *mExprList;

			AosMeasure()
			{
				mExpr = 0;
				mExprList = 0;
			}

			~AosMeasure()
			{
				delete mExpr;
				delete mExprList;
			}
		};

		struct ExprCmp                                                        
		{
			bool operator () (AosExprObjPtr lhs, AosExprObjPtr rhs) const
			{
				OmnString lhs_name = lhs->getValue(0);
				OmnString rhs_name = rhs->getValue(0);
				if (lhs_name < rhs_name)
				{
					return true;
				}

				return false;
			}
		};

typedef std::set<AosExprObjPtr, ExprCmp> AosExprSet;
private:
	OmnString						mStatName;		// this is user config's stat_name.
	OmnString						mStatObjid;
	OmnString						mTableName;
	//AosExprList*					mKeyFields;
	AosExprSet						mKeyFields;
	AosExprList*					mStatValues;
	OmnString						mTimeFieldName;
	OmnString					    mTimeOriginalFormat;
	OmnString						mTimeUnit;
	u32								mStartIdx;
	vector<AosExprList*>*			mUpdateDimensions;
	vector<AosExprSet>				mDimensions;
	vector<AosExprList*>*			mGroupByFields;
	AosExprList*					mShuffleFields;
	AosExprList*					mTableNames;
	AosExprObj						*mWhereCond;
	vector<AosMeasure*>				*mMeasures;
	AosExprSet						mDistinctGroup;
	AosExprSet						mDistinct;
	bool							mHasInsertDistinct;
	OmnString						mStatIdentifyKey;
	AosXmlTagPtr					mStatisticDoc;

	OmnString						mInteStatName;		// Ketty 2014/11/19
	AosExprSet						mDistKeyFields;
	AosExprList*					mDistStatValues;	// Ketty 2014/11/07
	AosExprList*					mNormStatValues;	// Ketty 2014/11/07

public:
	OmnString						mErrmsg;

public:
	AosJqlStmtStatistics(const OmnString &errmsg);
	AosJqlStmtStatistics();
	~AosJqlStmtStatistics();

	//member functions
	bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	AosJqlStatement *clone();
	virtual void dump();

	//getter/setters
	void setTimeFieldName(AosExprObj *name);
	void setTimeOriginalFormat(OmnString format);
	void setStatName(OmnString name);
	void setTableName(OmnString name);
	void setTableNames(AosExprList *names);
	void setKeyFields(AosExprSet &key_fields);
	void setStatValues(AosExprList* stat_values);
	void setTimeUnit(OmnString time_unit);
	void setDimensions(vector<AosExprList*> *dimensions);
	void setDimensions(vector<AosExprSet> dimensions);
	void setWhereCond(AosExprObj *cond);
	void setMeasures(vector<AosJqlStmtStatistics::AosMeasure*> *measures);
	void setGroupByFields(vector<AosExprList*> *fields);
	void setShuffleField(AosExprList *fields);

private:
	bool createStatistics(const AosRundataPtr &rdata);
	bool showStatistics(const AosRundataPtr &rdata);
	bool dropStatistics(const AosRundataPtr &rdata);
	bool describeStatistics(const AosRundataPtr &rdata);
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	bool addNodeInTable(const AosRundataPtr &rdata);
	AosXmlTagPtr createStatDoc(const AosRundataPtr &rdata);

	OmnString 	createKeyFieldsStr(
					const AosRundataPtr &rdata,
					AosExprSet  &key_fields,
					OmnString &identify_key);
	
	OmnString createStatValueStr(
					const AosRundataPtr &rdata,
					AosExprList* stat_values);

	OmnString createStat(AosExprSet  &key_fields, const AosRundataPtr &rdata);
	bool createControlDoc(
		const OmnString &root_control_objid, 
		//const OmnString &extension_control_objid,
		const AosRundataPtr &rdata);

	bool	updateStatistics(const AosRundataPtr &rdata);

	OmnString	createDistinctStr(
					const OmnString &distinct_field,
					const AosRundataPtr &rdata);
	bool	isDistinct(
				AosExprSet &key_fields,
				OmnString &distinct_field,
				const AosRundataPtr &rdata);
	
	bool 	checkConfig(const AosRundataPtr &rdata);

	bool	checkStatValues(const AosRundataPtr &rdata);

	bool	checkSameLevel(
				vector<AosExprSet> &vec,
				vector<AosExprSet> &dimension_vec,
				const AosRundataPtr &rdata);

	bool	checkNoRepeatFields(
				AosExprList* fields,
				AosExprSet &group,
				const AosRundataPtr &rdata);

	bool	checkIsChildLevel(
				AosExprSet &fields,
				const AosRundataPtr &rdata);

	bool	checkNoRepeatGroups(
				AosExprSet &fields1,
				AosExprSet &fields2,
				const AosRundataPtr &rdata);

	bool 	checkUpdateConfig(
				const AosXmlTagPtr &internal_statistics, 
				const AosRundataPtr &rdata);
	
	OmnString	getObjid(
					const OmnString &tname,
					const OmnString &dname);
	
	
	// Ketty 2014/10/11
	bool 	createStatisticsNew(const AosRundataPtr &rdata);

	AosXmlTagPtr createStatDefDoc(const AosRundataPtr &rdata);

	AosXmlTagPtr createStatDefDocConf(
				const AosRundataPtr &rdata,
				AosExprSet	&key_fields,
				AosExprList* stat_values);
#if 0	
	bool checkTimeField(const AosRundataPtr &rdata);

	OmnString	convertTimeFormat(
				const AosRundataPtr &rdata,
				bool &flag,
				const OmnString &format);

	OmnString reverseConvertTimeFormat(
				const AosRundataPtr &rdata,
				const OmnString &tfmt);
#endif
	bool 	addStatDefToTable(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_def_doc);

	AosXmlTagPtr retrieveStatDoc(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &table_doc);
	
	bool 	createStatDoc(
				const AosRundataPtr &rdata,
				AosXmlTagPtr &stat_doc,
				OmnString &stat_doc_objid);

	bool	 refactorInternalStat(
				const AosRundataPtr &rdata,
				AosExprSet	&key_fields,
				AosExprList* stat_values);

	bool 	refactorInternalDistStat(const AosRundataPtr &rdata);
	OmnString getType(const AosRundataPtr &rdata, const OmnString field_name);


	bool syntaxCheck(
				const AosRundataPtr &rdata,
				const AosExprObjPtr &shuffleField,
				const AosExprSet	&keyFields);

};

#endif
