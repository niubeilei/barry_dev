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
#ifndef AOS_JQLStatement_JqlStmtQueryUnion_H
#define AOS_JQLStatement_JqlStmtQueryUnion_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtQuery.h"
#include "JQLStatement/JqlSelect.h"

#include "Util/String.h"

#include <vector>
#include <map>


class AosJqlStmtQueryUnion : public AosJqlStmtQuery
{
private:
	vector<AosJqlStmtQueryPtr>	mQueryList;
	bool						mIsUnionAll;

public:
	AosJqlStmtQueryUnion();
	~AosJqlStmtQueryUnion();

	void setIsUnionAll(bool v){mIsUnionAll = v;}
	bool addQuery(AosJqlStmtQuery *query);

	//Generate the query result dataset
	virtual bool runQuery(
			const AosRundataPtr &rdata,
			AosDatasetObjPtr &ds);

};
#endif
