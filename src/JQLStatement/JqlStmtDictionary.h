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
#ifndef AOS_JQLStatement_JqlStmtDictionary_H
#define AOS_JQLStatement_JqlStmtDictionary_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDictionary : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mDictName;
	OmnString		mTableName;
	AosExprList		*mKeys;
	AosExprObj		*mWhereCond;

public:
	OmnString		mErrmsg;

public:
	AosJqlStmtDictionary(const OmnString &errmsg);
	AosJqlStmtDictionary();
	~AosJqlStmtDictionary();

	//getter/setters
	void setDictName(OmnString name);
	void setTableName(OmnString name);
	void setKeys(AosExprList *key_list);
	void setWhereCond(AosExprObj *where_cond);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

private:
	bool createDict(const AosRundataPtr &rdata);  
	bool showDicts(const AosRundataPtr &rdata);   
	bool describeDict(const AosRundataPtr &rdata);
	bool dropDict(const AosRundataPtr &rdata);   
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
};

#endif
