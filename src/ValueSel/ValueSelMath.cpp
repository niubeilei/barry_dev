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
// 2012/03/20	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelMath.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


#if 0 
AosValueSelMath::AosValueSelMath(const bool reg)
:
AosValueSel(AOSACTOPRID_MATH, AosValueSelType::eMath, reg)
{
}


AosValueSelMath::AosValueSelMath(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_MATH, AosValueSelType::eMath, false)
{
}


AosValueSelMath::~AosValueSelMath()
{
}


bool
AosValueSelMath::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata)
{
	// 	<valuesel 
	// 		AOSTAG_VALUE_TYPE="const" 
	// 		AOSTAG_DATA_TYPE="u64|int64">the-attribute-name</name>
	//
	aos_assert_r(item, false);
	aos_assert_r(rdata, false);

	AosXmlTagPtr lhs_tag = item->getFirstChild(AOSTAG_LHS);
	if (!lhs_tag)
	{
		rdata->setError() << "missing lhs";
		return false;
	}
	
	AosValueRslt lhs;
	if (!AosValueSel::getValueStatic(lhs, lhs_tag, rdata))
	{
		rdata->setError() << "missing lhs value";
		return false;
	}
	
	AosXmlTagPtr rhs_tag = item->getFirstChild(AOSTAG_RHS);
	if (!rhs_tag)
	{
		rdata->setError() << "missing rhs";
		return false;
	}

	AosValueRslt rhs;
	if (!AosValueSel::getValueStatic(rhs, rhs_tag, rdata))
	{
		rdata->setError() << "missing rhs value";
		return false;
	}		
	
	AosXmlTagPtr opr = item->getFirstChild(AOSTAG_OPR);
	if (!opr)
	{
		rdata->setError() << "missing opr";
		return false;
	}
	
	AosDataType::E lhs_type = lhs.getType();
	AosDataType::E rhs_type = rhs.getType();
	if(lhs_type != rhs_type || (lhs_type != AosDataType::eU64 && lhs_type != AosDataType::eInt64 && lhs_type != AosDataType::eDouble))
	{
		rdata->setError() << "datatype error";
		return false;
	}

	OmnString oprstr = opr->getNodeText();
	switch (toEnum(oprstr))
	{
	case eAdd :			valueRslt = lhs + rhs; break;
	case eSubtract :	valueRslt = lhs - rhs; break;
	case eMultiply :	valueRslt = lhs * rhs; break;
	case eDivide : 		valueRslt = lhs / rhs; break;
	default:            break;
	}
	
	return true;
}


OmnString 
AosValueSelMath::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
/*
	OmnString xml = "<";
	OmnString tname = getTagnameStatic(tagname, rule);
	xml << tname << " ";
	addHeader(xml, tname, AOSACTOPRID_CONST, rule);

	xml << ">" << AosCommonValues::pickAttrValue()
		<< "</" << tname << ">";
	return  xml;
*/
}


AosValueSelObjPtr
AosValueSelMath::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelMath(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

#endif
