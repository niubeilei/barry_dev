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
// This value selector selects the requester.
//
// Modification History:
// 01/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelRequester_h
#define Aos_ValueSel_ValueSelRequester_h

#include "ValueSel/ValueSel.h"

class AosValueSelRequester : public AosValueSel
{
private:

public:
	AosValueSelRequester(const bool reg);
	AosValueSelRequester(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelRequester();

	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata);
	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);
};
#endif

