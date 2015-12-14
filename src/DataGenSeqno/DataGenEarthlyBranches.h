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
#ifndef AOS_DataGenSeqno_DataGenEarthlyBranches_h
#define AOS_DataGenSeqno_DataGenEarthlyBranches_h

#include "DataGenSeqno/DataGenSeqno.h"
#include "SEUtil/Ptrs.h"


class AosDataGenEarthlyBranches : public AosDataGenSeqno 
{
	enum
	{
		eMaxNum = 12 
	};

public:
	AosDataGenEarthlyBranches(const bool reg);
	~AosDataGenEarthlyBranches();

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);
			
	OmnString	convert(u64 &num);

	virtual bool createSeqno(
			AosValueRslt &value,
			const AosRundataPtr &rdata);
};
#endif

