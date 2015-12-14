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
// 11 Jun 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BlobSERestore.h"

#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/Ptrs.h"
AosBlobSERestore::AosBlobSERestore()
{
    // TODO Auto-generated constructor stub

}


AosBlobSERestore::~AosBlobSERestore()
{
    // TODO Auto-generated destructor stub
}


int
AosBlobSERestore::restoreToTimestamp(
		AosRundata	*rdata,
		const u64	ullTimestamp)
{
	u32 ullBoundaryOldHeaderFileSeqno;
	int iRet = prepareForRestoration(rdata, ullBoundaryOldHeaderFileSeqno);
	if (0 != iRet)
	{
		OmnAlarm << "prepareForRestoration failed, iRet=" << iRet << enderr;
		return -1;
	}
	u64 ullBoundaryRawfid = 0;
	u64 ullBoundaryOffset = 0;
	iRet = getBodyFileBoundaryByTimestamp(rdata, ullTimestamp, ullBoundaryRawfid, ullBoundaryOffset);
	if (0 != iRet)
	{
		OmnAlarm << "getBodyFileBoundaryByTimestamp failed, iRet=" << iRet << " timestamp:" << ullTimestamp << enderr;
		return -2;
	}
	iRet = restoreHeadersToTimestamp(rdata, ullTimestamp,
			ullBoundaryOldHeaderFileSeqno, ullBoundaryRawfid, ullBoundaryOffset);
	if (0 != iRet)
	{
		OmnAlarm << "restoreHeaders failed, iRet=" << iRet << " timestamp:" << ullTimestamp << enderr;
		return -3;
	}
	iRet = removeBodyFilesThatBeyondBoundary(rdata, ullBoundaryRawfid, ullBoundaryOffset);
	if (0 != iRet)
	{
		OmnAlarm << "removeBodyFilesThatBeyondBoundary failed, iRet=" << iRet
				<< " BoundaryRawfid:" << ullBoundaryRawfid << " BoundaryOffset" << ullBoundaryOffset << enderr;
		return -4;
	}
	return 0;
}


int
AosBlobSERestore::prepareForRestoration(
		AosRundata	*rdata,
		u32			&ullBoundaryOldHeaderFileSeqno)
{
//	u32	ulHeaderFileForFlushingSeqno = 0;
//	u32	ulHeaderFileForWorkingSeqno = 0;
//	int iRet = mHeaderFileMgr->getLastOldHeaderFileSeqnosFromMetafile(rdata,
//			ulHeaderFileForFlushingSeqno, ulHeaderFileForWorkingSeqno);
//	if (0 != iRet)
//	{
//		OmnAlarm << "mHeaderFileMgr->getLastOldHeaderFileSeqnosFromMetafile failed, iRet=" << iRet << enderr;
//		return -1;
//	}
//	u32 ulMaxSeqno = ulHeaderFileForFlushingSeqno > ulHeaderFileForWorkingSeqno
//			? ulHeaderFileForFlushingSeqno : ulHeaderFileForWorkingSeqno;
//	ulHeaderFileForFlushingSeqno = ulMaxSeqno + 2;	//to leave a gap for human to check where it starts
//	ulHeaderFileForWorkingSeqno = ulHeaderFileForFlushingSeqno + 1;
//	iRet = mHeaderCache->saveOldestOldHeaderFileSeqnoForCompaction(ulHeaderFileForFlushingSeqno);
//	if (0 != iRet)
//	{
//		OmnAlarm << "mHeaderCache->saveOldestOldHeaderFileSeqnoForCompaction failed, iRet=" << iRet
//				<< " seqno trying to save:" << ulHeaderFileForFlushingSeqno << enderr;
//		return -2;
//	}
	return 0;
}


