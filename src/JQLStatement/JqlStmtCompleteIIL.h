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
#ifndef AOS_JQLStatement_JqlStmtCompleteIIL_H
#define AOS_JQLStatement_JqlStmtCompleteIIL_H

#include "JQLStatement/JqlStatement.h"

class AosJqlStmtCompleteIIL : public AosJqlStatement
{
private:
	OmnString			mIILName;

public:
	AosJqlStmtCompleteIIL();
	~AosJqlStmtCompleteIIL();
	
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void 	setIILName(const OmnString &iilname);
};

#endif
