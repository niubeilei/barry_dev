////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is a utility to select docs.
//
// Modification History:
// 01/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_Torturer_DocSelRandObj_h
#define AOS_DocSelector_Torturer_DocSelRandObj_h

#include "Alarm/Alarm.h"
#include "DocSelector/DocSelectorType.h"
#include "Random/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocSelRandObj.h"
#include "Util/String.h"

class AosDocSelRandUtil : public AosDocSelRandObj
{
	OmnDefineRCObject;

public:
	AosDocSelRandUtil();
	~AosDocSelRandUtil();
	enum
	{
		eMaxDocsPerCtnr
	};

	virtual OmnString pickDocSelector(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule,
					const AosRundataPtr &rdata);

	virtual AosDocSelectorType::E pickSelector();
};
#endif

