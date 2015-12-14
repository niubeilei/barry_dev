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
// 2013/09/16	Created by ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelStrMap_h
#define Aos_ValueSel_ValueSelStrMap_h

#include "ValueSel/ValueSel.h"


class AosValueSelStrMap : public AosValueSel
{
public:
	AosValueSelStrMap(const bool reg);
	AosValueSelStrMap(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelStrMap();

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

