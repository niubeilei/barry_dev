////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_JQLStatement_JqlSelectField_H
#define AOS_JQLStatement_JqlSelectField_H

#include "JQLStatement/JqlStatement.h"
#include "JQLExpr/ExprGenFunc.h"
#include "JQLExpr/ExprFieldName.h"
#include "SEInterfaces/ExprObj.h"      
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlSelect;

class AosJqlSelectField : public AosJqlDataStruct 
{
public:
	OmnString 		mOName;
	OmnString		mCName;
	OmnString		mTableName;
	OmnString		mDbName;
	bool            mIsInited;
	AosExprObjPtr 	mExpr;
	AosExprObjPtr 	mAlias;
	OmnString 		mMeasureName;

public:
	AosJqlSelectField();
	~AosJqlSelectField();

	OmnString generateFieldConf(const AosRundataPtr &rdata);

	bool isName();
	bool checkIsFunc();
	bool isMemberField();
	bool checkIsAgrFunc();
	bool isConditionMeasure();
	bool isMultiFieldExpr();
	OmnString getFuncName();
	OmnString getMeasureName();
	bool getFields(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields);

	bool getFieldsByStat(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields);

	OmnString getTableName();
	OmnString getAgrFuncTypeStr();
	OmnString getFieldName(const AosRundataPtr &rdata);
	vector<OmnString> getFieldNames(const AosRundataPtr &rdata);

	OmnString getFieldEscapeOName(const AosRundataPtr &rdata);
	OmnString getFieldEscapeCName(const AosRundataPtr &rdata);

	OmnString dump1(){return mExpr->dump();}
	OmnString dumpByNoEscape(){return mExpr->dumpByNoEscape();}
	OmnString dumpByStat(){return mExpr->dumpByStat();}
	OmnString getStatFieldName(){return mExpr->getStatFieldName();}

	void setField(AosExprObjPtr expr);
	void setAliasName(const AosExprObjPtr &alias_name);

	OmnString getXpathName()
	{
		aos_assert_r(mExpr, "");
		if (mExpr->getType() == AosExprType::eFieldName)
		{
			AosExprFieldNamePtr expr;
			expr= dynamic_cast<AosExprFieldName*>(mExpr.getPtr());
			if (expr)
			{
				return expr->getXpathName();
			}
		}
		return getFieldName(0);
	}

	AosExprObjPtr getField();
	OmnString getAliasName(const AosRundataPtr &rdata);
	AosExprObjPtr getAliasName();

	AosJqlSelectFieldPtr clone() const ;
};

#endif
