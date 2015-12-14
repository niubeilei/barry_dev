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
// 12/27/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelByUserArgs_h
#define Aos_ValueSel_ValueSelByUserArgs_h

#include "ValueSel/ValueSel.h"

class AosValueSelByUserArgs : public AosValueSel
{
private:

public:
	AosValueSelByUserArgs(const bool reg);
	AosValueSelByUserArgs(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelByUserArgs();

	virtual bool run(
					AosValueRslt &valueRslt,
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

