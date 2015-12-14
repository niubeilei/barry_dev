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
#ifndef Aos_Valueset_ValuesetXml_h
#define Aos_Valueset_ValuesetXml_h

#include "Value/Ptrs.h"
#include "Valueset/Valueset.h"

class AosValue;

class AosValuesetXml : public AosValueset
{
	OmnDefineRCObject;

protected:
	OmnString		mAttrname;
	OmnString		mRootTagname;
	OmnString		mEntryTagname;
	AosValuePtr		mValue;
	AosValue *		mValueRaw;
	AosXmlTagPtr	mXmlDoc;
	AosXmlTag *		mReadLoop;
	AosXmlTag *		mWriteLoop;
	AosXmlTagPtr	mRootTag;
	AosXmlTag *		mRootTagRaw;

public:
	AosValuesetXml(const OmnString &version);
	~AosValuesetXml();

	// Jimo Interface
	AosJimoPtr cloneJimo() const;

	// Valueset Interface
	virtual bool resetReadLoop();
	virtual bool resetWriteLoop();
	virtual AosValue *nextValue(const AosRundataPtr &rdata);
	virtual bool setData(
						const AosRundataPtr &rdata, 
						const AosValuePtr &data);
	virtual bool config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc, 
						const AosXmlTagPtr &jimo_doc);

protected:
	virtual void init();
	inline AosXmlTagPtr getFirstChild()
	{
		if (!mReadLoop) return 0;
		if (mEntryTagname == "")
		{
			return mReadLoop->getFirstChild();
		}
		return mReadLoop->getFirstChild(mEntryTagname);
	}
	inline AosXmlTagPtr getNextChild()
	{
		if (!mReadLoop) return 0;
		if (mEntryTagname == "")
		{
			return mReadLoop->getNextChild();
		}
		return mReadLoop->getNextChild(mEntryTagname);
	}
};
#endif

