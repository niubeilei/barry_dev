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
#ifndef Aos_SqlDriver_SqlParser_h
#define Aos_SqlDriver_SqlParser_h

#include "SEInterfaces/BigSQLParserObj.h"

class AosBigSQLParser : public AosBigSQLParserObj
{
	OmnDefineRCObject;

private:
	bool	mTablenameCaseSensitive;

public:
	AosBigSQLParser();
	~AosBigSQLParser();

	virtual bool parseScripts(
			vector<AosXmlTagPtr> &stmts,
			const OmnString &statement, 
			const AosRundataPtr &rdata);

	virtual bool parseStatement(
			const OmnString &statement, 
			OmnString &stmt,
			const AosRundataPtr &rdata);

	bool isTableNameCaseSensitive() const {return mTablenameCaseSensitive;}
	bool isValidTablename(
			const OmnString &tablename, 
			const AosRundataPtr &rdata) const;
};
#endif

