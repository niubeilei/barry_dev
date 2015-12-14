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
// 2013/09/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprUserVar_h
#define Aos_JQLExpr_ExprUserVar_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "JQLStatement/JQLCommon.h"

class AosExprUserVar : public AosExpr
{
private:
	OmnString 					mName;
	AosValueRslt				mValue;
	AosJQLDataFieldTypeInfoPtr 	mFieldTypeInfo;

public:
	AosExprUserVar();
	AosExprUserVar(const AosExprUserVar &rhs);
	virtual ~AosExprUserVar();

	virtual AosDataType::E getDataType(
								AosRundata *rdata,
								AosDataRecordObj *record);

	virtual bool getValue(
			AosRundata *rdata,
			AosDataRecordObj *record,
			AosValueRslt &value);

	virtual OmnString 	dump() const;
	virtual OmnString 	dumpByNoEscape() const ;
	virtual OmnString 	dumpByStat() const ;
	virtual AosExprObjPtr cloneExpr() const;                            
	virtual AosExprType::E	getType() const {return AosExprType::eUserVar;}

	void setType(const AosJQLDataFieldTypeInfoPtr &type);
	void setName(const OmnString &name);
	void setValue(const AosValueRslt &value); 
	AosJQLDataFieldTypeInfoPtr  getFieldTypeInfo(){return mFieldTypeInfo;}; 

	bool getValue(
		AosDataRecordObj *record,
		AosValueRslt &value,
		AosRundata *rdata);

};
#endif

