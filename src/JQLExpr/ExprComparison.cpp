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
// 2013/09/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprComparison.h"
#include "JQLExpr/ExprMemberOpt.h"
#include "SEInterfaces/QueryType.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Util/Opr.h"
#include "SQLDatabase/SqlDB.h"
#include "JQLStatement/JqlStmtTable.h"
//#include "AccessServer/JqlSessionMgr.h"
#include "SEUtil/IILName.h"

AosExprComparison::AosExprComparison(
		AosExprObjPtr lhs, 
		Operator opr,
		AosExprObjPtr rhs)
:
AosExprBinary(lhs, rhs),
mOpr(opr)
{
	if (!isValidOpr(opr))
	{
		OmnString errmsg = "Invalid Operator: ";
		errmsg << opr;
//		OmnThrowException(errmsg);
		return;
	}
}


AosExprComparison::~AosExprComparison()
{
}


AosDataType::E 
AosExprComparison::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record) 
{
	// 1. check comparion left/right data type
	aos_assert_r(mLHS, AosDataType::eInvalid);
	aos_assert_r(mRHS, AosDataType::eInvalid);

	AosDataType::E ltype = mLHS->getDataType(rdata, record);
	AosDataType::E rtype = mRHS->getDataType(rdata, record);

	if ((AosDataType::isCharacterType(ltype) && AosDataType::isCharacterType(rtype))
			|| (AosDataType::isNumericType(ltype) && AosDataType::isNumericType(rtype))
			|| (ltype == rtype))
	{
		return AosDataType::eBool;
	}

	OmnString msg = "";
	msg << "ERR: comparison type error, (" 
		<< mLHS->dumpByNoEscape() << ") with ("
		<< mRHS->dumpByNoEscape() << ")";
	rdata->setJqlMsg(msg);
	return AosDataType::eInvalid;
}


bool
AosExprComparison::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1;
	AosValueRslt v2;

	if (!mLHS->getValue(rdata, records, v1)) return false;
	if (!mRHS->getValue(rdata, records, v2)) return false;

	// by andy
	if (v1.isNull() || v2.isNull())
	{
		value.setBool(false);
		return true;
	}

	AosDataType::E type = AosDataType::autoTypeConvert(v1.getType(), v2.getType());

	switch (mOpr)
	{
		case eEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_eq, type, v1, v2));
			return true;

		case eNotEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ne, type, v1, v2));
			return true;

		case eLess:
			value.setBool(AosValueRslt::doComparison(eAosOpr_lt, type, v1, v2));
			return true;

		case eLarger:
			value.setBool(AosValueRslt::doComparison(eAosOpr_gt, type, v1, v2));
			return true;

		case eLessEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_le, type, v1, v2));
			return true;

		case eLargerEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ge, type, v1, v2));
			return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}


bool
AosExprComparison::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value) 
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	bool rslt = false;
	AosValueRslt vv2;

	if (mLHS->isConstant())
	{
		rslt = mLHS->getValue(0, 0, vv2);
		aos_assert_r(rslt, false);
	}
	else 
	{
		rslt = mRHS->getValue(0, 0, vv2);
		aos_assert_r(rslt, false);
	}

	// by andy
	if (key.isNull() || vv2.isNull())
	{
		value.setBool(false);
		return true;
	}

	AosDataType::E type = AosDataType::autoTypeConvert(key.getType(), vv2.getType());

	switch (mOpr)
	{
		case eEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_eq, type, key, vv2));
			return true;

		case eNotEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ne, type, key, vv2));
			return true;

		case eLess:
			value.setBool(AosValueRslt::doComparison(eAosOpr_lt, type, key, vv2));
			return true;

		case eLarger:
			value.setBool(AosValueRslt::doComparison(eAosOpr_gt, type, key, vv2));
			return true;

		case eLessEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_le, type, key, vv2));
			return true;

		case eLargerEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ge, type, key, vv2));
			return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}



