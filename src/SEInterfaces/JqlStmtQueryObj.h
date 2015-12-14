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
#ifndef Aos_SEInterfaces_JqlStmtQueryObj_h
#define Aos_SEInterfaces_JqlStmtQueryObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JqlStmtObj.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosJqlStmtQueryObj : public AosJqlStmtObj
{
public:
	AosJqlStmtQueryObj(const OmnString &version);
	~AosJqlStmtQueryObj();

	virtual bool runQuery(
					AosRundata *rdata, 
					AosBuffPtr &results) = 0;

	virtual AosMySqlRecordObjPtr getResultRecord(AosRundata *rdata) = 0;
};
#endif
