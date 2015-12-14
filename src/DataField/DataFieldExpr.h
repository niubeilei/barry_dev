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
// 2014/05/22 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataField_DataFieldExpr_h
#define Aos_DataField_DataFieldExpr_h

#include "DataField/DataField.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/DataTypes.h"


class AosDataFieldExpr : virtual public AosDataField
{
	OmnDefineRCObject;

	OmnString		mExprStr;
	AosExprObjPtr	mExpr;
	AosDataType::E	mExprDataType;

public:
	AosDataFieldExpr(const bool reg);
	AosDataFieldExpr(const AosDataFieldExpr &rhs);
	~AosDataFieldExpr();
	
	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) const
	{
		return mExpr->getDataType(rdata, record);
	}

	virtual AosDataFieldObjPtr clone(AosRundata *rdata) const;

	virtual bool	getValueFromRecord(
						AosDataRecordObj* record,
						const char * data,
						const int len,
						int &idx,
						AosValueRslt &value, 
						const bool copy_flag,
						AosRundata* rdata);

	virtual AosDataFieldObjPtr create(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata) const;

private:
	bool			config(
						const AosXmlTagPtr &def,
						AosDataRecordObj *record,
						AosRundata *rdata);

	OmnString 		getValueExpr(
				        const OmnString &str,
					    AosRundata *rdata);

	bool			str2Bool(
				        const OmnString &str,
					    AosRundata *rdata);
};

#endif

