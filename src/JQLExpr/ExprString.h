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
// 2013/09/28 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprString_h
#define Aos_JQLExpr_ExprString_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprString : public AosExpr
{
protected:
	OmnString	 mValue;

public:
	AosExprString();
	AosExprString(const char *value);
	virtual ~AosExprString();

	virtual AosJimoPtr cloneJimo() const;

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eString; }

	virtual bool getValue(
				AosRundata *rdata,
				vector<AosDataRecordObj *> &records,
				AosValueRslt &value);
	virtual bool getValue(
				AosRundata *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value);

	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;
	virtual OmnString dumpByNoQuote()const ;
	
	virtual AosExprObjPtr cloneExpr() const;                            
	virtual bool isUnary() const {return true;} 
	virtual bool isConstant() const { return true;}
	AosExprType::E getType() const {return AosExprType::eString;}

};
#endif

