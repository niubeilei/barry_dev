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
// 11 Aug 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_MarkerMgr_H_
#define Aos_BlobSE_MarkerMgr_H_
#include "BlobSE/BlobSE.h"
#include "RawSE/Ptrs.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosMarkerMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosRawFilePtr			mMarkerFile;
	AosBuffPtr				mMarkerFileBuff;
	AosMarker::MarkerFile*	mMarkerFileSt;

public:
    AosMarkerMgr(
    		AosRundata	*rdata,
    		AosBlobSE	*blobSE,
			AosRawSE	*rawSE);

    virtual ~AosMarkerMgr();

	int saveMarkerToMarkerFile(
			AosRundata	*rdata,
			AosMarker	&marker);

	int getRecoverStartLocation(
			AosRundata	*rdata,
			u64			&ullSnapshotSavedOprID,
			u64			&ullFlushDirtyHeaderOprID);
};

#endif /* Aos_BlobSE_MarkerMgr_H_ */
