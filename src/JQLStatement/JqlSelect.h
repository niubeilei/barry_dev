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
#ifndef AOS_JQLStatement_JqlSelect_H
#define AOS_JQLStatement_JqlSelect_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlTableReference.h"
#include "JQLStatement/JqlHaving.h"
#include "JQLStatement/JqlGroupBy.h"
#include "JQLStatement/JqlOrderBy.h"
#include "JQLStatement/JqlLimit.h"
#include "SEInterfaces/ExprObj.h"      
#include "XmlUtil/XmlTag.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlSelectField;
class AosJqlTableReference;
typedef vector<AosJqlSelectFieldPtr> AosJqlSelectFieldList;
typedef vector<AosJqlTableReferencePtr> AosTableReferences;

struct AosAliasName : public AosJqlDataStruct
{
public:
	OmnString 		mOName;
	OmnString 		mCName;
	AosJqlSelectFieldPtr mField;
}; 

class AosJqlSelect : public AosJqlDataStruct
{
public:
	AosJQLStmtSelectOptionList 		mType;
	AosJqlSelectFieldList*   		mFieldListPtr;
	AosJqlSelectInto*				mSelectIntoPtr;
	AosTableReferences*			  	mTableReferencesPtr;
	AosJqlWherePtr					mWherePtr;
	AosJqlGroupByPtr				mGroupByPtr;
	AosJqlGroupByPtr				mRollupGrpByFieldsGrp;
	AosJqlHavingPtr					mHavingPtr;
	AosJqlOrderByPtr				mOrderByPtr;
	AosJqlLimitPtr					mLimitPtr;
	map<OmnString, AosAliasName>	mAliasNameMap;
	// tmp
	map<OmnString, OmnString>		mNameToAlias;
	vector<AosExprObjPtr>			mAllTable;
	AosExprObjPtr					mHackFormat;
	AosExprObjPtr					mHackIntoFile;
	AosExprObjPtr					mHackConvertTo;

public:
	AosJqlSelect();
	~AosJqlSelect();

	bool init(const AosRundataPtr &rdata);
	bool generateQueryConf(const AosRundataPtr &rdata, const AosXmlTagPtr &query_conf);
	OmnString generateFieldConf(const AosRundataPtr &rdata);
	bool handleAliasName(const AosRundataPtr &rdata); 

	bool getAllTableDef(
			const AosRundataPtr &rdata,
			vector<AosXmlTagPtr> &table_defs);

	// 2015/09/14
	virtual void setSubQuery();
};

#endif
