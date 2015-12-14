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
// 2013/12/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Valueset_ValuesetCSV_h
#define Aos_Valueset_ValuesetCSV_h

#include "Value/Ptrs.h"
#include "Valueset/Valueset.h"

class AosValue;

class AosValuesetCSV : public AosValueset
{
	OmnDefineRCObject;

protected:
	char				mQuote;
	char				mSeparator;
	AosValuePtr			mValue;
	AosValue *			mValueRaw;
	OmnString			mData;
	char *				mRawData;
	int					mDataLen;
	int					mReadIdx;
	int					mWriteIdx;
}


public:
	AosValuesetCSV(const OmnString &version);
	~AosValuesetCSV();

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
};
#endif

