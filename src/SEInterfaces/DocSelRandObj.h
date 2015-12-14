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
// 01/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocSelRandObj_h
#define Aos_SEInterfaces_DocSelRandObj_h

#include "DocSelector/DocSelectorType.h"
#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosDocSelRandObj : virtual public OmnRCObject
{
private:
	static AosDocSelRandObjPtr		smDocSelector;

public:
	static void setDocSelRand(const AosDocSelRandObjPtr &obj);
	static AosDocSelRandObjPtr getDocSelRand() {return smDocSelector;}

	virtual OmnString pickDocSelector(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata) = 0;

	virtual AosDocSelectorType::E pickSelector() = 0;
};
#endif
