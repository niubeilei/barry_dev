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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ValueSel_ValsetMulRanges_h
#define AOS_ValueSel_ValsetMulRanges_h

#include "ValueSel/ValueSel.h"
#include <vector>

using namespace std;

class AosValueSelMulRanges : public AosValueSel
{
private:
	enum
	{
		eMaxRanges = 10
	};

public:
	AosValueSelMulRanges(const bool reg);
	AosValueSelMulRanges(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosValueSelMulRanges();

	// Valuset Interface
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
	bool parse(vector<AosValueRslt> &start, 
			vector<AosValueRslt> &end, 
			vector<AosValueRslt> &step, 
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
};
#endif

