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
// 2014/05/13 Created by Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlSelectField.h"

#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlSelect.h"
#include "JQLExpr/ExprMemberOpt.h"
#include "JQLExpr/ExprArith.h"      
#include "SEInterfaces/AggregationType.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"

AosJqlSelectField::AosJqlSelectField()
:
mIsInited(false)
{
}

AosJqlSelectField::~AosJqlSelectField()
{
}


bool
AosJqlSelectField::checkIsAgrFunc()
{
	aos_assert_r(mExpr, false);

	OmnString func_name;
	AosExprGenFuncPtr expr_func;
	if (mExpr->getType() != AosExprType::eGenFunc) return false;
	expr_func = dynamic_cast<AosExprGenFunc*>(mExpr.getPtr());
	aos_assert_r(expr_func, false);

	func_name = expr_func->getFuctName();
	AosAggregationType::E type = AosAggregationType::toEnum(func_name.toLower());
	return AosAggregationType::isValid(type);
}



OmnString
AosJqlSelectField::getAgrFuncTypeStr()
{	
	aos_assert_r(mExpr, "");
	if (mExpr->getType() == AosExprType::eGenFunc)
	{
		AosExprGenFuncPtr expr_func;
		expr_func = dynamic_cast<AosExprGenFunc*>(mExpr.getPtr());
		if (expr_func)
		{
			return expr_func->getFuctName();
		}
	}
	return "";
}

OmnString
AosJqlSelectField::generateFieldConf(const AosRundataPtr &rdata)
{
	OmnString conf = "";
	if (mCName == "") mCName = mOName;
	conf << "<fname>";

	if (mOName.toLower() == "zky_docid")
	{
		conf << "<oname>getMetaData('docid')</oname>";
	}
	else
	{
		//JIMODB-983,JIMODB-1011 Bryant 2015/10/22
		if(mExpr->getType() == AosExprType::eArith)
		{
			OmnString left,right,opr;
			AosExprObjPtr lefExpr = mExpr->getLHS();
			AosExprObjPtr rigExpr = mExpr->getRHS();
			left = lefExpr->dumpByStat();
			right = rigExpr->dumpByStat();
			//JIMODB-1022 Bryant 2015/10/23 
			if(AosExpr::checkIsAgrFunc(lefExpr))
			{
					AosJqlUtil::escape(left);
			}

			if(AosExpr::checkIsAgrFunc(rigExpr))
			{
					AosJqlUtil::escape(right);
			}
			AosExprArith * arith = dynamic_cast<AosExprArith*>(mExpr.getPtr());
			opr = arith->getEnumStr();
			conf << "<oname><![CDATA["<< left << opr << right <<"]]></oname>";

		}
		else
		{
			OmnString oname= dumpByStat();
			if(AosExpr::checkIsAgrFunc(mExpr))
			{
				OmnString func_name = mExpr->getFuctName().toLower();
				oname.replace(0,func_name.length(),func_name);
			}
//			AosJqlUtil::escape(oname);
			conf << "<oname><![CDATA[" << oname	<< "]]></oname>";
		}
	}
	OmnString cname = mCName;
	AosJqlUtil::escape(cname);
	conf << "<cname>" << cname << "</cname></fname>";
	return conf; 
}

bool 
AosJqlSelectField::isConditionMeasure()
{
	if(mExpr->getType() == AosExprType::eArith)
	{
		AosExprObjPtr expr = mExpr->getLHS();
		OmnString func_name;
		AosExprGenFuncPtr expr_func;
		expr_func = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
		aos_assert_r(expr_func, false);

		func_name = expr_func->getFuctName();
		AosAggregationType::E type = AosAggregationType::toEnum(func_name.toLower());
		if(AosAggregationType::isValid(type))
		{
			vector<AosExprObjPtr> exprList = *(expr->getParmList());
			if(exprList.size() != 2) return false;
			AosExprType::E type1 = exprList[0]->getType();
			AosExprType::E type2 = exprList[1]->getType();
			if(type1 == AosExprType::eFieldName && type2 != AosExprType::eFieldName)
				return true;
		}
	}
	if(checkIsAgrFunc())
	{
		vector<AosExprObjPtr> exprList = *(mExpr->getParmList());
		if(exprList.size() != 2) return false;
		AosExprType::E type1 = exprList[0]->getType(); 
		AosExprType::E type2 = exprList[1]->getType(); 
		if(type1 == AosExprType::eFieldName && type2 != AosExprType::eFieldName)
			return true;
	}
	return false;
}

bool
AosJqlSelectField::isMultiFieldExpr()
{
	if (isName() || mExpr->isConstant() || checkIsAgrFunc())
		return false;
	return true; 
}


bool
AosJqlSelectField::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	return mExpr->getFields(rdata, fields);
}


// for stat
bool
AosJqlSelectField::getFieldsByStat(
			const AosRundataPtr &rdata,
			vector<AosJqlSelectFieldPtr> &fields)
{
	return mExpr->getFieldsByStat(rdata, fields);
}

AosJqlSelectFieldPtr
AosJqlSelectField::clone() const 
{
	aos_assert_r(mExpr, 0)
	AosJqlSelectFieldPtr field = OmnNew AosJqlSelectField();

	field->mOName = mOName;
	field->mCName = mCName;
	field->mTableName = mTableName;
	field->mDbName = mDbName;
	field->mIsInited = mIsInited;
	field->mExpr = mExpr->cloneExpr();
	if (mAlias) field->mAlias = mAlias->cloneExpr();
	return field;
}

