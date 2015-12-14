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
#ifndef Aos_JQLExpr_ExprIsNotNull_h
#define Aos_JQLExpr_ExprIsNotNull_h

#include "JQLExpr/Expr.h"
#include "Util/ValueRslt.h"

class AosExprIsNotNull : public AosExpr
{
protected:
	AosExprObjPtr	mFieldPtr;
	AosExprObj		*mField;

public:
	AosExprIsNotNull(const AosExprObjPtr &field);

	AosExprIsNotNull(const AosExprIsNotNull &rhs);
	virtual ~AosExprIsNotNull();

	virtual AosDataType::E getDataType(AosRundata* rdata, AosDataRecordObj *record) { return AosDataType::eBool; }

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getValue(
					AosRundata *rdata,
					const AosValueRslt &key,
					AosValueRslt &value);

	virtual int 		getNumFieldTerms() const;  
	virtual OmnString 	dump() const ;
	virtual OmnString 	dumpByNoEscape() const ;
	virtual OmnString 	dumpByStat() const ;

	virtual bool   		getFieldName(
					        const AosRundataPtr &rdata,
							vector<AosExprObjPtr> &fields) {return false;};

	virtual bool		getFields(
					        const AosRundataPtr &rdata,
							vector<AosJqlSelectFieldPtr> &fields);

	virtual bool 		createConds(
							const AosRundataPtr &rdata, 
							vector<AosJqlQueryWhereCondPtr>  &conds,
							const OmnString &tableName);

	virtual AosExprType::E	getType() const {return AosExprType::eIsNotNull;}

	virtual AosExprObjPtr cloneExpr() const;

private:
	AosDataType::E getDataType(const AosRundataPtr &rdata,const OmnString &tableName);
};
#endif

