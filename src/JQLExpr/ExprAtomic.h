////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprAtomic_h
#define Aos_JQLExpr_ExprAtomic_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"

class AosExprAtomic : public AosExpr
{
private:
	OmnString 	mValue;

public:
	AosExprAtomic(){}
	AosExprAtomic(const char *data);
	virtual ~AosExprAtomic() {};

	virtual bool getValue(
				AosRundata       *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value) {return false;};

	virtual AosExprType::E	getType() const {return AosExprType::eAtomic;}

};
#endif

