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
//
// Modification History:
// 01/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StAccessRole.h"

#include "Random/CommonValues.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"

/*
bool 
AosStAccessRole::checkAccess(
		const AosStDocPtr &local_doc,
		bool &granted, 
		bool &denied,
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether the requester has common roles
	// with this class.
	if (!thread->isSameDomain(local_doc))
	{
		denied = true;
		granted = false;
		return true;
	}

	denied = false;
	granted = true;
	return true;
}
*/


bool 
AosStAccessRole::checkAccessByOpr(
		const AosStDocPtr &local_doc,
		const AosXmlTagPtr &def,
		bool &granted, 
		bool &denied,
		const AosSengTestThrdPtr &thread)
{
	// This function checks whether the requester has common roles
	// with this class.
	aos_assert_r(def, false);
	if (!thread->isSameDomain(local_doc))
	{
		denied = def->getAttrBool(AOSTAG_DENY_ACCESS, false);
		granted = false;
		return true;
	}

	OmnString roles = thread->getRequesterRoles();
	OmnString defined_roles = def->getNodeText();
	if (AosStrHasCommonWords(roles, defined_roles))
	{
		granted = true;
		denied = false;
		return true;
	}

	granted = false;
	denied = def->getAttrBool(AOSTAG_DENY_ACCESS, false);
	return true;
}


OmnString
AosStAccessRole::getXmlStr(
		const OmnString &tagname, 
		const AosSengTestThrdPtr &thread)
{
	aos_assert_r(tagname != "", "");
	OmnString docstr = "<";
	OmnString userroles = AosCommonValues::pickUserRoles(",");
	OmnScreen << "Userroles: " << userroles << endl;
	docstr << tagname << " " << AOSTAG_TYPE << "=\"" << AOSACCESSTYPE_ROLE << "\">"
		<< userroles << "</" << tagname << ">";
	return docstr;
}

