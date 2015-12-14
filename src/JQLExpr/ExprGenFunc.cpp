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
// All functions are implemented through jimos. Given a function, 
// its jimo doc's objid is:
//		AOSOBJIDRES_FUNC_JIMODOC + funcname
//
// Modification History:
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprGenFunc.h"

#include "SEInterfaces/AggregationType.h"
#include "SEInterfaces/DllMgrObj.h"

#include "JQLStatement/JqlSelectField.h"
#include "JQLExpr/ExprNameValue.h"
#include "JQLExpr/ExprNumber.h"

#include "Jimo/JimoGenFunc.h"
#include "Jimo/Jimo.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"

map<OmnString, OmnString>  AosExprGenFunc::sgFuncNamesMap;

AosExprGenFunc::AosExprGenFunc()
:
mExpr(0)
{
	mJimo = NULL;
	mIsAggr = false;
}


AosExprGenFunc::AosExprGenFunc(
		const OmnString &fname,
		AosExprObjPtr expr)
:
mFuncName(fname),
mExpr(expr)
{
	//arvin 2015.10.19
	//JIMODB-974
	mIsAggr = false;
	//2015/8/4 xuqi JIMODB-207
	funcNamesMapInit();
	init(fname,expr);
}

AosExprGenFunc::AosExprGenFunc(
		const OmnString &fname, 
		AosExprList  *parms)
:
mFuncName(fname),
mParms(parms),
mExpr(0)
{
	mIsAggr = false;
	//2015/8/4 xuqi JIMODB-207
	funcNamesMapInit();
	init(fname,parms);
}


AosExprGenFunc::~AosExprGenFunc()
{
}


bool 
AosExprGenFunc::init(const OmnString &fname,AosExprObjPtr expr)
{
	mJimo = NULL;
	mTestTag = false;

	mParms = new AosExprList;
	mParms->push_back(expr->cloneExpr());
	aos_assert_rm(fname != "", false, "%s","invalid function name");

	OmnString tmp = fname;
	AosAggregationType::E type = AosAggregationType::toEnum(tmp.toLower());
	if (AosAggregationType::isValid(type))
	{
		mIsAggr = true;
		return true;
	}

	OmnString objid = AOSOBJIDRES_FUNC_JIMODOC;
	objid << fname;
	
	AosRundataPtr rdata = OmnApp::getRundata();
	AosXmlTagPtr doc = createJimoDoc(rdata, fname); 
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), doc, 1);
	aos_assert_rm(jimo, false, "%s","function jimo not found");
	mJimoPtr = dynamic_cast<AosJimoGenFunc *>(jimo.getPtr());
	aos_assert_rm(mJimoPtr, false,"%s","not funtion jimo");

	mJimo = mJimoPtr.getPtr();
	mJimo->setParms(rdata.getPtr(), mParms);
	OmnString errmsg;                                      
	if (!mJimo->syntaxCheck(rdata, errmsg))       
	{                                                      
		if (rdata)                                         
			rdata->setJqlMsg(errmsg);                      
		return false;
	} 
	return true;
}


bool
AosExprGenFunc::init(const OmnString &fname, AosExprList *parms)
{
	mJimo = NULL;
	mTestTag = false;
	if (fname == "")
	{
		aos_assert_rm(NULL,false,"%s","invalid function name");
		OmnThrowException("invalid_func_name");
	}

	OmnString tmp = fname;
	AosAggregationType::E type = AosAggregationType::toEnum(tmp.toLower());
	if (AosAggregationType::isValid(type))
	{
		mIsAggr = true;
		return true;
	}

	// Jimo doc objid is constructed as:
	// 	AOSOBJIDRES_FUNC_JIMODOC + func_name
	if(tmp == "matrix" || tmp == "csv" || tmp == "local_file")
	{
		return true;
	}

	OmnString objid = AOSOBJIDRES_FUNC_JIMODOC;
	objid << fname;
	AosRundataPtr rdata = OmnApp::getRundata();
	AosDllMgrObj *mgr = AosDllMgrObj::getDllMgr1();
	aos_assert_r(mgr, false);

	AosXmlTagPtr doc = createJimoDoc(rdata, fname); 
	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), doc, 1);
	aos_assert_rm(jimo, false, "%s", "function jimo not found");

	mJimoPtr = dynamic_cast<AosJimoGenFunc *>(jimo.getPtr());
	aos_assert_rm(mJimoPtr, false, "%s", "not function jimo");

	mJimo = mJimoPtr.getPtr();
	mJimo->setParms(rdata.getPtr(), mParms);
	OmnString errmsg;                                      
	if (!mJimo->syntaxCheck(rdata, errmsg))       
	{                                                      
		if (rdata)                                         
			rdata->setJqlMsg(errmsg);                      
	}                                                      
	return true;
}



