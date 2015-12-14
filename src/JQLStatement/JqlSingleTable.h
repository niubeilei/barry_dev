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
#ifndef AOS_JQLStatement_JqlSingleTable_H
#define AOS_JQLStatement_JqlSingleTable_H

#include "JQLStatement/JqlSelectTable.h"
#include "Util/String.h"

class AosJqlSingleTable : public AosJqlSelectTable
{
public:
	AosExprList		*mIndexHint;

public:
	virtual OmnString generateTableConf(const AosRundataPtr &rdata);
	virtual OmnString generateWhereConf(const AosRundataPtr &rdata);
	virtual OmnString generateIndexHint(const AosRundataPtr &rdata);
	virtual OmnString generateInputDatasetConf(const AosRundataPtr &rdata);
	virtual bool getAllField(const AosRundataPtr &rdata, vector<AosJqlSelectFieldPtr> *&fields);

	virtual AosJqlSingleTable* clone(const AosRundataPtr &rdata) const;

	virtual OmnString generateDefaultIndexConf(const AosRundataPtr &rdata);

	static AosJqlSingleTablePtr createStatic(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		const AosExprObjPtr &table,
		const AosExprObjPtr &alias,
		AosExprList *index_hint);

	virtual void IsStatTable(bool &isStat){}
};

#endif