bool
AosExprComparison::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1;
	AosValueRslt v2;

	if (!mLHS->getValue(rdata, record, v1)) return false;
	if (!mRHS->getValue(rdata, record, v2)) return false;

	// by andy
	if (v1.isNull() || v2.isNull())
	{
		value.setBool(false);
		return true;
	}

	AosDataType::E type = AosDataType::autoTypeConvert(v1.getType(), v2.getType());

	switch (mOpr)
	{
		case eEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_eq, type, v1, v2));
			return true;

		case eNotEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ne, type, v1, v2));
			return true;

		case eLess:
			value.setBool(AosValueRslt::doComparison(eAosOpr_lt, type, v1, v2));
			return true;

		case eLarger:
			value.setBool(AosValueRslt::doComparison(eAosOpr_gt, type, v1, v2));
			return true;

		case eLessEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_le, type, v1, v2));
			return true;

		case eLargerEqual:
			value.setBool(AosValueRslt::doComparison(eAosOpr_ge, type, v1, v2));
			return true;

	default:
		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
		 return false;
	}	

	OmnShouldNeverComeHere;
	return true;
}


//bool
//AosExprComparison::getValue(
//		AosRundata *rdata,
//		vector<AosDataRecordObj *> &records,
//		AosValueRslt &value)
//{
//	aos_assert_r(mLHS, false);
//	aos_assert_r(mRHS, false);
//
//	AosValueRslt vv1;
//	AosValueRslt vv2;
//
//	if (!mLHS->getValue(rdata, records, vv1)) return false;
//	if (!mRHS->getValue(rdata, records, vv2)) return false;
//
//	switch (mOpr)
//	{
//		case eEqual:
//			value.setValue(vv1 == vv2);
//			return true;
//
//		case eNotEqual:
//			value.setValue(vv1 != vv2);
//			return true;
//
//		case eLess:
//			value.setValue(vv1 < vv2);
//			return true;
//
//		case eLarger:
//			value.setValue(vv1 > vv2);
//			return true;
//
//		case eLessEqual:
//			value.setValue(vv1 <= vv2);
//			return true;
//
//		case eLargerEqual:
//			value.setValue(vv1 >= vv2);
//			return true;
//
//	default:
//		 AosSetError(rdata, "invalid_operator") << mOpr << enderr;
//		 return false;
//	}	
//
//	OmnShouldNeverComeHere;
//	return true;
//}


bool
AosExprComparison::createConds(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	cond->mLHS = mLHS->dumpByNoEscape();
	cond->mRHS = mRHS->getValue(rdata.getPtrNoLock());
	cond->mOpr = getExprOptStr(); 
	cond->mExpr = this;
	cond->mLHSExpr = mLHS;
	cond->mRHSExpr = mRHS;
	conds.push_back(cond);
	return true;
}

