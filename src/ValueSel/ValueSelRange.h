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
// 05/10/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ValueSel_ValsetRange_h
#define AOS_ValueSel_ValsetRange_h

#include "ValueSel/ValueSel.h"
#include "SEUtil/XmlRandObj.h"
#include "SEUtil/Ptrs.h"



class AosValueSelRange : public AosValueSel ,virtual public AosRandXmlObj
{
public:
	AosValueSelRange(const bool reg);
	AosValueSelRange(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosValueSelRange();

	// ValueSel Interface
	virtual bool run(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	// RandXmlObj Interface
	virtual OmnString getXmlStr(
					const OmnString &tagname, 
					const int level,
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);

	virtual OmnString getXmlStr(
					const AosRandomRulePtr &rule, 
					const AosRundataPtr &rdata);
private:
	bool parse(AosValueRslt &start, 
			AosValueRslt &end, 
			AosValueRslt &step, 
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
};
#endif

