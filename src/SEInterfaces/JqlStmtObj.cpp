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
// 2013/12/31 Created by Phill
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/JqlStmtObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"

AosJqlStmtObjPtr AosJqlStmtObj::smCreator;


AosJqlStmtObj::AosJqlStmtObj(
		const AosJqlStmtType::E type, 
		const int version)
:
AosJimo(AosJimoType::eJQLStatement, version),
mStmtType(type)
{
}


AosJqlStmtObj::~AosJqlStmtObj()
{
}


AosJqlStmtObjPtr
AosJqlStmtObj::createStatementStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc);
	if (!jimo) return 0;

	if (jimo->getJimoType() != AosJimoType::eJQLStatement)
	{
		AosSetErrorUser(rdata, "value_invalid_jimo_type") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosJqlStmtObjPtr stmt = dynamic_cast<AosJqlStmtObj*>(jimo.getPtr());
	if (!stmt)
	{
		AosSetErrorUser(rdata, "value_internal_error") << enderr;
		return 0;
	}

	return stmt;
}


AosJqlStmtObjPtr
AosJqlStmtObj::createStatementStatic(
		AosRundata *rdata, 
		const OmnString &name)
{
	AosJimoPtr jimo;//AosCreateJimoByClassname(rdata, name);
	if (!jimo) return 0;

	if (jimo->getJimoType() != AosJimoType::eJQLStatement)
	{
		AosSetErrorUser(rdata, "value_invalid_jimo_type") 
			<< jimo->getJimoType() << enderr;
		return 0;
	}

	AosJqlStmtObjPtr stmt = dynamic_cast<AosJqlStmtObj*>(jimo.getPtr());
	if (!stmt)
	{
		AosSetErrorUser(rdata, "value_internal_error") << enderr;
		return 0;
	}

	return stmt;
}

