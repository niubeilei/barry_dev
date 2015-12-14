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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/Jimos/TermArithNew.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEUtil/IILName.h"
#include "QueryCond/QueryCond.h"
#include "QueryUtil/QueryFilter.h"
#include "SEInterfaces/QueryVarObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosTermArithNew_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosTermArithNew(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosTermArithNew::AosTermArithNew(const int version)
:
AosTermIILType(AOSTERMTYPE_JIMO, AosQueryType::eJimo, false),
AosJimo(AosJimoType::eQueryTerm, version)
{
}


/*
AosTermArithNew::AosTermArithNew(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_JIMO, AosQueryType::eJimo, false),
AosJimo(AosJimoType::eQueryTerm, 1)
{
	if (!config(rdata, def))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}

	mIsGood = true;
}
*/


bool
AosTermArithNew::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	// The new form will be:
	// 	<term type="arith" 
	// 		order="true|false" 
	// 		reverse="true|false"
	// 		with_values="true|false"
	// 		zky_opr="ddd"
	// 		iilname="xxx"
	// 		<value1>xxx</value1>
	// 		<value2>xxx</value2>
	// 		<aname ...>xxx</aname>
	// 		<container>xxx</container>
	// 	</term>
	
	mReverse = worker_doc->getAttrBool("reverse", false);
	mOrder = worker_doc->getAttrBool("order", false);
	mWithValues = worker_doc->getAttrBool("with_values", false);

	if (!retrieveFieldname(rdata, worker_doc)) return false;
	if (!retrieveContainer(rdata, worker_doc)) return false;
	if (!retrieveOpr(rdata, worker_doc)) return false;
	if (!retrieveValue1(rdata, worker_doc)) return false;
	if (!retrieveValue2(rdata, worker_doc)) return false;
	if (!resolveIndex(rdata, worker_doc)) return false;
	if (!retrieveQueryFilters(rdata, worker_doc)) return false;

	OmnScreen  << "========================Run Query 005 :: IILName : " << mIILName << endl; 

	return true;
}


bool 
AosTermArithNew::resolveIndex(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	//if (mOpr == eAosOpr_Objid)
	//{
	//	// It is to query based on objid
	//	if (mCtnrObjid != "") 
	//	{
	//		mIILName = AosIILName::composeCtnrMemberListing(mCtnrObjid, mFieldname);
	//		return true;
	//	}
		
	//	mIILName = AosIILName::composeObjidListingName(rdata->getSiteid());
	//	return true;
	//}

	mIILName = def->getAttrStr("iilname");
	if (mIILName != "") return true;
	
	// Resolve the index
	if (!AosIndexMgrObj::resolveIndexStatic(rdata.getPtr(), mCtnrObjid, mFieldname, mIILName))
	{
		AosSetEntityError(rdata, "termarithnew_internal_erro", "Query", "")
			<< def << enderr;
		return false;
	}

	return true;
}


bool
AosTermArithNew::retrieveValue2(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	if (AosOpr_isRangeOpr(mOpr))
	{
		AosXmlTagPtr tag = def->getFirstChild("value2");
		if (!tag)
		{
			AosSetEntityError(rdata, "termarithnew_missing_second_value", "Query", "")
				<< def << enderr;
			return false;
		}

		mFormat2 = tag->getAttrStr("format");

	//	AosValueRslt vv;
	//	AosQueryVarObj::evalStatic(rdata, tag, vv);
	//	if (!vv.isValid())
	//	{
	//		AosSetEntityError(rdata, "termarithnew_invalid_second_value", "Query", "")
	//			<< def << enderr;
	//		return false;
	//	}

		//mValue2 = vv.getValueStr1();

		mValue2 = tag->getNodeText();
		if (mValue2 == "")
		{
			AosSetEntityError(rdata, "termarithnew_invalid_second_value", "Query", "")
				<< def << enderr;
			return false;
		}
	}
	return true;
}


