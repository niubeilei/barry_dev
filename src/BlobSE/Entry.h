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
#ifndef Aos_BlobSE_Entry_H_
#define Aos_BlobSE_Entry_H_

#include "aosUtil/Types.h"

#pragma pack(1)
enum EntryType
{
	eInvalid				= 0,
	eAddModify				= 1,
	eDelete					= 2,
	eHCLDeltaStartToSave	= 3,
	eHCLDeltaSaved			= 4,
	eHeaderStartToSave		= 5,
	eHeaderSaved			= 6,
	eCompacted				= 7,
};

//TODO:these structures should be nested and organized, wish that i would do it someday
struct BodyEntryGeneralHeader
{
	u32	ulLength;
	u8	ucType;
};

struct BodyEntryHead
{
	u32	ulLength;
	u8	ucType;
	u64	ullOprID;
	u64	ullDocid;
	u64	ullSnapID;
};

struct DeletionLogEntry
{
	u64	ullRawfid;
	u64	ullOffset;
	u64	ullLength;
};

struct DeleteEntry
{
	u32	ulLength;
	u8	ucType;
	u64	ullOprID;
	u64	ullDocid;
	u64	ullSnapID;
	u64	ullTimestamp;
	u32	ulLengthTail;
};

struct SaveHeaderChangeLogEntry
{
	u32	ulLength;
	u8	ucType;
	u32	ulFileSeqno;
	u32	ulLengthTail;
};

struct SaveHeaderEntry
{
	u32	ulLength;
	u8	ucType;
	u32	ulLengthTail;
};

struct HeaderChangeLogEntry
{
	u64		ullDocID;
	char	pHeaderBuff[];
};

struct MetaFile
{
	u64	ullActiveLogFileID;
	u64	ullLastOldHeaderFileID;
};

struct MetaFileOldHeaderFileIDs
{
	u32	ulOldHeaderFileForFlushingSeqno;
	u32	ulOldHeaderFileForWorkingSeqno;
};
#pragma pack(0)

#endif /* Aos_BlobSE_Entry_H_ */
