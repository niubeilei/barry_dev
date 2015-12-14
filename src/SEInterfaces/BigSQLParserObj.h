////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 12/25/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BigSQLParserObj_h
#define Aos_SEInterfaces_BigSQLParserObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBigSQLParserObj : virtual public OmnRCObject
{
private:
	static AosBigSQLParserObjPtr		smBigSQLParser;

public:
	static void setBigSQLParser(const AosBigSQLParserObjPtr &d) {smBigSQLParser = d;}
	static AosBigSQLParserObjPtr getBigSQLParser() {return smBigSQLParser;}

	virtual AosDocFileMgrObjPtr getDocFileMgr(
					const u32 cube_id, 
					const AosRundataPtr &rdata) = 0;

	virtual bool parseScripts(
			vector<AosXmlTagPtr> &stmts,
			const OmnString &statement, 
			const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr parseStatement(
			const OmnString &statement, 
			const AosRundataPtr &rdata) = 0;
};

#endif
