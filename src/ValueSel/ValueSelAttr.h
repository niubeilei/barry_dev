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
#ifndef Aos_ValueSel_ValueSelAttr_h
#define Aos_ValueSel_ValueSelAttr_h

#include "ValueSel/ValueSel.h"

class AosValueSelAttr : public AosValueSel
{
private:
	AosDataType::E 	mDataType;
	AosXmlTagPtr	mDocSelector;
	OmnString		mXPath;
	OmnString		mDefault;

public:
	AosValueSelAttr(const bool reg);
	AosValueSelAttr(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	AosValueSelAttr(
			const AosDataType::E datatype,
			const AosXmlTagPtr &doc_selector,
			const OmnString &xpath, 
			const OmnString &dft);
	~AosValueSelAttr();

	virtual bool run(
		AosValueRslt &valueRslt,
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
						const OmnString &datatype,
						const OmnString &xpath,
						const OmnString &docsel,
						const OmnString &dft,
						const AosRundataPtr &rdata);
};
#endif

