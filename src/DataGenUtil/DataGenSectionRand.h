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
#ifndef AOS_DataGenUtil_DataGenSectionRand_h
#define AOS_DataGenUtil_DataGenSectionRand_h

#include "DataGenUtil/DataGenUtil.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataGenSectionRand : public AosDataGenUtil 
{
private:	
	enum
	{
		eMax = 100
	};

	vector<int>		mWeights;
	vector<int64_t>	mStart;
	vector<int64_t>	mEnd;

public:
	AosDataGenSectionRand(const bool reg);
	AosDataGenSectionRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	AosDataGenSectionRand();
	~AosDataGenSectionRand();

	virtual bool nextValue(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool nextValue(AosValueRslt &value, const AosRundataPtr &rdata);

	bool	parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
			
	int	procWeights(const AosXmlTagPtr &sdoc);

	virtual AosDataGenUtilPtr clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
};
#endif