bool
AosTermArithNew::retrieveOpr(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	if (mValue == "*")
	{
		mOpr = eAosOpr_an;
		return true;
	}

	mOpr = AosOpr_toEnum(def->getAttrStr("zky_opr"));
	if (!AosOpr_valid(mOpr))
	{
		AosSetEntityError(rdata, "termarithnew_invalid_operator", "Query", "")
			<< def << enderr;
		return false;
	}

	return true;
}


bool
AosTermArithNew::retrieveValue1(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	AosXmlTagPtr tag = def->getFirstChild("value1");
	if (!tag)
	{
		AosSetEntityError(rdata, "termarithnew_missing_value", "Query", "")
			<< def << enderr;
		return false;
	}

	mFormat1 = tag->getAttrStr("format");

	/*AosValueRslt vv;
	AosQueryVarObj::evalStatic(rdata, tag, vv);
	if (!vv.isValid())
	{
		AosSetEntityError(rdata, "termarithnew_invalid_value", "Query", "")
			<< def << enderr;
		return false;
	}

	mValue = vv.getValueStr1();*/
	mValue = tag->getNodeText();
	if (mValue == "")
	{
		AosSetEntityError(rdata, "termarithnew_invalid_value", "Query", "")
			<< def << enderr;
		return false;
	}

	return true;
}


bool
AosTermArithNew::retrieveContainer(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	mCtnrObjid = "";
	AosXmlTagPtr tag = def->getFirstChild("container");
	AosValueRslt vv;
	if (tag)
	{
		if (!AosQueryVarObj::evalStatic(rdata, tag, vv)) return false;
		mCtnrObjid = vv.getStr();
	}

	return true;
}


bool
AosTermArithNew::retrieveFieldname(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def)
{
	// <aname var_type="value_literal"> xxx </aname>
	// <aname var_type="value_func" func_type="xxxx" > xxx </aname>
	if (def->getAttrStr("iilname") != "") return true;

	AosXmlTagPtr tag = def->getFirstChild("aname");
	if (!tag)
	{
		AosSetEntityError(rdata, "termarithnew_missing_field", "Query", "")
			<< def << enderr;
		return false;
	}

	AosValueRslt vv;
	if (!AosQueryVarObj::evalStatic(rdata, tag, vv)) 
	{
		AosSetEntityError(rdata, "termarithnew_invalid_fieldname", "Query", "")
			<< def << enderr;
		return false;
	}

	mFieldname = vv.getStr();
	if (mFieldname == "") 
	{
		AosSetEntityError(rdata, "termarithnew_missing_fieldname", "Query", "")
			<< def << enderr;
		return false;
	}

	return true;
}


void
AosTermArithNew::toString(OmnString &str)
{
	// 	<arith iilname="xxx" reverse="xxx" opr="xxx" value="xxx"/>
	OmnString od = (mReverse)?"true":"false";
	str << "<arith iilname=\"" << mIILName
		<< "\" reverse=\"" << od
		<< "\" order=\"" << mOrder
		<< "\" odrattr=\"" << mCtnrObjid
		<< "\" opr=\"" << AosOpr_toStr(mOpr)
		<< "\" value=\"" << mValue; 
		if (mValue2 != "") str << "\" value2=\"" << mValue2; 
		str << "\"/>";
}


AosQueryTermObjPtr
AosTermArithNew::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	AosTermArithNew *ptr = OmnNew AosTermArithNew(*this);
	if (!ptr->config(rdata, def, 0)) return 0;
	return ptr;
}


AosJimoPtr
AosTermArithNew::cloneJimo() const
{
	return OmnNew AosTermArithNew(*this);
}


bool
AosTermArithNew::retrieveQueryFilters(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);

	mFilters.clear();
	AosXmlTagPtr filters_tag = def->getFirstChild("query_filters");
	if (!filters_tag) return true;

	AosXmlTagPtr filter_tag = filters_tag->getFirstChild();
	if (!filter_tag) return true;

	AosQueryFilterObjPtr filter;
	while (filter_tag)
	{
		filter = AosQueryFilter::create(filter_tag, rdata);
		aos_assert_r(filter, false);

		mFilters.push_back(filter);

		filter_tag = filters_tag->getNextChild();
	}

	return true;
}

