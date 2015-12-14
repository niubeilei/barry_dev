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
// 2013/11/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelCounterKey_h
#define Aos_ValueSel_ValueSelCounterKey_h

#include "ValueSel/ValueSel.h"

class AosValueSelCounterKey : public AosValueSel
{
public:
	AosValueSelCounterKey(const bool reg);
	AosValueSelCounterKey(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelCounterKey();

	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
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

