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
#ifndef AOS_JQLStatement_JqlStmtDebug_H
#define AOS_JQLStatement_JqlStmtDebug_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"
#include "SQLDatabase/SqlTable.h"

class AosJqlStmtDebug : public AosJqlStatement
{
private:
	AosXmlTagPtr 				mDoc;
	OmnString 					mOption;

public:
	AosJqlStmtDebug();
	~AosJqlStmtDebug();

	//getter/setters
	void setSwitch(OmnString opt);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();
};

#endif