OmnString 
AosJqlSelectField::getFuncName()
{
	OmnString func_name;
	AosExprGenFuncPtr expr_func;
	if (mExpr->getType() != AosExprType::eGenFunc) return "";
	expr_func = dynamic_cast<AosExprGenFunc*>(mExpr.getPtr());
	aos_assert_r(expr_func, "");

	func_name = expr_func->getFuctName();
	return func_name;
}

OmnString 
AosJqlSelectField::getMeasureName()
{
	if(mMeasureName != "") 
		return mMeasureName;
	if (isConditionMeasure())
	{
		mMeasureName="";
		vector<AosExprObjPtr> exprList = *(mExpr->getParmList());
		OmnString func_name = getFuncName();
		OmnString field_name = exprList[0]->dumpByNoEscape();
		mMeasureName <<func_name << "0x28" << field_name << "0x29";
	}
	return mMeasureName;
}

bool
AosJqlSelectField::checkIsFunc()
{
	//return (mExpr->getType() == AosExprType::eGenFunc && !checkIsAgrFunc());
	return (mExpr->getType() == AosExprType::eGenFunc);
}

bool 
AosJqlSelectField::isMemberField()
{
	AosExprType::E type = mExpr->getType();
	return AosExprType::eMemberOpt == type;
}


bool
AosJqlSelectField::isName()
{
	AosExprType::E type = mExpr->getType();
	return (AosExprType::eFieldName == type) ;
}


OmnString
AosJqlSelectField::getTableName()
{
	return mTableName;
}

OmnString
AosJqlSelectField::getFieldName(const AosRundataPtr &rdata)
{
//	if (mOName != "") return mOName;

	OmnString name;
	if (checkIsFunc())
	{
		//arvin 2015.09.15
		if(isConditionMeasure())
		{
			vector<AosExprObjPtr> parms = *(mExpr->getParmList());
			name = parms[0]->dumpByNoEscape();
			return name;
		}
		if (checkIsAgrFunc())
		{
			AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(mExpr.getPtr());
			aos_assert_r(func_expr, name);
			AosExprList *parms = func_expr->getParmList();
			aos_assert_r(parms && parms->size() == 1, name);
			name = (*parms)[0]->getValue(rdata.getPtrNoLock());
			return name;
		}
		return mExpr->dumpByNoEscape();
	}
	name = mExpr->getValue(rdata.getPtrNoLock());
	return name;
}


vector<OmnString> 
AosJqlSelectField::getFieldNames(const AosRundataPtr &rdata)
{
	vector<OmnString> names;
	OmnString name;

	aos_assert_r(checkIsFunc(), names);
	AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(mExpr.getPtr());
	aos_assert_r(func_expr, names);
	AosExprList *parms = func_expr->getParmList();
	aos_assert_r(parms && !parms->empty(), names);
	for (u32 i = 0; i < parms->size(); i++)
	{
		AosExprObjPtr expr = (*parms)[i];
		AosExprType::E type = expr->getType();
		if (AosExprType::eFieldName == type)
		{
			name = (*parms)[i]->getValue(rdata.getPtrNoLock());
			names.push_back(name);
		}
	}
	return names;
}

OmnString
AosJqlSelectField::getFieldEscapeOName(const AosRundataPtr &rdata)
{
	if(mExpr->getType() == AosExprType::eArith)
	{
		OmnString left,right,opr;
		AosExprObjPtr lefExpr = mExpr->getLHS();
		AosExprObjPtr rigExpr = mExpr->getRHS();
		left = lefExpr->dumpByStat();
		right = rigExpr->dumpByStat();
		//JIMODB-1022 Bryant 2015/10/23 
		if(AosExpr::checkIsAgrFunc(lefExpr))
		{
			AosJqlUtil::escape(left);
		}

		if(AosExpr::checkIsAgrFunc(rigExpr))
		{
			AosJqlUtil::escape(right);
		}
		AosExprArith * arith = dynamic_cast<AosExprArith*>(mExpr.getPtr());
		opr = arith->getEnumStr();
		return  left << opr << right;

	}

	if (mOName == "") return getFieldName(rdata);
	return mOName;
}

	OmnString 
AosJqlSelectField::getFieldEscapeCName(const AosRundataPtr &rdata)
{
	return mCName;
}

void 
AosJqlSelectField::setField(AosExprObjPtr expr)
{
	aos_assert(expr);
	mExpr = expr;
}

void 
AosJqlSelectField::setAliasName(const AosExprObjPtr &alias_name)
{
	mAlias = alias_name;
}

AosExprObjPtr 
AosJqlSelectField::getField()
{
	return mExpr;
}

OmnString
AosJqlSelectField::getAliasName(const AosRundataPtr &rdata)
{
	if (!mAlias) 
	{
		//if (checkIsAgrFunc()) return getFuncStr();
		return "";
	}
	return mAlias->getValue(rdata.getPtrNoLock());
}

AosExprObjPtr 
AosJqlSelectField::getAliasName()
{
	return mAlias;
}
	

