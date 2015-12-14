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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActModifyRundataAttr.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

AosActModifyRundataAttr::AosActModifyRundataAttr(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYRUNDATAATTR, AosActionType::eModifyRundataAttr, flag)
{
}


/*
AosActModifyRundataAttr::AosActModifyRundataAttr(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eModifyRundataAttr, false)
{
	if (!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Missing the doc tag"));
		throw e;
	}
}
*/


/*
bool
AosActModifyRundataAttr::config(const AosXmlTagPtr &def)
{
	// The 'def' should be in the form:
	// 	<action type="addattr">
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		<attr namebd="xxx" attrname="xxx"/>
	// 		...
	// 	</action>
	if (!def)
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Definition is null"));
		throw e;
	}

	AosXmlTagPtr attr = def->getFirstChild();
	if (!attr)
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
			OmnString("Missing attr tag"));
		throw e;
	}

	while (attr)
	{
		OmnString name = attr->getAttrStr(AOSTAG_NAME_BIND);	// join .. namebd

		if (name == "")
		{
			OmnAlarm << "Attribute name binding is empty!" << enderr;
		}
		else
		{
			mNames.push_back(name);
			OmnString attrname = attr->getAttrStr(AOSTAG_ATTRNAME);
			if (attrname == "") 
			{
				mAttrNames.push_back(name);
			}
			else
			{
				mAttrNames.push_back(attrname);
			}
		}

		attr = def->getNextChild();
	}
	return true;
}
*/


AosActModifyRundataAttr::~AosActModifyRundataAttr()
{
}


bool	
AosActModifyRundataAttr::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// It add a atribute into rdata
	aos_assert_r(def, false);

	// 1. Get attr_name
	OmnString attr_name = def->getAttrStr(AOSTAG_ATTR_NAME);
	aos_assert_r(attr_name != "", false);

	// 2. Get attr_value
	OmnString attr_value = def->getAttrStr(AOSTAG_ATTR_VALUE);
	aos_assert_r(attr_value != "", false);

	// 3. set attr_name
	rdata->setArg1(attr_name, attr_value);
	return true;
}


AosActionObjPtr
AosActModifyRundataAttr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyRundataAttr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


