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
#ifndef AOS_JQLStatement_JqlStmtDatabase_H
#define AOS_JQLStatement_JqlStmtDatabase_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDatabase : public AosJqlStatement
{
private:
	AosXmlTagPtr 					mDoc;
	AosRundataPtr					mRundata;
	OmnString						mName;

public:
	OmnString						mErrmsg;

public:
	AosJqlStmtDatabase(const OmnString &errmsg);
	AosJqlStmtDatabase();
	~AosJqlStmtDatabase();

	//getter/setters
	void setName(OmnString name);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	bool createDatabase(const AosRundataPtr &rdata);
	bool showDatabases(const AosRundataPtr &rdata);
	bool dropDatabase(const AosRundataPtr &rdata);
	bool useDatabase(const AosRundataPtr &rdata);

	static bool checkDatabase(
			const AosRundataPtr &rdata,
			const OmnString &database_name);
};

#endif
