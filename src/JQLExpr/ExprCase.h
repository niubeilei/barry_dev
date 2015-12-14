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
#ifndef Aos_JQLExpr_ExprCase_h
#define Aos_JQLExpr_ExprCase_h

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprSearchCase.h"
#include "JQLExpr/ExprSimpleCase.h"

#include "Util/ValueRslt.h"

class AosExprCase : public AosExpr
{
protected:
	AosExprObjPtr	mSearchPtr;
	AosExprObj		*mSearch;

	AosExprObjPtr	mSimplePtr;
	AosExprObj		*mSimple;

	AosExprObjPtr	mElsePtr;
	AosExprObj		*mElse;

public:
	AosExprCase(const AosExprObjPtr &search, 
			const AosExprObjPtr &simple,
			const AosExprObjPtr &expr);

	AosExprCase(const AosExprCase &rhs);
	virtual ~AosExprCase();

	virtual OmnString 	dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual int	 		getNumFieldTerms() const;  

	virtual AosDataType::E getDataType(
					AosRundata *rdata,
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getFieldName(
					const AosRundataPtr &rdata,
					vector<AosExprObjPtr> &fields) {return false;};

	virtual bool getFields(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool getFieldsByStat(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool createConds(
					const AosRundataPtr &rdata, 
					vector<AosJqlQueryWhereCondPtr>  &conds,
					const OmnString &tableName);

	virtual AosExprObjPtr 	cloneExpr() const;
	virtual AosExprType::E	getType() const {return AosExprType::eCase;}
	
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}

};
#endif

