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
// 01/18/2012 Moved from Util/ by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Random/CommonValues.h"

AosCommonValues::ValueGroup AosCommonValues::smAttrnames("Attr");
AosCommonValues::ValueGroup AosCommonValues::smAttrvalues("Value");
AosCommonValues::ValueGroup AosCommonValues::smDocNames("Doc");
AosCommonValues::ValueGroup AosCommonValues::smOprCodes("Operation");
AosCommonValues::ValueGroup AosCommonValues::smUserGroups("Role");
AosCommonValues::ValueGroup AosCommonValues::smUserRoles("Role");
AosCommonValues::ValueGroup AosCommonValues::smObjids("Objid");
AosCommonValues::ValueGroup AosCommonValues::smUserDomains("UserDomain");
AosCommonValues::ValueGroup AosCommonValues::smXpaths("Attr");
AosCommonValues::ValueGroup AosCommonValues::smOperations("Operation");

OmnString
AosCommonValues::pickXpath()
{
	OmnString str;
	switch (OmnRandom::percent(70, 20, 10, 1))
	{
	case 0:
		 return smXpaths.pickValue();

	case 1:
		 str = smXpaths.pickValue();
		 str << "/" << smXpaths.pickValue();
		 return str;

	case 2:
		 str = smXpaths.pickValue();
		 str << "/" << smXpaths.pickValue() 
			 << "/" << smXpaths.pickValue();
		 return str;

	case 3:
		 str = smXpaths.pickValue();
		 str << "/" << smXpaths.pickValue() 
			 << "/" << smXpaths.pickValue()
			 << "/" << smXpaths.pickValue();
		 return str;

	default:
		 break;
	}

	str = smXpaths.pickValue();
	str << "/" << smXpaths.pickValue() 
		<< "/" << smXpaths.pickValue()
		<< "/" << smXpaths.pickValue()
		<< "/" << smXpaths.pickValue();
	return str;
}

