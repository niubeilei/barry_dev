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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TimeSelector/TimeAdditive.h"

#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosTimeAdditive::AosTimeAdditive(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_ADD, AosTimeSelectorType::eAdd, reg)
{
}


AosTimeAdditive::~AosTimeAdditive()
{
}


bool  
AosTimeAdditive::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" zky_number = "xxx" AOSTAG_TIMESEL_TYPE="add" zky_datetime="">
	// <AOSTAG_LHS zky_value_type="xxx" zky_data_type="int64">
	// 	...
	// </AOSTAG_LHS>
	// <AOSTAG_RHS zky_value_type="xxxx" zky_data_type="int64">
	// ...
	// </AOSTAG_RHS>
	// <zky_number zky_value_type = "xxxx" zky_data_type="int64">
	// </zky_number>
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		AosSetErrorUser(rdata, "data_type_incorrect") << datatype << enderr;
		return false;
	}

	//The default:Current time
	int64_t lhs_value = 0;
	AosXmlTagPtr tt = sdoc->getFirstChild(AOSTAG_LHS);
	if (tt)
	{
		AosValueRslt lhs;
		if (!AosValueSel::getValueStatic(lhs, tt, rdata))
		{
			// Failed retrieving the value.
			AosSetErrorU(rdata, "time_failed_001") << sdoc->toString() << enderr;
			return false;
		}
		lhs_value = lhs.getI64();
	}

	if (lhs_value == 0)
	{
		//The default:Current time
		time_t lt = time(0);
		lhs_value = lt;
	}

	int64_t rhs_value = 0;
	tt = sdoc->getFirstChild(AOSTAG_RHS);
	if (tt)
	{
		AosValueRslt rhs;
		if (!AosValueSel::getValueStatic(rhs, tt, rdata))
		{
			// Failed retrieving the value.
	    	AosSetErrorU(rdata, "time_failed_002") << sdoc->toString() << enderr;
	     	return false;
		}
		rhs_value = rhs.getI64();
	}

	if (rhs_value == 0)
	{	
		//<sdoc zky_data_type="" zky_number = "xxx" AOSTAG_TIMESEL_TYPE="add" zky_datetime="">
		int64_t num = sdoc->getAttrU64("zky_number", 0);
		if (num == 0)
		{
			AosXmlTagPtr value_def = sdoc->getFirstChild("zky_number");
			aos_assert_r(value_def, false);
			AosValueRslt valueRslt;
			if (!AosValueSel::getValueStatic(valueRslt, value_def, rdata))
			{
				return false;
			}
			num = valueRslt.getI64();	
		}

		//The default: day;
		OmnString datetime = sdoc->getAttrStr("zky_datetime", "d");
		// day to second
		rhs_value = num * 24 * 60 *60;
	}

	OmnString  vv;
	vv << (lhs_value + rhs_value);
	value.setStr(vv);
	return true;
}

