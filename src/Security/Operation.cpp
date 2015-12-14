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
// 12/23/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Security/Operation.h"

#include "Conds/Condition.h"
#include "SEBase/SecReq.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueRslt.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


OmnString 
AosOperation::determineOperation(
		AosSecReq &sec_req,
		const AosRundataPtr &rdata)
{
	// This function determines whether the sec_opr identifies an
	// operation.
	// Operations are defined by the container:
	// 	<container ...>
	// 		<AOSTAG_OPR_DEFS>
	// 			<sec_opr_id>
	// 				<cond ...>
	// 					<value_sel .../>
	// 				<cond .../>
	// 				...
	// 			</sec_opr_id>
	// 			...
	// 		</AOSTAG_OPR_DEFS>
	// 		...
	// 	</container>
	OmnString sec_opr_id = sec_req.getOprStr();
	aos_assert_rr(sec_opr_id != "", rdata, "");
	AosXmlTagPtr container = sec_req.getAccessedParentCtnr(rdata);
	
	// Chen Ding, 2013/04/19
	if (!container)
	{
		// No container for the doc. It simply returns "".
		return "";
	}

	AosXmlTagPtr defs = container->getFirstChild(AOSTAG_OPR_DEFS);
	if (!defs) return "";
	AosXmlTagPtr opr_tag = defs->getFirstChild(sec_opr_id);
	if (!opr_tag) return "";

	AosXmlTagPtr cond = opr_tag->getFirstChild();
	if (!cond) return "";

	int guard = eMaxConds;
	while (guard-- && cond)
	{
		if (AosCondition::evalCondStatic(cond, rdata))
		{
			AosXmlTagPtr value_sel = cond->getFirstChild("value_sel");
			if (value_sel)
			{
				AosValueRslt value;
				bool rslt = AosValueSel::getValueStatic(value, value_sel, rdata);
				if (rslt)
				{
					bool rslt;
					OmnString vv = value.getStr();
					if (rslt && vv != "")
					{
						return vv;
					}
					else
					{
						OmnAlarm << "Invalid data type: " << cond->toString() << enderr;
					}
				}
			}
		}
		cond = opr_tag->getNextChild();
	}

	aos_assert_rr(guard > 0, rdata, "");
	return "";
}

