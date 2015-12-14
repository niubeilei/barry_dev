////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014-11-26 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_PartitionIDMgr_H_
#define Aos_RawSE_PartitionIDMgr_H_

#include "RawSE/MediaType.h"
#include "RawSE/RawFile.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosPartitionIDMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString	mHomeDir;

public:
    AosPartitionIDMgr(AosXmlTagPtr	&conf);
    virtual ~AosPartitionIDMgr();

    u32 pickPartition(
    		AosRundata *rdata,
    		AosMediaType::Type media_type);

    bool getPathByPartitionID(
    		AosRundata				*rdata,
    		const u32				partition_id,
			const AosRawFile::Type	eType,
    		OmnString				&path);

	OmnString getBaseDir();
};


#endif /* Aos_RawSE_PartitionIDMgr_H_ */
