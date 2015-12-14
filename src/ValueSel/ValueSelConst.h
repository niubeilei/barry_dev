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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelConst_h
#define Aos_ValueSel_ValueSelConst_h

#include "ValueSel/ValueSel.h"

class AosValueSelConst : public AosValueSel
{
private:
	OmnString		mValue;
	OmnString		mXPath;
	bool			mCopyDoc;
	AosXmlTagPtr 	mDocSelector;

public:
	AosValueSelConst(const bool reg);
	AosValueSelConst(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	AosValueSelConst(
			const OmnString &value, 
			const OmnString &xpath, 
			const bool copydoc,
			const AosXmlTagPtr &doc_selector);
	~AosValueSelConst();

	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
	virtual OmnString getXmlStr(
				const OmnString &tagname, 
				const int level,
				const AosRandomRulePtr &rule, 
				const AosRundataPtr &rdata);

	static OmnString getXmlStr(
			const OmnString &tagname,
			const int level,
			const OmnString &contents,
			const AosRundataPtr &rdata);
};
#endif

