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
// 2013/09/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprBinary_h
#define Aos_JQLExpr_ExprBinary_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"

#include <vector>

class AosExprBinary : public AosExpr
{
protected:
	AosExprObjPtr			mLHSPtr;
	AosExprObj*				mLHS;

	AosExprObjPtr			mRHSPtr;
	AosExprObj*				mRHS;

public:
	AosExprBinary();
	AosExprBinary(AosExprObjPtr lhs, AosExprObjPtr rhs);
	virtual ~AosExprBinary() {};

	virtual int getNumFieldTerms() const;
	virtual AosExprObjPtr getRHS() const {return mRHS->cloneExpr();};
	virtual AosExprObjPtr getLHS() const {return mLHS->cloneExpr();};
	virtual AosExprType::E	getType() const {return AosExprType::eBinary;}
	virtual bool fieldExprWithAggr() const ;
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;};

	virtual bool getValue(AosRundata *rdata,
					vector<AosDataRecordObj *> records,
					const AosValueRslt &key);

	virtual bool getFields(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields);

	virtual bool getFieldsByStat(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields);

	virtual bool 		isAlwaysFalse();
	virtual bool 		isExprCond()const;
	virtual bool 		isJoin()const;

};
#endif

