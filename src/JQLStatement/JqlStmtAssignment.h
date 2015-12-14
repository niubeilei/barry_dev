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
#ifndef AOS_JQLStatement_JqlStmtAssignment_H
#define AOS_JQLStatement_JqlStmtAssignment_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/ExprObj.h"


class AosJqlStmtAssignment : public AosJqlStatement
{
private:
	bool							mIsParms;
	int								mParmsIdx;
	OmnString 						mName;
	AosExprObjPtr					mValue;
	AosJQLDataFieldTypeInfoPtr  	mFieldTypeInfo;


public:
	AosJqlStmtAssignment();
	AosJqlStmtAssignment(const AosJqlStmtAssignment &rhs);
	~AosJqlStmtAssignment();

	// interfaces;
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setName(const OmnString &name);
	void setValue(const AosExprObjPtr &value);
	void setType(const AosJQLDataFieldTypeInfoPtr &type);
	void setIsParms(const bool &flag);
	void setParms(const int &index);

	AosExprObjPtr getUserVar(const AosRundataPtr &rdata);
};

#endif
