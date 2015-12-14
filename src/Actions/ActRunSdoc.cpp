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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActRunSdoc.h"


#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SMDMgr.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
using namespace std;

AosActRunSdoc::AosActRunSdoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_RUNSDOC, AosActionType::eRunSdoc, flag)
{
}


AosActRunSdoc::~AosActRunSdoc()
{
}


bool	
AosActRunSdoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<action 
	// 		<docid .../>
	// 		<objid .../>
	// 		<sdoc .../>
	// 	</action>
	
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt value;
	bool rslt;
	AosXmlTagPtr xml = sdoc->getFirstChild("docid");
	if(xml)
	{	
		rslt = AosValueSel::getValueStatic(value, xml, rdata);
		aos_assert_rr(rslt, rdata, false);
		
		u64 docid;
		if (!value.getU64()) return false;
		aos_assert_rr(docid, rdata, false);
			
		return AosSmartDocObj::procSmartdocStatic(docid, rdata);
	}

	xml = sdoc->getFirstChild("objid");
	if(xml)
	{
		rslt = AosValueSel::getValueStatic(value, xml, rdata);
		aos_assert_rr(rslt, rdata, false);
		
		OmnString objid = value.getStr();
		aos_assert_rr(rslt && objid != "", rdata, false);
			
		return AosSmartDocObj::procSmartdocsStatic(objid, rdata);	
	}

	xml = sdoc->getFirstChild("sdoc");
	aos_assert_rr(xml, rdata, false);

	return AosSmartDocObj::runSmartdocStatic(xml, rdata); 
}


AosActionObjPtr
AosActRunSdoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActRunSdoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


