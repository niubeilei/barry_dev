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
// 11/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "ValueSel/VsUtil.h"

bool 
AosSdocUtil::retrieveAttrValues(
		vector<OmnString> &m_values, 
		const AosXmlTagPtr &working_doc, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &tagname,
		const AosRundataPtr &rdata)
{
	// This function retrieves all attribute values. 
	// The definition of the attributes to be retrieved is the subtag
	// identified by 'tagname'.
	// M values are defined by the subtag:
	// 	<sdoc ...>
	// 		<tagname>
	// 			<valuesel .../>
	// 			<valuesel .../>
	// 			...
	//		</tagname>	
	// 	</sdoc>
	aos_assert_rr(sdoc, rdata, false);
	aos_assert_rr(working_doc, rdata, false);
	aos_assert_rr(tagname != "", rdata, false);
	m_values.clear();

	AosXmlTagPtr subtag = sdoc->getFirstChild(tagname);
	if (!subtag)
	{
		AosSetError(rdata, AosErrmsgId::eSmartDocSyntaxError);
		//OmnAlarm << rdata->getErrmsg() << ". Missing Attribute definition: " << sdoc->toString()
		//	<< enderr;
		rdata->setError() << ". Missing Attribute definition:" << sdoc->toString();	
		return false;
	}

	bool rslt = AosValueSel::retrieveValuesStatic(m_values, subtag, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


void
AosSdocUtil::normalizeSha1(OmnString &objid)
{
	OmnString oo = objid;
	int len = oo.length();
	for (int i = len-1; i >= 0;i--)
	{
		if (oo.data()[i] == '=')
			oo.getBuffer()[i] = '\0';
	}
	objid  = "";
	objid << oo.data();
}
