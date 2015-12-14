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
// 2015/12/04 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtJoinSyncher_H
#define AOS_JQLStatement_JqlStmtJoinSyncher_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"
#include "Rundata/Ptrs.h"

class AosJqlStmtJoinSyncher : public AosJqlStatement
{
private:
	OmnString 					mJoinType;
	OmnString					mCond;
	vector<OmnString>			mJoinTables;
	vector<OmnString>			mJoinIndexs;

public:
	OmnString		mErrmsg;

public:
	AosJqlStmtJoinSyncher(const OmnString &errmsg);
	AosJqlStmtJoinSyncher();
	~AosJqlStmtJoinSyncher();

	//getter/setters
	void setJoinType(OmnString name);
	void setJoinTables(AosExprList *key_list);
	void setJoinIndexs(AosExprList *key_list);
	void setCondition(AosExprObj *value);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump() {}

public:
	bool createSyncher(const AosRundataPtr &rdata);  
};

#endif
