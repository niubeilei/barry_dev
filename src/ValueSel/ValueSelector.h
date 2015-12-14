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
// 08/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_ValueSel_ValueSelector_h
#define Aos_ValueSel_ValueSelector_h

#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "ValueSel/Ptrs.h"
#include "ValueSel/ValueRslt.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/XmlRandObj.h"

class AosValueSelector : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	vector<AosValueSelPtr>	mSelectors;

public:
	AosValueSelector();
	AosValueSelector(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosValueSelector();

	OmnString getValue(const AosXmlTagPtr &data, const AosRundataPtr &rdata);

	bool setConfig(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool setSelectByConstant(const OmnString &value);
	bool setSelectByAttr(const OmnString &aname);

private:
	bool parse(const AosXmlTagPtr &data, const AosRundataPtr &rdata);
};
#endif
#endif
