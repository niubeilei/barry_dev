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
// 01/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelCompose_h
#define Aos_ValueSel_ValueSelCompose_h

#include "ValueSel/ValueSel.h"

class AosValueSelCompose : public AosValueSel
{
private:
	enum
	{
		eMaxSegments = 500
	};

public:
	AosValueSelCompose(const bool reg);
	AosValueSelCompose(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelCompose();

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
	static OmnString getXmlStr(
						const OmnString &tagname,
						const int level,
						const OmnString &selectors,
						const OmnString &sep, 
						const AosRundataPtr &rdata);
};
#endif

