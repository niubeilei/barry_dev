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
#ifndef AOS_JQLStatement_JqlStmtDeleteItem_H
#define AOS_JQLStatement_JqlStmtDeleteItem_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "JQLStatement/JQLCommon.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlStmtDeleteItem : public AosJqlStatement
{
private:
	AosJqlStmtDelete*				mPDelete;				
	AosExprList*					mPExprList;
	AosExprObj*						mPTable;
	AosExprObj*						mOptWhere;
	map<OmnString, OmnString>		mValues;

public:
	OmnString						mErrmsg;

public:
	AosJqlStmtDeleteItem(AosJqlStmtDelete *stmt);
	AosJqlStmtDeleteItem(const OmnString &errmsg);
	AosJqlStmtDeleteItem(const AosJqlStmtDeleteItem &rhs);
	~AosJqlStmtDeleteItem();

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

};

#endif
