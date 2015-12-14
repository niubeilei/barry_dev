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
// 05/16/2011	Created by Tom
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ValueSel_ValsetPattern_h
#define AOS_ValueSel_ValsetPattern_h

#include "ValueSel/ValueSel.h"
#include <vector>

using namespace std;


class AosValueSelPattern : public AosValueSel
{
private:

public:
	AosValueSelPattern(const bool reg);
	AosValueSelPattern(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosValueSelPattern();

	// ValueSel Interface
	virtual bool run(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);
	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);

private:
	bool parse(
			OmnString &pattern, 
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
};
#endif

