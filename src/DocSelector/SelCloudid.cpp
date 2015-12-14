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
#include "DocSelector/SelCloudid.h"

#include "SEInterfaces/DocClientObj.h"
#include "DocSelector/DocSelectorType.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServer.h"
#include "ValueSel/ValueSel.h"

AosDocSelCloudid::AosDocSelCloudid(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_CLOUDID, AosDocSelectorType::eCloudid, reg)
{
}


AosDocSelCloudid::~AosDocSelCloudid()
{
}


AosXmlTagPtr 
AosDocSelCloudid::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the doc identified by a cloudid specified in 'sdoc'.
	// format : 
	//	1.const:
	// 			<zky_docselector zky_docselector_type="cloudid" AOSTAG_DOC_CLOUDID ="" >
	// 				<zky_valuedef zky_value_type ="const" zky_xpath="usr_objid">
	// 					the-attribute-name
	// 				</zky_valuedef>
	// 			</zky_docselector>
	// 	2.variable:
	// 			<zky_docselector zky_docselector_type="cloudid">
	// 				<zky_valuedef zky_value_type ="attr" zky_xpath="usr_objid">
	// 					<zky_docselector zky_docselector_type="retrieveddoc"/>
	// 				</zky_valuedef>
	// 			</zky_docselector>
	aos_assert_r(false, NULL);
	return NULL;
#if 0
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	u32 siteid = rdata->getSiteid();
	if (siteid == 0)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSiteid);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnString cloudid= sdoc->getAttrStr(AOSTAG_DOC_CLOUDID);
	if (cloudid== "") 
	{
		// Retrieve the value
		AosXmlTagPtr value_def = sdoc->xpathGetChild(AOSTAG_VALUEDEF);
		if (!value_def)
		{
			AosSetError(rdata, AosErrmsgId::eMissingValueSelector);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		AosValueRslt value;
		if (!AosValueSel::getValueStatic(value, value_def, rdata))
		{
			return 0;
		}

		// The data type cannot be XML DOC
	
		bool rslt;
		cloudid = value.getValueStr("", rslt);
		if (!rslt)
		{
			AosSetErrorUser(rdata, "data_type_error") << value.getTypeStr() << enderr;
			return 0;
		}
	}
		
	if (cloudid == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingCloudid);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return AosDocClientObj::getDocClient()->getDocByCloudid(cloudid, rdata);
#endif
}


OmnString 
AosDocSelCloudid::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

