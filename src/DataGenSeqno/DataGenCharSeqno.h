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
#ifndef AOS_DataGenSeqno_DataGenCharSeqno_h
#define AOS_DataGenSeqno_DataGenCharSeqno_h

#include "DataGenSeqno/DataGenSeqno.h"
#include "SEUtil/Ptrs.h"


class AosDataGenCharSeqno : public AosDataGenSeqno 
{
	enum
	{
		eDftStart = 1,
		eCharMaxNum = 625
	};

public:
	AosDataGenCharSeqno(const bool reg);
	~AosDataGenCharSeqno();

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	OmnString getNumberChar(u64 &num);

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata);
			
};
#endif

