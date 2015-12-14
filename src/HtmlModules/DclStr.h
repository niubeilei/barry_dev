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
//   
//
// Modification History:
// 07/20/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_DclStr_h
#define Aos_HtmlModules_DclStr_h

#include "HtmlModules/DataCol.h"


class AosDclStr : public AosDataCol
{
public:
	AosDclStr();
	~AosDclStr() {}

	virtual OmnString getJsonConfig(const AosXmlTagPtr &vpd);

	AosXmlTagPtr 
	retrieveData(
		const u32 siteid,
		const AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &ssid,
		const OmnString &tagname);
};

#endif
