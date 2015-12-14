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
#ifndef Aos_BigSQLDriver_SqlStmtParser_h
#define Aos_BigSQLDriver_SqlStmtParser_h

#include "BigSQLDriver/Ptrs.h"
#include "BigSQLDriver/StmtTypes.h"
#include "Util/StrParser.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosSqlStmtParser : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosSqlStmtType::E		mType;

public:
	AosSqlStmtParser(
			const OmnString &name, 
			const AosSqlStmtType::E type, 
			const bool regflag);
	~AosSqlStmtParser();

	virtual bool parse(
				const OmnString &statement, 
				OmnStrParser1 &parser,
				OmnString &stmt,
				const AosBigSQLParser *bigsql,
				const AosRundataPtr &rdata) = 0;

	static AosSqlStmtParserPtr getParser(
			const OmnString &name, 
			const AosRundataPtr &rdata);

	AosSqlStmtType::E getType() const {return mType;}

private:
	bool registerStmt(
			const AosSqlStmtParserPtr &parser, 
			const OmnString &name);
};
#endif

