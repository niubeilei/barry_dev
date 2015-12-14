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
// 01/03/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_DocSelStmcByDoc_h
#define AOS_DocSelector_DocSelStmcByDoc_h

#include "DocSelector/DocSelector.h"
#include "SEUtil/Ptrs.h"


class AosDocSelStmcByDoc : public AosDocSelector 
{

public:
	AosDocSelStmcByDoc(const bool reg);
	~AosDocSelStmcByDoc();

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

