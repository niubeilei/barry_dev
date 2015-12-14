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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CustomizedValueSel_Unicom_UnicomSubstr_h
#define Aos_CustomizedValueSel_Unicom_UnicomSubstr_h

#include "ValueSel/ValueSel.h"

class AosUnicomSubstr : virtual public AosValueSel
{
private:

public:
	AosUnicomSubstr(const bool reg);
	AosUnicomSubstr(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosUnicomSubstr();

	virtual bool run(
					AosValueRslt    &valueRslt,
					const AosXmlTagPtr &item,
					const AosRundataPtr &rdata);

	virtual bool run(
					AosValueRslt &value,
					const char *record, 
					const int record_len, 
					const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	bool config(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif

