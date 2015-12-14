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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_SmartObj_h
#define Aos_SmartObj_SmartObj_h

#include "Rundata/Ptrs.h"
#include "SmartObj/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosValue;

class AosSmartObj : public OmnRCObject
{

private:
	OmnString 		mName;

public:
	AosSmartObj(const OmnString &name, const bool flag);
	~AosSmartObj();


	virtual AosSmartObjPtr clone() = 0; 
	virtual bool run(
					const int command, 
					AosValue &value,
					const AosXmlTagPtr &parms, 
					const AosRundataPtr &rdata) = 0;

	AosSmartObjPtr getSmartObj(const OmnString &name);

private:
	bool registerSmartObj(
					const AosSmartObjPtr &sobj, 
					const OmnString &name, 
					OmnString &errmsg);
};

#endif

