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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Valueset_ValuesetXmlTagSel_h
#define Aos_Valueset_ValuesetXmlTagSel_h

#include "Valueset/ValuesetXml.h"


class AosValuesetXmlTagSel : public AosValuesetXml
{
	OmnDefineRCObject;

protected:
	AosExprObjPtr	mExpr;
	OmnString		mEntryTagname;

public:
	AosValuesetXmlTagSel(const OmnString &version);
	~AosValuesetXmlTagSel();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// Valueset Interface
	virtual AosValue *nextValue(const AosRundataPtr &rdata);
	virtual bool config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc, 
						const AosXmlTagPtr &jimo_doc);

private:
	virtual void init();
};
#endif

