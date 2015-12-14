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
// 2014/10/22 Created by Bryant Zhou
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtActor_H
#define AOS_JQLStatement_JqlStmtActor_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "JQLStatement/JQLCommon.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlStmtActor : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString mName;
	OmnString mClassName;
	OmnString mPath;
	OmnString mType_language;
	OmnString mDescription;
	AosExprList* par_key_names; 
	AosExprList* run_vals_list;
    AosExprList* mPNames;
    AosExprList* mPValues;

public:
	AosJqlStmtActor();
	~AosJqlStmtActor();
	
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();
	void setName(const OmnString &name);
	void setClassName(const OmnString &className);
	void setPath(const OmnString &path);
	void setType_language(const OmnString &type_language);
	void setDescription(const OmnString &description);
	void setKeys(AosExprList* par_key_names);
	void setValues(AosExprList* run_vals_list);
	bool runActor(const AosRundataPtr &rdata);
	bool createActor(const AosRundataPtr &rdata);
};

#endif
