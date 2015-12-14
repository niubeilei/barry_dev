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
// 2014/04/08: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoGenFunc_h
#define Aos_Jimo_JimoGenFunc_h

#include "Jimo/Jimo.h"
#include "Util/ValueRslt.h"
#include "JQLStatement/JQLCommon.h"


class AosJimoGenFunc : public AosJimo
{
public:
	AosJimoGenFunc(const u32 type, const int version)
	:
	AosJimo(type, version)
	{
	}

	AosJimoGenFunc(const AosJimoGenFunc &rhs)
	:
	AosJimo(rhs)
	{
	}

	// Andy, 2015/08/21
	virtual bool setParms(AosRundata *rdata, AosExprList* parms) = 0;

	// Young, 2015/05/29
	virtual AosDataType::E getDataType(
						AosRundata *rdata,
						AosDataRecordObj *record) = 0;

	// Chen Ding, 2014/09/16
	virtual bool getValue(AosRundata *rdata, 
						AosValueRslt &value, 
						AosDataRecordObj *record) = 0;

	// Chen Ding, 2014/09/16
	virtual bool syntaxCheck(AosRundata *rdata, 
						OmnString &errmsg) = 0;

	// Chen Ding, 2014/05/01
	virtual bool syntaxCheck(const AosRundataPtr &rdata, 
						OmnString &errmsg) = 0;

};
#endif

