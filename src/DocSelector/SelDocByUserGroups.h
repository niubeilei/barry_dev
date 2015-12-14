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
// Modification History:
// 01/04/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_DocSelDocByUserGroups_h
#define AOS_DocSelector_DocSelDocByUserGroups_h

#include "DocSelector/DocSelector.h"
#include "SEUtil/Ptrs.h"


class AosDocSelDocByUserGroups : public AosDocSelector 
{

public:
	AosDocSelDocByUserGroups(const bool reg);
	~AosDocSelDocByUserGroups();

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

