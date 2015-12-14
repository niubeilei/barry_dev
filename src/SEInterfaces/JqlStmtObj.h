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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JqlStmtObj_h
#define Aos_SEInterfaces_JqlStmtObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JqlStmtType.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosJqlStmtObj : public AosJimo
{
protected:
	AosJqlStmtType::E	mStmtType;

	static AosJqlStmtObjPtr		smCreator;

public:
	AosJqlStmtObj(const AosJqlStmtType::E type, const int version);
	~AosJqlStmtObj();

	virtual AosJqlStmtObjPtr cloneStatement() const = 0;
	virtual void dump() = 0;
	virtual bool run(AosRundata *rdata) = 0;
	virtual bool run(AosRundata *rdata, AosDataRecordObj *record) = 0;

	static void setCreator(const AosJqlStmtObjPtr &creator)
	{
		smCreator = creator;
	}

	static AosJqlStmtObjPtr createStatementStatic(
					AosRundata *rdata, 
					const OmnString &statement_name);

	static AosJqlStmtObjPtr createStatementStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &worker_doc);
};
#endif
