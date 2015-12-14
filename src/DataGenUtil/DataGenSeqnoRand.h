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
#ifndef AOS_DataGenUtil_DataGenSeqnoRand_h
#define AOS_DataGenUtil_DataGenSeqnoRand_h

#include "DataGenUtil/DataGenUtil.h"
#include "DataGenSeqno/Ptrs.h"
//#include "DataGenSeqno/DataGenSeqno.h"
#include "SEUtil/Ptrs.h"


class AosDataGenSeqnoRand : public AosDataGenUtil 
{
	enum
	{
		eMax = 100
	};
private:
	AosDataGenSeqnoPtr	 mDataGenSeqno;

public:
	AosDataGenSeqnoRand(const bool reg);
	AosDataGenSeqnoRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	AosDataGenSeqnoRand();
	~AosDataGenSeqnoRand();

	virtual bool nextValue(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool nextValue(AosValueRslt &value, const AosRundataPtr &rdata);
	
	virtual AosDataGenUtilPtr clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata);

	bool	parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
			
};
#endif

