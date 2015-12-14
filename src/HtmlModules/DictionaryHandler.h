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
// 07/16/2010: Created by lynch 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_DictionaryHandler_h
#define Aos_HtmlModules_DictionaryHandler_h

#include "Dictionary/DictionaryMgr.h"
#include "XmlUtil/XmlTag.h"


class AosDictionaryHandler 
{
public:
	AosDictionaryHandler();
	~AosDictionaryHandler() {}

	const static OmnString 
	getDictionary(
		const OmnString &attr, 
		const AosXmlTagPtr &vpd,
		const OmnString &langcode, 
		const AosRundataPtr& rdata);
	const static OmnString 
	matchLangType(const OmnString &language);
};

#endif
