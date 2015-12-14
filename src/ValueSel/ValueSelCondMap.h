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
#ifndef Aos_ValueSel_ValueSelCondMap_h
#define Aos_ValueSel_ValueSelCondMap_h

#include "ValueSel/ValueSel.h"
class AosValueSelCondMap : virtual public AosValueSel
{
private:

public:
	AosValueSelCondMap(const bool reg);
	AosValueSelCondMap(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelCondMap();

	virtual bool run(
					AosValueRslt    &valueRslt,
					const AosXmlTagPtr &item,
					const AosRundataPtr &rdata);
	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
};
#endif

