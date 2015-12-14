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
#ifndef AOS_DataGenSeqno_DataGenRomanSeqno_h
#define AOS_DataGenSeqno_DataGenRomanSeqno_h

#include "DataGenSeqno/DataGenSeqno.h"
#include "SEUtil/Ptrs.h"


class AosDataGenRomanSeqno : public AosDataGenSeqno 
{
	enum
	{
		eDftStart = 1,
		eDftStep = 1,
		eMaxRomanNum = 9999
	};
public:
	AosDataGenRomanSeqno(const bool reg);
	~AosDataGenRomanSeqno();

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosXmlTagPtr &doc, 
			const AosRundataPtr &rdata);

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata);
private:
	OmnString	getNumberRoman(u64 &num);
			
};
#endif

