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
#ifndef Aos_JQLExpr_ExprMemberOpt_h
#define Aos_JQLExpr_ExprMemberOpt_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprMemberOpt : public AosExpr
{
protected:
	OmnString		mMember1;
	OmnString		mMember2;
	OmnString 		mMember3;
	AosExprObjPtr	mExpr;
	int				mRecordIdx;
	int				mFieldIdx;

	OmnString 		mFieldName;

public:
	AosExprMemberOpt() {mRecordIdx = -1;};
	AosExprMemberOpt(
			const char *v1, 
			const char *v2,
			const char *v3);

	virtual ~AosExprMemberOpt() {};

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record);

	virtual bool getValue(
			AosRundata *rdata,
			AosDataRecordObj *record,
			AosValueRslt &value);

	virtual bool getValue(
			AosRundata *rdata,
			vector<AosDataRecordObj *> &records,
			AosValueRslt &value);

	virtual bool getFields(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields);

	virtual AosExprObjPtr cloneExpr() const;
	virtual bool isUnary() const {return true;}
	virtual bool isMemberOpt() const {return true;}
	virtual int getNumFieldTerms() const {return 1;}
	virtual bool transformConstantExprs() {return false;}

	virtual OmnString dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	void setMember1(const char *vv);
	void setMember2(const char *vv);
	void setMember3(const char *vv);

	OmnString getMember1();
	OmnString getMember2();
	OmnString getMember3();

	AosExprType::E getType() const {return AosExprType::eMemberOpt;}

	bool checkSequenceExist(const AosRundataPtr& rdata);
	AosExprObjPtr getSequenceExpr(const AosRundataPtr &rdata);
	OmnString getSequenceName(const AosRundataPtr &rdata);

};
#endif

