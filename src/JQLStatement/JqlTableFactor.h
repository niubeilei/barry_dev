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
#ifndef AOS_JQLStatement_JqlTableFactor_H
#define AOS_JQLStatement_JqlTableFactor_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSingleTable.h"
#include "JQLStatement/JqlSelect.h"
#include "JQLStatement/JqlStmtQuery.h" 

#include "Rundata/Rundata.h"
#include "Util/String.h"


class AosJqlSubQueryTable;
class AosJqlTableFactor : public AosJqlDataStruct   
{
public:

	AosJQLTableReferenceType 	mType;
	AosExprObjPtr				mTableName;
	AosExprObjPtr				mAliasName;

	AosJqlSingleTablePtr		mTable;
	AosJqlSubQueryTablePtr		mVirtualTable;

	AosExprList					*mIndexHint;
	AosJqlStmtQuery             *mQuery;

public:
	AosJqlTableFactor();
	~AosJqlTableFactor();

	bool init(
				const AosRundataPtr &rdata, 
				const AosJqlSelectPtr &select);

	bool getAllField(
				const AosRundataPtr &rdata,
				const AosJqlSelectPtr &select, 
				vector<AosJqlSelectFieldPtr> *&fields);

	bool getAllTableDef(
				const AosRundataPtr &rdata,
				vector<AosXmlTagPtr> &table_def);

	OmnString generateTableConf(const AosRundataPtr &rdata);
	AosJqlTableFactor* clone(const AosRundataPtr &rdata) const;
	vector<AosJqlSelectFieldPtr> getTableFieldList( const OmnString &table_name, bool &rslt);

	void setVirtualTable(AosJqlSubQueryTable* virtual_table);
	void setType(AosJQLTableReferenceType type){mType = type;}
	void setAliasName(AosExprObj* alias_name){mAliasName = alias_name;}
	void setTable(AosExprObj* table){mTableName = table;}
	void setIndexHint(AosExprList* index_hint){mIndexHint = index_hint;}

	AosJQLTableReferenceType getType(){return mType;} 
	AosExprObjPtr getAliasName(){return mAliasName;};

	AosJqlSubQueryTablePtr getVirtualTbale();
	//Phil 2015/09/07
	void setQuery(AosJqlStmtQuery *query) { mQuery = query; }
	AosJqlStmtQuery *getQuery() { return mQuery; }

	virtual void setSubQuery();

	// jimodb-889
	//OmnString getTableType(bool &isStat); 
	void IsStatTable(bool &isStat);
};

#endif
