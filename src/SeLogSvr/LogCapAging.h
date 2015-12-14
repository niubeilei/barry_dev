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
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_LogCapAging_h
#define AOS_SeLogSvr_LogCapAging_h

#include "SEUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SeLogSvr/Ptrs.h" 
#include "SeLogSvr/LogAging.h"
#include "Util/Buff.h"

class AosLogCapAging : public AosLogAging
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftMaxNumFiles = 10,
		eDftFileSize = 2000000000		// 2G 
	};

	u32		mMaxFileSize;
	int		mMaxNumFiles;

public:
	AosLogCapAging(
			const u32 maxfilesize,
			const int maxfiles,
			const AosRundataPtr &rdata);
	AosLogCapAging(AosBuff &buff);
	AosLogCapAging(
			const AosXmlTagPtr &ageconfig,
			const AosRundataPtr &rdata);
	~AosLogCapAging();

	virtual bool isFull(
			const u32 crt_pos, 
			const AosSeLogPtr &selog,
			const AosRundataPtr &rdata);

	virtual AosLogAgingPtr clone();

	bool setContentsToBuff(AosBuff &buff)
	{
		buff.setChar((char)eCapacityAging);
		buff.setU32(mMaxFileSize);
		buff.setInt(mMaxNumFiles);
		return true;
	}
};
#endif
