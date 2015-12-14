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
// 01/11/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conds_CondValueListing_h
#define Aos_Conds_CondValueListing_h

#include "Conds/Condition.h"
#include <vector>
#include "SEUtil/XmlRandObj.h"

class AosCondValueListing : virtual public AosCondition
{

public:
	AosCondValueListing(const bool reg);
	~AosCondValueListing();

	// Condition interface
	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosConditionObjPtr clone();
	virtual OmnString generateCommand(
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata);

	// RandomXml interface. This function should not be in this class.
	// Will move to the torturer.
	OmnString getXmlStr(
					const OmnString &tagname,
					const int level,
					const OmnString &then_part,
					const OmnString &else_part,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

private:
	bool getCid(
				OmnString &cid,
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);
	bool getValueThrSel(
				OmnString &value, 
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);
	bool getUsername(
				OmnString &username,
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);
	bool getUserid(
				OmnString &userid,
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);
};
#endif

