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
#ifndef Aos_ValueSel_ValueSelGetCounter_h
#define Aos_ValueSel_ValueSelGetCounter_h

#include "ValueSel/ValueSel.h"
#include "CounterUtil/StatTypes.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"

struct CounterInfo
{
	i64						mStartTime;
	i64						mEndTime;
	AosTimeGran::E			mTimeGran;
	AosTime::TimeFormat		mTimeFormat;
	vector<AosStatType::E>	mStatTypes;
	OmnString				mCounterId;
	OmnString				mCounterName;
	int64_t					mDftValue;
};

class AosValueSelGetCounter : public AosValueSel
{
private:
	OmnString		mValue;
	OmnString		mXPath;
	bool			mCopyDoc;
	AosXmlTagPtr 	mDocSelector;

public:
	AosValueSelGetCounter(const bool reg);
	AosValueSelGetCounter(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	AosValueSelGetCounter(
			const OmnString &value, 
			const OmnString &xpath, 
			const bool copydoc,
			const AosXmlTagPtr &doc_selector);
	~AosValueSelGetCounter();

	virtual bool run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &item,
		const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	bool parse(struct CounterInfo &counterobj, const AosXmlTagPtr &term, const AosRundataPtr &rdata);
					
};
#endif

