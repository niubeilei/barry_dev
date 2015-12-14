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
#ifndef AOS_SeLogSvr_LogAging_h
#define AOS_SeLogSvr_LogAging_h

#include "SEUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransServer/TransProc.h"
#include "XmlUtil/XmlTag.h"
#include "SeLogSvr/Ptrs.h" 

#define AosSeLogAgingCap   "agecap"
#define AosSeLogAgingRcd   "agercd"

class AosLogAging : virtual public OmnRCObject
{
public:
	enum AgingType
	{
		eInvalidAging,

		eCapacityAging,
		eNumRecordsAging,

		eMaxAging
	};

protected:
	AgingType	mAgingType;

public:
	AosLogAging(const AgingType type);
	~AosLogAging();

	virtual bool isFull(
					const u32 crt_pos,
					const AosSeLogPtr &selog,
					const AosRundataPtr &rdata) = 0;
	virtual AosLogAgingPtr clone() = 0;

	static AosLogAgingPtr getAging(AosBuff &buff);
	static AosLogAgingPtr getAging(const AosXmlTagPtr &ageconfig,
						const AosRundataPtr &rdata);
	static AgingType agingToEnum(const OmnString &type, const AosRundataPtr &rdata);
	virtual bool setContentsToBuff(AosBuff &buff) = 0;
};
#endif
