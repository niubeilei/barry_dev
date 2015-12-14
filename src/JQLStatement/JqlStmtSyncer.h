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
#ifndef AOS_JQLStatement_JqlStmtSyncer_H
#define AOS_JQLStatement_JqlStmtSyncer_H

#include "JQLStatement/JqlStatement.h"
#include "JQLExpr/ExprMemberOpt.h"

#include "SEInterfaces/ExprObj.h"
#include "Util/String.h"

class AosJqlStmtSyncer : public AosJqlStatement
{
private:
	OmnString			mName;
	vector<AosExprObjPtr> *mParms;

public:
	AosJqlStmtSyncer();
	~AosJqlStmtSyncer();

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

public:
	void setName(const AosExprObjPtr &name);
	void setParms(vector<AosExprObjPtr> *&parms) {mParms = parms;}
	OmnString getName() const {return mName;}

private:

	bool createSyncer(const AosRundataPtr &rdata);
	bool dropSyncer(const AosRundataPtr &rdata);

};

#endif
