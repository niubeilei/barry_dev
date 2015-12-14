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
#ifndef AOS_DataGenUtil_DataGenEnumRand_h
#define AOS_DataGenUtil_DataGenEnumRand_h

#include "DataGenUtil/DataGenUtil.h"
#include "SEUtil/Ptrs.h"


class AosDataGenEnumRand : public AosDataGenUtil 
{
private:
	vector<OmnString> mValue;	

public:
	AosDataGenEnumRand(const bool reg);
	AosDataGenEnumRand(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
	AosDataGenEnumRand();
	~AosDataGenEnumRand();

	virtual bool nextValue(
			AosValueRslt &value,
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool nextValue(AosValueRslt &value, const AosRundataPtr &rdata);
	
	virtual AosDataGenUtilPtr clone(const AosXmlTagPtr &config, const AosRundataPtr &rdata);

	bool parse(const AosXmlTagPtr &config, const AosRundataPtr &rdata);
			
};
#endif

