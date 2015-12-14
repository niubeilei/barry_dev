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
// 01/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_SdocActionObj_h
#define Aos_SdocAction_SdocActionObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosSdocActionObj : virtual public AosActionObj
{
	OmnDefineRCObject;

public:
	AosSdocActionObj();
	~AosSdocActionObj();

	virtual bool runActions(const AosXmlTagPtr &def, 
							const OmnString &tagname,
							const AosRundataPtr &rdata);

	virtual bool runActions( const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	virtual bool runAction( const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	virtual OmnString getActionId(const AosXmlTagPtr &action);

	virtual OmnString pickActions(
						const OmnString &tagname, 
						const int level,
						const AosRandomRulePtr &rule,
						const AosRundataPtr &rdata);
};
#endif

