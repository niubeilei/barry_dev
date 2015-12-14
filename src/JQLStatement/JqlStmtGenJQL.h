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
// 01/17/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtGenJQL_H
#define AOS_JQLStatement_JqlStmtGenJQL_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"
#include "SQLDatabase/SqlTable.h"

class AosJqlStmtGenJQL : public AosJqlStatement
{
private:
	string 			mVerbName;
	string			mObjectType;
	string			mObjectName;
	string			mConfig;	// json format

public:
	AosJqlStmtGenJQL();
	~AosJqlStmtGenJQL();

	//member functions
	//virtual bool run(const AosRundataPtr run(const AosRundataPtr &rdata)rdata, const AosJimoProgObjPtr run(const AosRundataPtr &rdata)prog);
	virtual bool run(
					const AosRundataPtr &rdata, 
					const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setVerbName(string verb) { mVerbName = verb; }
	void setObjectType(string type) { mObjectType = type; }
	void setObjectName(string name) { mObjectName = name; }
	void setJQLConfig(string conf) { mConfig = conf; }
};

#endif
