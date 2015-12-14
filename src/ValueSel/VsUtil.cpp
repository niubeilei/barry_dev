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
// 11/10/2011	Created by Tom
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/VsUtil.h"

#include "ValueSel/ValueSel.h"



// Chen Ding, 12/30/2011
// Moved to AosValueSel
/* 
bool 
AosValueSelUtil::retrieveValues(
		vector<OmnString> &m_values,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function retrieves values based on 'sdoc', which is in the form:
	// 	<sdoc ...>
	// 		<valuesel .../>
	// 		<valuesel .../>
	// 		...
	// 	</sdoc>
	m_values.clear();
	aos_assert_rr(sdoc, rdata, false);
	AosXmlTagPtr def = sdoc->getFirstChild();
	while (def)
	{
		AosValueRslt value;
		bool rslt = AosValueSel::getValueStatic(value, def, rdata);
		aos_assert_rr(rslt, rdata, false);
		m_values.push_back(value.getStrValue(rdata));
		def = sdoc->getNextChild();
	}
	return true;
}
*/
