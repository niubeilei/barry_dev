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
#ifndef AOS_JQLStatement_JqlJoinTable_H
#define AOS_JQLStatement_JqlJoinTable_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelectTable.h"

class AosJqlTableReference;
typedef vector<AosJqlTableReferencePtr> AosTableReferences;
class AosJqlJoinTable : public AosJqlSelectTable
{
public:
	AosTableReferences*         mTableReferencesPtr;

public:
	AosJqlJoinTable();
	~AosJqlJoinTable();
	/*
	virtual AosJqlJoinTable* clone(const AosRundataPtr &rdata)const;
	virtual bool init(const AosRundataPtr &rdata, AosJqlSelectPtr select);
	virtual vector<AosJqlSelectFieldPtr> getTableFieldList(const OmnString table_name, bool &rslt);
	*/

	bool getAllTableDef(const AosRundataPtr &rdata, vector<AosXmlTagPtr> &table_defs);

	AosJqlJoinTablePtr createStatic(
				const AosRundataPtr &rdata,
				AosExprObjPtr table,
				const AosJqlSelectPtr &select);

	virtual void setSubQuery();

};

#endif
