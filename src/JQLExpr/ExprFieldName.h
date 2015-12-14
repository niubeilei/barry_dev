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
#ifndef Aos_JQLExpr_ExprFieldName_h
#define Aos_JQLExpr_ExprFieldName_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprFieldName : public AosExpr
{
	OmnString 				mFieldName;
	OmnString 				mXpathName;
	int 					mFieldIdx;

	bool					mIsParsedFieldIdx;
	bool					mIsMultiRecord;
	map<OmnString, int>		mFieldIdxMap;

public:
	AosExprFieldName();
	AosExprFieldName(const char *data);
	AosExprFieldName(const char *data, const char *xpath);
	virtual ~AosExprFieldName() {};

	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata *rdata,
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getFields(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool isFieldName() const {return true;}
	virtual bool isUnary() const {return true;}
	virtual int getNumFieldTerms() const {return 1;}
	virtual AosExprObjPtr cloneExpr() const;
	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const;
	virtual OmnString dumpByStat() const;
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return mIsParsedFieldIdx = prasedFieldIdx;}

	/*
	//yang
	virtual u32 getFieldIdx() const {return mFieldIdx; }
*/

	AosExprType::E getType() const {return AosExprType::eFieldName;}

	OmnString getXpathName()
	{
		if (mXpathName != "")
			return mXpathName;
		return mFieldName;
	}
	void setXpathName(const char *data){mXpathName = OmnString(data);}

private:
	bool	parseFieldIdx(
				AosRundata *rdata,
				AosDataRecordObj *record);

};
#endif