AosDataType::E 
AosExprGenFunc::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_rr(mJimo, rdata, AosDataType::eInvalid);
	return mJimo->getDataType(rdata, record);
}


bool
AosExprGenFunc::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	//if the function is already evaluated as the result of a field
	//in the record, such as aggr function, return the value from
	//the field directly
	//
	if (mIsAggr)
	{
		if (record && (record->getType() == AosDataRecordType::eStatRecord) )
		{
			OmnString exprStr = dumpByStat();
			return record->getFieldValue(exprStr, value, true, rdata);
		}
	}

	aos_assert_rr(mJimo, rdata, false);
	return mJimo->getValue(rdata, value, record);
}


u32
AosExprGenFunc::getParms(
		const AosRundataPtr &rdata,
		vector<AosExprObjPtr> &parms)
{
	parms = *mParms;
	return parms.size();
}

bool
AosExprGenFunc::mergeTerms(
		const AosRundataPtr &rdata, 
		bool &merged, 
		AosExprObjPtr &merged_term)
{
	return true;
}


AosExprObjPtr 
AosExprGenFunc::cloneExpr() const 
{  
	try
	{   
		AosExprGenFuncPtr expr = OmnNew AosExprGenFunc();
		AosExprList* parms = new AosExprList;
		expr->mFuncName = mFuncName;
		for (u32 i=0; i<mParms->size(); i++)
		{
			parms->push_back((*mParms)[i]->cloneExpr());
		}
		expr->mParms = parms;
		expr->mIsAggr = mIsAggr;
		if (mExpr)
			expr->mExpr = mExpr->cloneExpr();
		if (mJimoPtr)
		{
			AosJimoPtr jimo = mJimoPtr->cloneJimo();
			expr->mJimoPtr  = dynamic_cast<AosJimoGenFunc *>(jimo.getPtr()); 
			expr->mJimo = expr->mJimoPtr.getPtr();
//			expr->mJimo->setParms(0, expr->mParms);
			aos_assert_r(expr->mJimo, expr);
		}
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


AosXmlTagPtr
AosExprGenFunc::createJimoDoc(
		const AosRundataPtr &rdata,
		OmnString class_name,
		const OmnString &lib_name,
		const u32 ver)
{
	class_name.toLower();
	OmnString fname = sgFuncNamesMap[class_name];
	aos_assert_r(fname != "", NULL);
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"" << ver << "\" "
		<< "zky_classname=\"" << fname << "\" " 
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_" << fname << "\">"
		<< "<versions>"
		<<      "<version_1>" << lib_name << "</version_1>"
		<< "</versions>"
		<< "</jimo>"; 

	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);       
	//aos_assert_r(jimo_doc, 0);                                        
	aos_assert_rm(jimo_doc,0,"%s","can not create jimo doc");                                      
	return jimo_doc;                                                  
}


bool
AosExprGenFunc::getFields(
	const AosRundataPtr &rdata,
	vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt = false;
	aos_assert_r(mParms, false);
	AosExprObjPtr expr;
	OmnString fname = mFuncName;

	for (u32 i = 0; i < mParms->size(); i++)
	{
		expr = (*mParms)[i];
		aos_assert_r(expr, false);
		rslt = expr->getFields(rdata, fields);
		aos_assert_r(rslt, false);
	}

	AosAggregationType::E type = AosAggregationType::toEnum(fname.toLower());
	if (AosAggregationType::isValid(type))
	{
		AosJqlSelectFieldPtr n_field = OmnNew AosJqlSelectField;
		AosExprObjPtr expr = cloneExpr();
		aos_assert_r(expr, false);
		n_field->setField(expr);
		fields.push_back(n_field);
	}
	return true;
}

bool
AosExprGenFunc::getFieldsByStat(
	const AosRundataPtr &rdata,
	vector<AosJqlSelectFieldPtr> &fields)
{
	bool rslt = false;

	aos_assert_r(mParms ,false);
	AosExprObjPtr expr;
	OmnString fname = mFuncName;

	
	AosAggregationType::E type = AosAggregationType::toEnum(fname.toLower());
	if (AosAggregationType::isValid(type))
	{
		AosJqlSelectFieldPtr n_field = OmnNew AosJqlSelectField;
		AosExprObjPtr expr = cloneExpr();
		aos_assert_r(expr, false);
		n_field->setField(expr);
		fields.push_back(n_field);
	}
	else
	{
		for (u32 i = 0; i < mParms->size(); i++)
		{
			expr = (*mParms)[i];
			aos_assert_r(expr, false);
			rslt = expr->getFieldsByStat(rdata, fields);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


int 
AosExprGenFunc::getNumFieldTerms() const
{
	int num = 0;
	AosExprObjPtr expr;
	if (mParms)
	{
		for (u32 i=0; i<mParms->size(); i++)
		{
			expr = (*mParms)[i];
			aos_assert_r(expr, num);
			AosExprType::E type = expr->getType();
			if (AosExprType::eFieldName == type ||AosExprType::eMemberOpt == type)
				num++;
		}
	}
	
	return num;
}


//
//return true if the function is an aggregation func
//
bool
AosExprGenFunc::fieldExprWithAggr() const 
{
	OmnString fname = mFuncName;
	AosAggregationType::E type = AosAggregationType::toEnum(fname.toLower());
	return (AosAggregationType::isValid(type));
}


OmnString
AosExprGenFunc::dump() const
{
	OmnString str, vv;
	AosExprObjPtr expr;
	str << "`" << mFuncName << "`(";
	if (mParms && !(mParms->empty()))
	{
		for (u32 i = 0; i < mParms->size(); i++)
		{
			expr = (*mParms)[i];
			if (expr)
			{
				if (i > 0) str << ",";
				str << expr->dump();
			}
		}
	}
	str << ")";
	return str;
}


OmnString
AosExprGenFunc::dumpByNoEscape() const
{
	OmnString str, vv;
	AosExprObjPtr expr;
	str <<  mFuncName << "(";
	if (mParms && !(mParms->empty()))
	{
		for (u32 i = 0; i < mParms->size(); i++)
		{
			expr = (*mParms)[i];
			if (expr)
			{
				if (i > 0) str << ",";
				str << expr->dumpByNoEscape();
			}
		}
	}
	str << ")";
	return str;
}

OmnString
AosExprGenFunc::dumpByStat() const
{
	OmnString fname = mFuncName;
	OmnString str;
	AosExprObjPtr expr;
	AosAggregationType::E type = AosAggregationType::toEnum(fname.toLower());
	if (!AosAggregationType::isValid(type)) 
	{
		//2015/8/13 xuqi JIMODB-390
		str <<  mFuncName << "(";
		for (u32 i = 0; i < mParms->size(); i++)
		{
			expr = (*mParms)[i];
			if (i > 0) str << ",";
			str << expr->dumpByStat();
		}
		str << ")";
		return str;
	}

	str = dumpByNoEscape();
	AosJqlUtil::escape(str);
	return str;
}


OmnString
AosExprGenFunc::getStatFieldName()
{
	aos_assert_r(mParms->size() == 1, "");
	OmnString str;
	AosExprObjPtr expr;
	str << mFuncName << "(";
	expr = (*mParms)[0];
	str << expr->dumpByNoEscape();
	str << ")";
	return str;
}

bool 
AosExprGenFunc::isExprCond() const
{
	OmnString fname = mFuncName;
	AosAggregationType::E type = AosAggregationType::toEnum(fname.toLower());
	if (!AosAggregationType::isValid(type)) 
		return true;
	return false;
}

void
AosExprGenFunc::funcNamesMapInit()
{
	if (!sgFuncNamesMap.empty()) return ;
	sgFuncNamesMap["abs"] = "AosAbs";
	sgFuncNamesMap["acos"] = "AosAcos";
	sgFuncNamesMap["addsec"]="AosAddsec";
	sgFuncNamesMap["ascii"]="AosAscii";
	sgFuncNamesMap["asin"] = "AosAsin";
	sgFuncNamesMap["atan"]="AosAtan";
	sgFuncNamesMap["avg"] = "AosAvg";
	sgFuncNamesMap["bin"] = "AosBin";
	sgFuncNamesMap["bitwiseand"] = "AosBitwiseAnd";
	sgFuncNamesMap["bitwiseor"] = "AosBitwiseOr";
	sgFuncNamesMap["bit_count"] = "AosBit_count";
	sgFuncNamesMap["bit_length"] = "AosBit_length";
	sgFuncNamesMap["ceiling"] = "AosCeil";
	sgFuncNamesMap["check_database_exist"] = "AosCheckDatabaseExist";
	sgFuncNamesMap["check_table_exist"] = "AosCheckTableExist";
	sgFuncNamesMap["coalesce"] = "AosCoalesce";
	sgFuncNamesMap["concat"] = "AosConcat";
	sgFuncNamesMap["cos"] = "AosCos";
	sgFuncNamesMap["cot"] = "AosCot";
	sgFuncNamesMap["count"] = "AosCount";
	sgFuncNamesMap["curdate"] = "AosCurDate";
	sgFuncNamesMap["curtime"] = "AosCurTime";
	sgFuncNamesMap["currval"] = "AosCurrVal";
	sgFuncNamesMap["datepart"] = "AosDatePart";
	sgFuncNamesMap["date_add"] = "AosDateAdd";
	sgFuncNamesMap["date_day"] = "AosDateDay";
	sgFuncNamesMap["date_format"] = "AosDateFormat";
	sgFuncNamesMap["date_hour"] = "AosDateHour";
	sgFuncNamesMap["date_month"] = "AosDateMon";
	sgFuncNamesMap["date_sub"] = "AosDateSub";
	sgFuncNamesMap["date_week"] = "AosDateWeek";
	sgFuncNamesMap["day"] = "AosDay";
    sgFuncNamesMap["decode"] = "AosDecode";
	sgFuncNamesMap["dist_count"] = "AosCount";
	sgFuncNamesMap["elt"] = "AosElt";
	sgFuncNamesMap["epoch2str"] = "AosEpoch2Str"; 
	sgFuncNamesMap["execjql"] = "AosExecuteJQL";
	sgFuncNamesMap["exp"] = "AosExp";
	sgFuncNamesMap["field"] = "AosField";
	sgFuncNamesMap["floor"] = "AosFloor";
	sgFuncNamesMap["from_base64"] = "AosFromBase64";
	sgFuncNamesMap["func_get_first_day"] = "AosFuncGetFirstDay";
	sgFuncNamesMap["func_get_first_day_of_mon"] = "AosFuncGetFirstDayOfMonth";
	sgFuncNamesMap["func_get_last_day"] = "AosFuncGetLastDay";
	sgFuncNamesMap["func_get_last_day_of_mon"] = "AosFuncGetLastDayOfMonth";
	sgFuncNamesMap["func_get_month_id"] = "AosFuncGetMonthID";
	sgFuncNamesMap["func_next_mon_id"] = "AosFuncNextMonthID";
	sgFuncNamesMap["func_prev_date_id"] = "AosFuncPrevDateID";
	sgFuncNamesMap["func_prev_mon_id"] = "AosFuncGetPrevMonthID";
	sgFuncNamesMap["getcubeid"] = "AosGetCubeId";
	sgFuncNamesMap["getcubeidfromdistmap"] = "AosDistMap";
	sgFuncNamesMap["getdocid"] = "AosGetDocid";
	sgFuncNamesMap["getmetadata"] = "AosGetMetaData";
	sgFuncNamesMap["greatest"] = "AosGreatest";
	sgFuncNamesMap["hour"] = "AosHour";
	sgFuncNamesMap["instr"] = "AosInstr";
	sgFuncNamesMap["iilmap"] = "AosIILMap";
	sgFuncNamesMap["iilentry"] = "AosIILEntry";
	sgFuncNamesMap["least"] = "AosLeast";
	sgFuncNamesMap["left"] = "AosLeft";
	sgFuncNamesMap["length"] = "AosLength";
	sgFuncNamesMap["lower"] = "AosLower";
	sgFuncNamesMap["ltrim"] = "AosLtrim";
	sgFuncNamesMap["minute"] = "AosMinute";
	sgFuncNamesMap["month"] = "AosMonth";
	sgFuncNamesMap["mod"] = "AosMod";
	sgFuncNamesMap["nextval"] = "AosNextVal";
	sgFuncNamesMap["now"]   = "AosNow";
	sgFuncNamesMap["nvl"] = "AosNvl";
	sgFuncNamesMap["quote"]="AosQuote";
	sgFuncNamesMap["rand"] = "AosRand";
	sgFuncNamesMap["repeat"] = "AosRepeat";
	sgFuncNamesMap["replace"] = "AosReplace";
	sgFuncNamesMap["reverse"] = "AosReverse";
	sgFuncNamesMap["right"] = "AosRight";
	sgFuncNamesMap["round"] = "AosRound";
	sgFuncNamesMap["rtrim"] = "AosRtrim";
	sgFuncNamesMap["second"] = "AosSecond";
	sgFuncNamesMap["sign"] = "AosSign";
	sgFuncNamesMap["sin"] = "AosSin";
	sgFuncNamesMap["space"] = "AosSpace";
	sgFuncNamesMap["str2epoch"] = "AosStr2Epoch";
	sgFuncNamesMap["strcmp"] = "AosStrcmp";
	sgFuncNamesMap["str_to_date"] = "AosStr2Date";
	sgFuncNamesMap["substr"] = "AosSubstr";
	sgFuncNamesMap["substring"] = "AosSubstring";
	sgFuncNamesMap["sysdate"] = "AosSysDate";
	sgFuncNamesMap["tan"] = "AosTan";
	sgFuncNamesMap["time_converter"] = "AosConvert";
	sgFuncNamesMap["tochar"] = "AosChar";
	sgFuncNamesMap["to_base64"] = "AosToBase64";
	sgFuncNamesMap["to_char"] = "AosToChar";
	sgFuncNamesMap["to_date"] = "AosToDate";
	sgFuncNamesMap["to_number"] = "AosToNumber";
	sgFuncNamesMap["tz_offset"] = "AosTzOffset";
	sgFuncNamesMap["trim"] = "AosTrim";
	sgFuncNamesMap["trunc"] = "AosTrunc";
	sgFuncNamesMap["truncate"] = "AosTruncate";
	sgFuncNamesMap["timestampdiff"] = "AosTimeStampDiff";
	sgFuncNamesMap["upper"] = "AosUpper"; 
	sgFuncNamesMap["year"] = "AosYear";
	sgFuncNamesMap["_day"] = "AosDateDay";
	sgFuncNamesMap["_hour"] = "AosDateHour";
	sgFuncNamesMap["_minute"] = "AosDateMinute";
	sgFuncNamesMap["_month"] = "AosDateMonth";
	sgFuncNamesMap["_second"] = "AosDateSecond";
	sgFuncNamesMap["_week"] = "AosDateWeek";
	sgFuncNamesMap["_year"] = "AosDateYear";
	return ;
}


OmnString 
AosExprGenFunc::getFuctName()const
{
	return mFuncName;      
}


AosExprList*
AosExprGenFunc::getParmList()const 
{
	return mParms;         
}


AosExprObjPtr 
AosExprGenFunc::getFieldExpr() 
{
	return	mExpr; 
}
