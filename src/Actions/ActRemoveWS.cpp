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
// 06/21/2012	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRemoveWS.h"

#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelector.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include <string>
#include <vector>
using namespace std;

AosActRemoveWS::AosActRemoveWS(const bool flag)
:
AosSdocAction(AOSACTTYPE_REMOVEWS, AosActionType::eRemoveWS, flag)
{
}


AosActRemoveWS::~AosActRemoveWS()
{
}


bool	
AosActRemoveWS::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This function should remove the white space user wanted.
	// the white space should be in the front or end of a attribute.
	// 1.  The first, we should retrieve the doc need to modify.
	// 2.  The second, retrieve the attribute's name.
	// 3.  retrieve the value of attribute.
	// 4.  depend on the configuration, remove the white space.
	// 5.  modify the doc's attribute to value modified.
	//
	// The 'def' format is:
	// 	<action zky_type="rmvws" zky_xpath="xxx" rmtype="xxx">
	// 		<docselector />
	// 	</action>
	aos_assert_rr(def, rdata, false);
	
	//  1. retrieve the doc.
	AosXmlTagPtr target_doc= AosRunDocSelector(rdata, def, AOSTAG_DOCSELECTOR);
	if (!target_doc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingDoc);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}
	
	
	//  2. retrieve the attribute's value.
	OmnString path = def->getAttrStr(AOSTAG_XPATH);
	if (path == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingValue);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}

	OmnString value = target_doc->xpathQuery(path);
	if (value == "")
	{
		rdata->setError() << "Failed to retrieve the value configed by user";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString oriValue = value;

	//  3.  remove the white space.
	OmnString type = def->getAttrStr(AOSTAG_RMTYPE);

	E t = toEnum(type);
	if (!isTrueType(t))
	{
		AosSetError(rdata, AosErrmsgId::eMissingType);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}

	switch (t)
	{
	case eLeading:
		value.removeLeadingWhiteSpace();
		break;
	case eTail:
		value.removeTailWhiteSpace();
		break;
	case eAll:
		value.removeWhiteSpaces();
		break;
	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}

	if (value == "")
	{
		rdata->setError() << "To remove white space, but the result of value is empty!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (oriValue == value) return true;
OmnScreen << "Modify: " << target_doc->getAttrStr(AOSTAG_OBJID) << endl;


	// modify the doc
	bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
			    target_doc->getAttrU64(AOSTAG_DOCID, 0), 
				target_doc->getAttrStr(AOSTAG_OBJID), 
				path, value, "", true, true, true);
	if (!rslt)
	{
		rdata->setError() << "Failed to modify the doc";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


AosActRemoveWS::E
AosActRemoveWS::toEnum(const OmnString &type)
{
	aos_assert_r(type != "", eInvalid);
	if (type == "leading") return eLeading;
	if (type == "tail") return eTail;
	if (type == "all") return eAll;
	OmnAlarm << "Undefined type" << enderr;
	return eInvalid;
}


bool
AosActRemoveWS::isTrueType(const E &t)
{
	return ((eInvalid < t) && (t < eMax));
}


AosActionObjPtr
AosActRemoveWS::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRemoveWS(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

