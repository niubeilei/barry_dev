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
#ifndef AOS_JQLStatement_JqlTableReference_H
#define AOS_JQLStatement_JqlTableReference_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlJoinTable.h"
#include "JQLStatement/JqlSelect.h"

#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"

#include <vector>
using namespace std;

class AosJqlTableFactor;

class AosJqlTableReference : public AosJqlDataStruct
{
public:
	AosJqlTableFactorPtr	mTableFactor;
	AosJqlJoinTablePtr		mJoinTable;

public:
	AosJqlTableReference();
	~AosJqlTableReference();

	bool init(
			const AosRundataPtr &rdata,
			const AosJqlSelectPtr &select);

	bool getAllField(
			const AosRundataPtr &rdata, 
			const AosJqlSelectPtr &select, 
			vector<AosJqlSelectFieldPtr> *&fields);

	bool getAllTableDef(
			const AosRundataPtr &rdata, 
			vector<AosXmlTagPtr> &table_defs);

	OmnString generateTableConf(const AosRundataPtr &rdata);
	vector<AosJqlSelectFieldPtr> getTableFieldList( const OmnString &table_name, bool &rslt);

	AosJqlTableReference* clone(const AosRundataPtr &rdata) const;

	void setTableFactor(AosJqlTableFactor *table_factor);
	void setJqlJoinTable(AosJqlJoinTable *join_table);
	AosJqlTableFactorPtr getTableFactor();
	AosJqlJoinTablePtr getJoinTable();

	virtual void setSubQuery();
};

#endif
