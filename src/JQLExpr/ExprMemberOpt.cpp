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
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprMemberOpt.h"

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelectField.h"

#include "API/AosApi.h"
#include "Util/String.h"


AosExprMemberOpt::AosExprMemberOpt(
		const char *v1,
		const char *v2,
		const char *v3)
{
	mMember1 = v1;
	mMember2 = v2;
	mMember3 = v3;
	mRecordIdx = -1;
	mFieldIdx = -1;
}


bool
AosExprMemberOpt::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value_rslt)
{
	if (mFieldIdx < 0)
	{
		mFieldName = mMember3 != "" ? mMember3 : mMember2;
		mFieldIdx = record->getFieldIdx(mFieldName, rdata); 
		aos_assert_r(mFieldIdx >= 0, false);
	}

	if (record)
	{
		bool rslt = record->getFieldValue(mFieldIdx, value_rslt, true, rdata);
		if (rslt)
		{
			return true;
			AosDataType::E type = value_rslt.getType();
			if (type == AosDataType::eString 
					|| type == AosDataType::eCharStr 
					|| type == AosDataType::eChar)
			{
				OmnString vv = value_rslt.getStr();
				vv.removeLeadingWhiteSpace();
				value_rslt.setStr(vv);
			}
			return true;
		}
		
		OmnString errmgr = "Record is not find this field : field name is ";
		errmgr << mFieldName;

		AosSetEntityError(rdata, errmgr, 
			"Deny Reason", errmgr) << enderr;
		return false;
	}

	if (mFieldName != "")
	{
		value_rslt.setStr(mFieldName);
		return true;
	}
	return false;
}

bool
AosExprMemberOpt::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value_rslt)
{
	if ( !records.size() ) return false;
	
	if (mRecordIdx < 0)
	{
		OmnString table_name, record_name;
		mFieldName = mMember3 != "" ? mMember3 : mMember2;
		table_name = mMember2 != "" ? mMember2 : mMember1;
		for (size_t i = 0; i < records.size(); i++)
		{
			record_name = records[i]->getRecordName();
			if (record_name == table_name)
			{
				mRecordIdx = i;
				break;
			}
		}
		aos_assert_r(mRecordIdx >= 0 && size_t(mRecordIdx) < records.size(), false);
		mFieldIdx = records[mRecordIdx]->getFieldIdx(mFieldName, rdata); 
		aos_assert_r(mFieldIdx >= 0, false);
	}
	bool rslt = records[mRecordIdx]->getFieldValue(mFieldIdx, value_rslt, true, rdata);
	if (rslt)
	{
		return true;
		AosDataType::E type = value_rslt.getType();
		if (type == AosDataType::eString 
				|| type == AosDataType::eCharStr 
				|| type == AosDataType::eChar)
		{
			OmnString vv = value_rslt.getStr();
			vv.removeLeadingWhiteSpace();
			value_rslt.setStr(vv);
		}
		return true;
	}
	else
	{
		OmnString errmgr = "Record is not find this field : field name is ";
		errmgr << mFieldName;

		AosSetEntityError(rdata, errmgr, 
				"Deny Reason", errmgr) << enderr;
		return false;
	}

	if (mFieldName != "")
	{
		value_rslt.setStr(mFieldName);
		return true;
	}
	return false;
}


OmnString
AosExprMemberOpt::dump() const 
{
	OmnString str = "";
	if (mMember1 != "")
		str << "`" << mMember1 << "`" << "." ;
	if (mMember2 != "")
		str << "`" << mMember2 << "`" << "." ;
	str << "`" << mMember3 << "`";   
	return str;
};

OmnString
AosExprMemberOpt::dumpByNoEscape() const 
{
	OmnString str = "";
	if (mMember1 != "")
		str << mMember1 << "." ;
	if (mMember2 != "")
		str << mMember2 << "." ;
	str << mMember3;   
	return str;
};

OmnString
AosExprMemberOpt::dumpByStat() const 
{
	return dump();
};

bool
AosExprMemberOpt::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	AosJqlSelectFieldPtr n_field = OmnNew AosJqlSelectField;
	AosExprObjPtr expr = cloneExpr();
	aos_assert_r(expr, false);
	n_field->setField(expr);
	fields.push_back(n_field);
	return true;
}

AosExprObjPtr
AosExprMemberOpt::cloneExpr() const
{
	try                                                                   
	{                                                                     
		AosExprMemberOptPtr expr = OmnNew AosExprMemberOpt();                     
		expr->mMember1 = mMember1;                                          
		expr->mMember2 = mMember2;                                          
		expr->mMember3 = mMember3;                                          
		expr->mRecordIdx = mRecordIdx;
		expr->mFieldIdx = mFieldIdx;
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
AosExprMemberOpt::checkSequenceExist(const AosRundataPtr& rdata)
{
	return AosCheckSequenceExist(rdata, mMember2, mMember3);
}


AosExprObjPtr
AosExprMemberOpt::getSequenceExpr(const AosRundataPtr &rdata)
{
	OmnString expr_str, errmsg;
	expr_str << mMember3 << "(\"" << mMember2 << "\");";
	mExpr = AosParseExpr(expr_str, errmsg, rdata.getPtr()); 
	aos_assert_r(mExpr, NULL);
	return mExpr;
}

OmnString 
AosExprMemberOpt::getSequenceName(const AosRundataPtr &rdata)
{
	OmnString name ;
	name << mMember2 << "." << mMember3;
	return name; 
}


void
AosExprMemberOpt::setMember1(const char *vv)
{
	mMember1 = vv;
}


void
AosExprMemberOpt::setMember2(const char *vv)
{
	mMember2 = vv;
}

void
AosExprMemberOpt::setMember3(const char *vv)
{
	mMember3 = vv;
}


OmnString 
AosExprMemberOpt::getMember1()
{
	return mMember1;
}


OmnString 
AosExprMemberOpt::getMember2()
{
	return mMember2;
}

OmnString 
AosExprMemberOpt::getMember3()
{
	return mMember3;
}


AosDataType::E 
AosExprMemberOpt::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_r(record, AosDataType::eInvalid);
	OmnString recordname = record->getRecordName();
	if (recordname != mMember2)
	{
		return AosDataType::eInvalid;
	}

	AosDataFieldObj* field = record->getDataField(rdata, mMember3);
	return field->getDataType(rdata, record);
}
