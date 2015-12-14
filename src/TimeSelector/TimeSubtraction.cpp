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
#include "TimeSelector/TimeSubtraction.h"

#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosTimeSubtraction::AosTimeSubtraction(const bool reg)
:
AosTimeSelector(AOSTIMESELTYPE_SUB, AosTimeSelectorType::eSub, reg)
{
}


AosTimeSubtraction::~AosTimeSubtraction()
{
}


bool  
AosTimeSubtraction::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//Time difference
	aos_assert_r(sdoc, false);
	//<sdoc zky_data_type="" zky_number = "xxx" zky_timesel_type="add" output_type="y|m|d|H|M|S|w">
	// <AOSTAG_LHS zky_value_type="xxx" zky_data_type="int64">
	// 	...
	// </AOSTAG_LHS>
	// <AOSTAG_RHS zky_value_type="xxxx" zky_data_type="int64">
	// ...
	// </AOSTAG_RHS>
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

	OmnString output_type= sdoc->getAttrStr("output_type", "S");

	//The default:Current time
	int64_t lhs_value = 0;
	AosXmlTagPtr tt = sdoc->getFirstChild(AOSTAG_LHS);
	if (tt)
	{
		AosValueRslt lhs;
		if (!AosValueSel::getValueStatic(lhs, tt, rdata))
		{
			// Failed retrieving the value.
			AosSetErrorU(rdata, "time_failed_001") << sdoc->toString();
			return false;
		}
		lhs_value = lhs.getI64();
	}

	time_t lt = time(0);
	if (lhs_value == 0)
	{
		//The default:Current time
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
	    	AosSetErrorU(rdata, "time_failed_002") << sdoc->toString();
	     	return false;
		}
		rhs_value = rhs.getI64();
	}

	if (rhs_value == 0)
	{
		rhs_value = lt;
	}
	
	int second = lhs_value - rhs_value;
	// proc output string
	OmnString  vv;
	char c = output_type.data()[0];
	switch (c)
	{
	case 'y':
		 OmnNotImplementedYet;
		 break;

	case 'm':
		 OmnNotImplementedYet;
		 break;

	case 'w':
		 OmnNotImplementedYet;
		 break;

	case 'd':
		 vv << second/60/60/24;
		 break;

	case 'H':
		 vv << second/60/60;
		 break;

	case 'M':
		 vv << second/60;
		 break;

	case 'S':
		 vv << second;
		 break;

	default:	
		 vv = "";
	}

	//vv << (lhs_value - rhs_value);
	value.setStr(vv);
	return true;
}

