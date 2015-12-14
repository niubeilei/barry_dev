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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlSelectTable_H
#define AOS_JQLStatement_JqlSelectTable_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlQueryWhereConds.h"
#include "JQLStatement/JqlQueryWhereCond.h"
#include "JQLStatement/JqlSelectField.h"
#include "JQLStatement/JqlStmtTable.h"

#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"
#include <vector>

struct AosJqlSelect;
struct AosJqlWhere;

class AosJqlSelectTable : public AosJqlStatement
{
public:
	bool 									mIsInited;
	bool									mIsCondFilter;
	bool									mCondsIsAlwaysFalse;
	u64										mQueryId;
	OmnString								mTableNameStr;
	OmnString								mWhereCondsStr;
	AosJQLTableReferenceType 				mType;
	AosExprObjPtr							mAliasName;
	AosExprObjPtr							mTableName;
	// table dataset doc
	AosXmlTagPtr							mTableDoc;

	//2015/09/07 Phil  : input dataset for subquery
	AosDatasetObjPtr						mInputDataset;

	//whether or not this is a subquery
	bool									mIsSubQuery;



	vector<AosJqlSelectFieldPtr>			mInputFields;
	map<OmnString, AosJqlSelectFieldPtr>	mInputFieldsMap;
	vector<AosJqlSelectFieldPtr>			mInputExprFields;
	vector<AosJqlSelectFieldPtr>			mInputAggrFields;

	vector<AosJqlSelectFieldPtr>			mOutputFields;
	vector<AosJqlSelectFieldPtr>			mGroupByFields;
	vector<AosJqlSelectFieldPtr>			mCubeFields;
	vector<vector<AosJqlSelectFieldPtr> >			mRollupLists;
	vector<AosJqlSelectFieldPtr>			mHavingFields;
	vector<AosJqlOrderByFieldPtr>			mOrderByFields;
	vector<AosJqlQueryWhereCondPtr>			mConds;

public:
	AosJqlSelectTable();
	~AosJqlSelectTable();

	virtual bool init(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initOutputFieldList(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initWhereConds(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initGroupByFields(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initCubeFields(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initRollupLists(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initOrderByFields(const AosRundataPtr &rdata, const AosJqlSelectPtr &select);
	virtual bool initInputFields(const AosRundataPtr &rdata);
	virtual bool checkOrderByFeildIsInConds(const AosRundataPtr &rdata);

	virtual OmnString generateTableConf(const AosRundataPtr &rdata){return "";};
	virtual OmnString generateWhereConf(const AosRundataPtr &rdata){return "";};
	virtual OmnString generateGroupByConf(const AosRundataPtr &rdata);
	virtual OmnString generateOrderByConf(const AosRundataPtr &rdata);
	virtual OmnString generateHavingConf(const AosRundataPtr &rdata);
	virtual OmnString generateOrderByNormlConf(const AosJqlOrderByFieldPtr &order_field, const AosRundataPtr &rdata);
	virtual OmnString generateOutputDatasetConf(const AosRundataPtr &rdata);
	virtual OmnString generateBuffOutputDatasetConf(const AosRundataPtr &rdata);

	virtual	bool getAllField(const AosRundataPtr &rdata, const AosJqlSelectPtr &select, vector<AosJqlSelectFieldPtr> *&fields);

	virtual vector<AosJqlSelectFieldPtr> getTableFieldList(const OmnString &table_name, bool &rslt);

	virtual AosJqlSelectTable* clone(const AosRundataPtr &rdata) const;

	virtual void setType(AosJQLTableReferenceType type){mType = type;}
	virtual void setAliasName(AosExprObj* alias_name){mAliasName = alias_name;}
	virtual void setTable(AosExprObj* table){mTableName = table;};
	virtual AosExprObjPtr getTable(){return mTableName;};
	virtual AosJQLTableReferenceType getType(){return mType;} 
	virtual AosExprObjPtr getAliasName(){return mAliasName;};

	bool handleAliasName(const AosRundataPtr &rdata, const AosJqlSelectPtr &select, const AosJqlSelectFieldPtr &field);
	bool handleField(const AosRundataPtr &rdata, const AosJqlSelectPtr &select, const AosJqlSelectFieldPtr &field);
	bool checkIsOrderFieldOrGroupByField(const AosJqlSelectFieldPtr &field, const AosRundataPtr &rdata);
	AosJqlColumnPtr getDatasetColumn(
		const AosRundataPtr &rdata, 
		const AosDatasetObjPtr &dataset,
		const OmnString &field_name);
/*
	AosXmlTagPtr  getField(
		const AosRundataPtr &rdata, 
		const OmnString &table_name,
		const OmnString &field_name);
*/
	//AosDatasetObjPtr getInputDataset(){return mInputDataset;}
	AosXmlTagPtr getDatasetField(
		const AosRundataPtr &rdata,
		const AosDatasetObjPtr &dataset,
		AosJqlSelectFieldPtr &field);

	virtual void IsStatTable (bool &isStat){}

};


#endif
