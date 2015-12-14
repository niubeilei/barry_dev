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
// 2014-12-26 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_RawfidRange_H_
#define Aos_BlobSE_RawfidRange_H_

enum AosRawfidRange
{
	//randomly picked, change it if needed
	eHeaderFileIDCount			= 100000,
	eOldHeaderFileIDCount		= 30000000000,
	eOHFIDsReservedForEachUnit	= 10,				//old header file IDs reserved for each day
	eHeaderChangeLogFileIDCount	= 100000,
	eDeletionLogFileIDCount		= 100000,
	eActiveLogFileIDCount		= 100000000,
	eCompactedFileIDCount		= 100000,
	eSnapshotLogFileIDCount		= 100000,
	eOprIDFileIDCount			= 18446744073710,	//max(u64) / eOprIDsPerFile, please refer to OprIDCache.h
	eOprIDBucketFileIDCount		= 0xFFFFFFFF,
	eHeaderBucketFileIDCount	= 0xFFFFFFFF,

	eSnapshotMgrFileID1			= 51,				// Ketty 2015/03/31
	eSnapshotMgrFileID2			= 52,				// Ketty 2015/03/31
	ePendingDeletionFileID		= 53,
	eMarkerFileID				= 95,
	eTempOldHeaderFileID		= 96,
	eMetaFile0ID				= 97,				//active opr id file id
	eMetaFile1ID				= 98,				//compacted file id
	eMetaFile2ID				= 99,				//system first start timestamp(u64)
	eMetaFile3ID				= 100,				//active log file id
	eMetaFile4ID				= 101,				//last old header file for flushing and working
	eHeaderFileID_start			= eMetaFile4ID + 1,													//102
	eHeaderFileID_end			= eHeaderFileID_start + eHeaderFileIDCount - 1,						//100101
	eOldHeaderFileID_start		= eHeaderFileID_end + 1,											//100102
	eOldHeaderFileID_end		= eOldHeaderFileID_start + eOldHeaderFileIDCount - 1,				//30000100101
	eHeaderChangLogFileID_start	= eOldHeaderFileID_end + 1,											//30000100102
	eHeaderChangLogFileID_end	= eHeaderChangLogFileID_start + eHeaderChangeLogFileIDCount - 1,	//30000200101
	eDeletionLogFileID_start	= eHeaderChangLogFileID_end + 1,									//30000200101
	eDeletionLogFileID_end		= eDeletionLogFileID_start + eDeletionLogFileIDCount - 1,			//30000300101
	eActiveLogFileID_start		= eDeletionLogFileID_end + 1,										//30000300102
	eActiveLogFileID_end		= eActiveLogFileID_start + eActiveLogFileIDCount - 1,				//30100300101
	eComopactedFileID_start		= eActiveLogFileID_end + 1,											//30100300102
	eComopactedFileID_end		= eComopactedFileID_start + eCompactedFileIDCount - 1,				//30100400101
	eSnapshotLogFileID_start	= eComopactedFileID_end + 1,										//30100400102 Ketty 2015/03/31
	eSnapshotLogFileID_end		= eSnapshotLogFileID_start + eSnapshotLogFileIDCount - 1,			//30100500101 Ketty 2015/03/31
	eOprIDFileID_start			= eSnapshotLogFileID_end + 1,										//30100500102
	eOprIDFileID_end			= eOprIDFileID_start + eOprIDFileIDCount - 1,						//18476844573811
	eOprIDBucketFileID_start	= eOprIDFileID_end + 1,												//18476844573812
	eOprIDBucketFileID_end		= eOprIDBucketFileID_start + eOprIDBucketFileIDCount - 1,			//18481139541106
	eHeaderBucketFileID_start	= eOprIDBucketFileID_end + 1,										//18481139541107
	eHeaderBucketFileID_end		= eHeaderBucketFileID_start + eHeaderBucketFileIDCount - 1, 		//18485434508401
};

#endif /* Aos_BlobSE_RawfidRange_H_ */
