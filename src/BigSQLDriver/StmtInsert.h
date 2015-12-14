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
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BigSQLDriver_SqlStmtInsert_h
#define Aos_BigSQLDriver_SqlStmtInsert_h

#include "BigSQLDriver/StmtParser.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosSqlStmtInsert : public AosSqlStmtParser
{
private:
	enum
	{
		eMaxFields = 100000
	};

public:
	AosSqlStmtInsert(const bool flag);
	~AosSqlStmtInsert();

	virtual bool parse(
				const OmnString &statement, 
				OmnStrParser1 &parser,
				OmnString &stmt,
				const AosBigSQLParser *bigsql,
				const AosRundataPtr &rdata);

private:
	bool parseOneValueSet(
				OmnStrParser1 &parser,
				const vector<OmnString> &colnames,
				OmnString &stmt,
				const AosRundataPtr &rdata);
};
#endif

