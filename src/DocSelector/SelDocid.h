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
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_DocSelDocid_h
#define AOS_DocSelector_DocSelDocid_h

#include "DocSelector/DocSelector.h"
#include "XmlUtil/XmlTag.h"


class AosDocSelDocid : public AosDocSelector 
{

public:
	AosDocSelDocid(const bool reg);
	~AosDocSelDocid();

	virtual AosXmlTagPtr selectDoc(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	virtual OmnString getXmlStr(
			const OmnString &tagname,
			const int level,
			const AosRandomRulePtr &rule,
			const AosRundataPtr &rdata);
};
#endif

