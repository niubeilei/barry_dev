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
#ifndef Aos_JQLExpr_ExprContain_h
#define Aos_JQLExpr_ExprContain_h

#include "JQLExpr/Expr.h"

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprContain : public AosExpr
{
protected:
	AosExprObjPtr  	mFieldPtr;
	AosExprObj		*mField;

	OmnString	mValue;

public:
	AosExprContain();
	AosExprContain(AosExprObjPtr field, char *value);
	virtual ~AosExprContain();

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eBool; }

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getValue(
					AosRundata *rdata,
					const AosValueRslt &key, 
					AosValueRslt &value);

	virtual bool createConds(
					const AosRundataPtr& rdata,
					vector<AosJqlQueryWhereCondPtr> &conds,
					const OmnString &tableName);

	virtual bool getFields(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual AosExprObjPtr cloneExpr() const;
	
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}

	bool check(OmnString data) const;
	AosExprType::E getType() const {return AosExprType::eContain;}

};
#endif