int
AosBlobSERestore::restoreHeadersToTimestamp(
		AosRundata	*rdata,
		const u64	ullTimestamp,
		const u32	ullBoundaryOldHeaderFileSeqno,
		const u64	ullBoundaryRawfid,
		const u64	ullBoundaryOffset)
{
	u64 ullMaxDocid = 999999999;	//FIXME:how to get the max docid?
	u64 ullTmpTimestamp = ullTimestamp;
	AosBlobHeaderPtr pHeader = NULL;
	stack_header_t headerStack;
	int iRet = 0;
	for (u64 ullDocid = 1; ullDocid <= ullMaxDocid; ullDocid++)
	{
		if (!headerStack.empty())
		{
			OmnAlarm << "headerStack not empty! dumping contents..." << enderr;
			while(!headerStack.empty())
			{
				headerStack.top()->dump();
				headerStack.pop();
			}
		}
		ullTmpTimestamp = 0;
		pHeader = mHeaderCache->readHeader(rdata, ullDocid, ullTmpTimestamp);
		if (pHeader.isNull())
		{
			OmnAlarm << "mHeaderCache->readHeader failed, docid:" << ullDocid << " timestamp:" << ullTmpTimestamp << enderr;
			return -1;
		}
		headerStack.push(pHeader);
		while(AosBlobHeader::noNextHeader != pHeader->getNextHeaderPtr())
		{
			ullTmpTimestamp = pHeader->getTimestamp() - 1;	//to read the older one
			pHeader = mHeaderCache->readHeader(rdata, ullDocid, ullTmpTimestamp);
			if (pHeader.isNull())
			{
				OmnAlarm << "mHeaderCache->readHeader failed, docid:" << ullDocid << " timestamp:" << ullTmpTimestamp << enderr;
				return -2;
			}
			if (pHeader->getTimestamp() <= ullTimestamp)
			{
				if (ullBoundaryRawfid > pHeader->getBodyRawfid())
				{
					OmnAlarm << "ullBoundaryRawfid:" << ullBoundaryRawfid
							<< " > pHeader->getBodyRawfid():" << pHeader->getBodyRawfid() << enderr;
					return -3;
				}
				else if (ullBoundaryRawfid == pHeader->getBodyRawfid() && ullBoundaryOffset > pHeader->getBodyOffset())
				{
					OmnAlarm << "ullBoundaryRawfid:" << ullBoundaryRawfid
							<< " == pHeader->getBodyRawfid():" << pHeader->getBodyRawfid()
							<< "&& ullBoundaryOffset:" << ullBoundaryOffset
							<< " > pHeader->getBodyOffset():" << pHeader->getBodyOffset() << enderr;
					return -4;
				}
				headerStack.push(pHeader);
			}
			else	//for debugging
			{
				OmnScreen << "header's timestamp:" << pHeader->getTimestamp()
						<< " is newer than timestamp:" << ullTimestamp << endl;
			}
		}
		iRet = mHeaderCache->clearHeaderCacheForDocid(ullDocid);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderCache->clearHeaderCacheForDocid failed, iRet=" << iRet << enderr;
			return -5;
		}
		while(!headerStack.empty())
		{
			pHeader = headerStack.top();
			iRet = mHeaderCache->updateHeader(rdata, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "mHeaderCache->updateHeader failed, iRet=" << iRet << " docid:" << ullDocid << enderr;
				return -6;
			}
		}
	}
	iRet = removeUnusedOldHeaderFiles(rdata, ullBoundaryOldHeaderFileSeqno);
	if (0 != iRet)
	{
		OmnAlarm << "removeUnusedOldHeaderFiles failed, iRet=" << iRet
				<< " BoundaryOldHeaderFileSeqno:" << ullBoundaryOldHeaderFileSeqno << enderr;
		return -7;
	}
	return 0;
}


int
AosBlobSERestore::getBodyFileBoundaryByTimestamp(
		AosRundata	*rdata,
		const u64	ullTimestamp,
		u64			&ullBoundaryRawfid,
		u64			&ullBoundaryOffset)
{
	return mBlobSE->getBodyFileBoundaryByTimestamp(rdata, ullTimestamp, ullBoundaryRawfid, ullBoundaryOffset);
}


int
AosBlobSERestore::removeBodyFilesThatBeyondBoundary(
		AosRundata	*rdata,
		const u64	ullBoundaryRawfid,
		const u64	ullBoundaryOffset)
{
	return 0;
}


int
AosBlobSERestore::removeUnusedOldHeaderFiles(
		AosRundata	*rdata,
		const u32	ullBoundaryOldHeaderFileSeqno)
{
	return 0;
}
