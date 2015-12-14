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
// 2014/04/08 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprNameValue_h
#define Aos_JQLExpr_ExprNameValue_h

#include "JQLExpr/Expr.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"

class AosExprNameValue : virtual public AosExpr
{
protected:
	OmnString 		mName;
	AosExprObjPtr  	mValue;

public:
	AosExprNameValue() {};
	AosExprNameValue(const OmnString &name, AosExprObjPtr value);
	virtual ~AosExprNameValue() {};

	virtual AosJimoPtr cloneJimo() const;

	// AosExprNameValueObj interface
	virtual void setNameValue(const OmnString &name, const AosExprObjPtr &value);

	virtual bool getValue(
				AosRundata *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value);
	virtual AosExprObjPtr getValue(){return mValue;}
	virtual AosExprObjPtr cloneExpr() const;
	virtual bool isUnary() const {return false;}
	virtual AosExprType::E getType() const {return AosExprType::eNameValue;}
	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;
	virtual OmnString dumpByNoQuote()const;

	virtual OmnString getName() const {return mName;}
	virtual AosExprObjPtr getValueAsExpr() const {return mValue;}
	virtual vector<AosExprObjPtr> getExprList();
};
#endif

