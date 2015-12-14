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
#include "Actions/ActModifyAttr.h"

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

AosActModifyAttr::AosActModifyAttr(const bool flag)
:
AosSdocAction(AOSACTTYPE_MODIFYATTR, AosActionType::eModifyAttr, flag)
{
}


/*
AosActModifyAttr::AosActModifyAttr(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eModifyAttr, false)
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
AosActModifyAttr::config(const AosXmlTagPtr &def)
{
}
*/


AosActModifyAttr::~AosActModifyAttr()
{
}


bool	
AosActModifyAttr::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// The 'def' format is:
	// 	<action zky_type=AOSACTTYPE_MODIFYATTR
	// 		AOSTAG_XPATH="xxx">
	// 		<AOSTAG_VALUESELECTOR .../>
	// 		<AOSTAG_DOCSELECTOR .../>
	// 	</action>
	aos_assert_rr(def, rdata, false);

	OmnString attrname = def->getAttrStr(AOSTAG_XPATH);
	bool value_unique = def->getAttrBool(AOSTAG_VALUE_UNIQUE, false);
	bool docid_unique = def->getAttrBool(AOSTAG_DOCID_UNIQUE, false);
	AosXmlTagPtr target_doc= AosRunDocSelector(rdata, def, AOSTAG_DOCSELECTOR);
	if (!target_doc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingDoc);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}

	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, def, AOSTAG_VALUESELECTOR, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eMissingValue);
		OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
		return false;
	}

	OmnString oldval = target_doc->getAttrStr(attrname);
	OmnString newval = value.getStr();
	if (oldval == newval)
	{
		rdata->setOk();
		return true;
	}

	AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
		target_doc->getAttrU64(AOSTAG_DOCID, 0), 
		target_doc->getAttrStr(AOSTAG_OBJID), 
		attrname, newval, "", value_unique, docid_unique, true);

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActModifyAttr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActModifyAttr(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


