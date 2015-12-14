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
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataGenSeqno_DataGenArabicSeqno_h
#define AOS_DataGenSeqno_DataGenArabicSeqno_h

#include "DataGenSeqno/DataGenSeqno.h"
#include "SEUtil/Ptrs.h"


class AosDataGenArabicSeqno : public AosDataGenSeqno 
{
	enum
	{
		eDftStep = 1,
		eDftStart = 0
	};

private:
	u64 		mStart;
	u64 		mEnd;
	u64 		mStep;
	u64 		mSeqnovalue;
	OmnString 	mPrefix;
	OmnString 	mPostfix;
	u64		 	mLength;
	OmnString 	mPadding;


public:
	AosDataGenArabicSeqno(const bool reg);
	AosDataGenArabicSeqno(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	~AosDataGenArabicSeqno();

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata);

	bool	parse(
			const AosXmlTagPtr &config,
			const AosRundataPtr &rdata);
};
#endif