/*
bool
AosExprComparison::createConds(
		AosRundata *rdata,
		vector<AosWhereCond*> &conds, 
		AosExprObj*	table,
		map<OmnString, OmnString> &alias_map,
		map<OmnString, OmnString > &cmp_iil_list,
		AosValueRslt &value_rslt)
{
	// This function creates the where clause.
	// 1. Check whether it is an Always-True expression or
	//    Always-False expressoin. 
	// 2. Check Simple Condition: one side has only one field
	//    name term and the other side is a constant.
	// 3. Check Convertable Simple Condition: one side contains
	//    a field name term and some other constant terms, and
	//    the other side is a constant term. For this type of
	//    conditions, we need to transform it to the simple
	//    condition.
	// 3.1: Both sides have fields, but these fields are the same
	//    field.
	// 4. Check Multiple Field Condition: this type of conditions
	//    cannot be done using simple index. For example:
	//    		age/10 > years_of_service
	// 5. Check field name: 
	// 	  (a) determine whether it is a valid field name, 
	// 	  (b) determine table name
	// 	  (c) determine database name
	// 6. When transforming an expression, need to pay attention
	//    to:
	//    		age * 0 > 100
	
	bool rslt, isJoin;
	isJoin = false;
	AosJqlSessionMgrPtr jql_session_mgr = AosJqlSessionMgr::getJqlSessionMgr(); 
	if (!jql_session_mgr) return 0;
	OmnString database_name = jql_session_mgr->getCurDatabase(0, AosJqlStatement::mHandler.session_id);
	if (database_name == "")
	{                                                                                   
		rdata->setJqlMsg("ERROR: No database selected");                                
		return false;                                                                       
	}    

	OmnString table_name; 
	AosValueRslt vv;
	rslt = table->getValue(rdata, 0, vv);
	table_name = vv.getStr();
	aos_assert_r(table_name != "", 0);

	if (value_rslt.getBool()
	{
		// Check whether one side is a field and the other side
		// is a constant.
		AosValueRslt name, value;
		int lhs_fields = mLHS->getNumFieldTerms();
		int rhs_fields = mRHS->getNumFieldTerms();

		if (lhs_fields + rhs_fields > 1)
		{
			isJoin = true;
			//return procMultiFieldConds(...);
			//AosSetErrorUser(rdata, "query_not_supported_001") << enderr;
			//return false;
		}

		if (lhs_fields + rhs_fields == 0)
		{
			rslt = getValue(rdata, 0, value_rslt);
			aos_assert_r(rslt, false);
			return true;
			//return procConstConditions();
		}

		// Try to merge terms
		if (mLHS->isUnary() && mRHS->isUnary())
		{
			if (mLHS->isFieldName() && mRHS->isConstant())
			{
				// check this field;
				// AosResolveTablename(rdata, tablenames, mName);
				rslt = mLHS->getValue(rdata, 0, name);
				aos_assert_r(rslt, false);
				rslt = mRHS->getValue(rdata, 0, value);
				aos_assert_r(rslt, false);
			}
			else if (mLHS->isMemberOpt() && mRHS->isConstant())
			{
				AosExprMemberOpt *expr = dynamic_cast<AosExprMemberOpt*>(mLHS);
				aos_assert_r(expr, false);
				if (expr->getTableName() == table_name)                         
				{
					if (expr->getDatabase() != "")                              
					{                                                           
						if (expr->getDatabase() != database_name)
							return false;
					}                                                           
				}                                                               
				else
				//	isJoin = true;

				rslt = mLHS->getValue(rdata, 0, name);
				aos_assert_r(rslt, false);
				rslt = mRHS->getValue(rdata, 0, value);
				aos_assert_r(rslt, false);

			}
			else
			{
				// check this field;
				// AosResolveTablename(rdata, tablenames, mName);
				rslt = mRHS->getValue(rdata, 0, name);
				aos_assert_r(rslt, false);
				rslt = mLHS->getValue(rdata, 0, value);
				aos_assert_r(rslt, false);
			}

			OmnString aname;
			aname = name.getStr();
			if (alias_map.find(aname) != alias_map.end() && alias_map[aname] != "")
			{
				aname = alias_map[aname];
			}

			AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);

			OmnString iilname, cmp_iil;
			iilname = cmp_iil = "";
		//	if (!table_doc) 
		//		iilname = AosIILName::composeCtnrMemberListing("", aname);           
		//	else
			iilname = AosJqlStatement::getIILName(rdata, table_doc, aname, cmp_iil, cmp_iil_list, false);

			AosWhereCond *cond = new AosWhereCond;
			cond->mLHS = aname;
			if (cmp_iil != "")
			{
				cond->mCMPIILName = cmp_iil;
			}

			if (mLHS->isMemberOpt()) 
			{
				OmnString str = mLHS->dump();
				AosJqlUtil::escape(str);
				cond->mLHS = str;
			}
			cond->mRHS = value.getStr();
			cond->mOpr = getExprOptStr(); 
			cond->mIILName = iilname; 
			cond->mTable = table_name; 
			cond->mDataBase = database_name; 
			cond->mIsJoin = isJoin; 
			//cond->mExpr = this;
			value_rslt.setValue(true);
			conds.push_back(cond);
			return true;
		}
		else
		{
			// emp_age*5000 > 10000*100
			bool merged; 
			AosExprObj *term = 0;

			merged = false;
			if (!mLHS->isUnary())
				mLHS->mergeTerms(rdata, merged, term);

			//merge result can be failure. But this is not an error
			//aos_assert_r(rslt, false);
			if (merged) mLHS = term;

			merged = false;
			if (!mRHS->isUnary())
				mRHS->mergeTerms(rdata, merged, term);

			//merge result can be failure. But this is not an error
			//aos_assert_r(rslt, false);
			if (merged) mRHS = term;

			//assume no Jimo Cond can be used here, do nothing for now
			return true;
		}
	}
	return true;
}
*/

