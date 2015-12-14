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
// 01/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocSelector_DocSelRdataDoc_h
#define AOS_DocSelector_DocSelRdataDoc_h

#include "DocSelector/DocSelector.h"


class AosDocSelRdataDoc : public AosDocSelector 
{

public:
	AosDocSelRdataDoc(const bool reg);
	~AosDocSelRdataDoc();

	virtual AosXmlTagPtr selectDoc(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	virtual OmnString getXmlStr(
			const OmnString &tagname,
			const int level,
			const AosRandomRulePtr &rule,
			const AosRundataPtr &rdata);

	static OmnString getXmlStr(
			const OmnString &tagname, 
			const OmnString &source,
			const OmnString &docname, 
			const AosRundataPtr &rdata);

private:
	static bool init();
	static OmnString pickSource();
};
#endif

