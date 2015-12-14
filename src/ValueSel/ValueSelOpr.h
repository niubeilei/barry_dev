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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_ValueSel_ValueSelValueSel_h
#define Aos_ValueSel_ValueSelValueSel_h

#include "ValueSel/ValueSel.h"

class AosValueSelValueSel : virtual public AosValueSel
{

private:

public:
	AosValueSelValueSel(const bool reg);
	~AosValueSelValueSel();


	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata);

	virtual bool getValue(
				AosValueRslt &valueRslt, 
				const AosXmlTagPtr &obj,
				const AosRundataPtr &rdata);

	virtual AosValueSelPtr createValueSelector(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

	virtual AosValueSelPtr clone();

	// RandXmlObj Interface
	virtual OmnString getXml(const AosRandomRulePtr &rule);
};
#endif
#endif