bool
AosExprComparison::isJoin()const
{
	if (mLHS->getNumFieldTerms() && mRHS->getNumFieldTerms())
		return true;
	return false;
}


AosExprObjPtr
AosExprComparison::cloneExpr() const                                 
{
	try
	{
		AosExprComparisonPtr expr = OmnNew AosExprComparison();
		aos_assert_r(mLHSPtr && mRHSPtr, NULL);
		expr->mLHSPtr = mLHSPtr->cloneExpr();
		expr->mLHS = expr->mLHSPtr.getPtr();
		expr->mRHSPtr = mRHSPtr->cloneExpr();
		expr->mRHS = expr->mRHSPtr.getPtr();
		expr->mOpr = mOpr;
		return expr; 
	}   

	catch (...)
	{
		OmnAlarm << "failed create expr" << enderr;
		return 0;
	}   

	OmnShouldNeverComeHere;
	return 0;
}   

bool 
AosExprComparison::isAlwaysFalse() 
{
	AosRundataPtr rdata = OmnApp::getRundata();
	AosValueRslt value_rslt;
	int lhs_fields = mLHS->getNumFieldTerms();
	int rhs_fields = mRHS->getNumFieldTerms();
	if (lhs_fields + rhs_fields == 0)
	{
		bool rslt = getValue(rdata.getPtr(), 0, value_rslt);
		aos_assert_r(rslt, false);
		return !value_rslt.getBool();
	}
	return false;
}


OmnString   
AosExprComparison::getEnumStr() const
{
	if (mOpr == eEqual) return "=";
	if (mOpr == eLargerEqual) return ">=";
	if (mOpr == eLarger) return ">";
	if (mOpr == eLessEqual) return "<=";
	if (mOpr == eLess) return "<";
	if (mOpr == eNotEqual) return "!=";
	return "";
}


OmnString 
AosExprComparison::dump() const
{
	OmnString str, opr;
	opr = getEnumStr();
	aos_assert_r(opr != "", "");
	str << mLHS->dump() << opr << mRHS->dump();
	return str;
}

OmnString 
AosExprComparison::dumpByNoEscape() const
{
	OmnString str, opr;
	opr = getEnumStr();
	aos_assert_r(opr != "", "");
	str << mLHS->dumpByNoEscape() << opr << mRHS->dumpByNoEscape();
	return str;
}

OmnString 
AosExprComparison::dumpByStat() const
{
	OmnString str, opr;
	opr = getEnumStr();
	aos_assert_r(opr != "", "");
	str << mLHS->dumpByStat() << opr << mRHS->dumpByStat();
	return str;
}


OmnString
AosExprComparison::getExprOptStr()const
{
	if (mOpr == eEqual) return "eq";
	if (mOpr == eLargerEqual) return "ge";
	if (mOpr == eLarger) return "gt";
	if (mOpr == eLessEqual) return "le";
	if (mOpr == eLess) return "lt";
	if (mOpr == eNotEqual) return "ne";
	return "";
}


