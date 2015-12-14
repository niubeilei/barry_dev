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
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_CondExpr_h
#define Aos_SdocAction_CondExpr_h

#include "Conds/Condition.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"


class AosCondExpr : virtual public AosCondition
{
	OmnString  			mExprStr;
	AosExprObjPtr		mExpr;

public:
	AosCondExpr(const bool flag);
	AosCondExpr(const OmnString &expr_str, const AosRundataPtr &rdata);
	~AosCondExpr();

	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata) {return false;}
	virtual bool evalCond(const AosRundataPtr &rdata){return false;}
	virtual bool evalCond(const char *record, const int len, const AosRundataPtr &rdata) {return false;}
	virtual bool evalCond(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata);         
	virtual bool evalCond(const AosValueRslt &value, const AosRundataPtr &rdata);
	virtual bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual AosConditionObjPtr clone();

};
#endif

