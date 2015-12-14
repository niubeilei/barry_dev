////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileItr.cpp
// Description:
//	A FileItr is an iterator that segments the original buffer into 
//  chunks. These chunks are suitable for transportation.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/FileItr.h"

/*
#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "UtilComm/ConnBuff.h"



OmnFileItr::OmnFileItr()
:
mFileLength(0),
mCrtSegId(0),
mSegmentSize(0),
mBatchSize(0)
{
}


OmnFileItr::OmnFileItr(const OmnFilePtr &file, 
					   const int segmentSize, 
					   const int batchSize)
:
mFile(file),
mCrtSegId(0),
mSegmentSize(segmentSize),
mBatchSize(batchSize)
{
	if (!mFile)
	{
		OmnAlarm << OmnErrId::eFileError
			<< "File is null" << enderr;
		mFileLength = 0;
		return;
	}

	mFileLength = mFile->getLength();
}


OmnFileItr::~OmnFileItr()
{
}


OmnConnBuffPtr
OmnFileItr::next()
{
	// 
	// It moves to the next segment, retrieves it, and returns it. If it is
	// the last segment, the actual size may be less than mSegmentSize. 
	// Otherwise, the retrieved should be mSegmentSize big.
	//
	if (!mFile)
	{
		OmnAlarm << OmnErrId::eFileError
			<< "File is null" << enderr;
		return 0;
	}

	OmnLL index = mCrtSegId * mSegmentSize;

	// 
	// Determine the size
	//
	if (index >= mFileLength)
	{
		// 
		// We have already retrieved everything. 
		// 
		OmnWarn << OmnErrId::eFileError
			<< "No next: " << mFileLength << ":" << index << enderr;
		return 0;
	}

	int len;
	if (mFileLength - index >= mSegmentSize)
	{
		len = mSegmentSize;
	}
	else
	{
		len = (mFileLength - index).toInt();
	}

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(len+1);

	// 
	// Retrieve the data
	//
	int bytesRead = mFile->readToBuff(index, len, buff->getBuffer());
	if (bytesRead <= 0)
	{
		// 
		// This indicates an error. 
		// 
		OmnAlarm << OmnErrId::eFileError
			<< "Failed to read the file: " << mFile->getFileName() << enderr;
		return 0;
	}

	buff->setDataLength(bytesRead);
	if (bytesRead < len)
	{
		// 
		// This indicates that this is the last segment.
		//
		buff->setLastSegment(true);
	}

	mCrtSegId++;
	return buff;
}


OmnConnBuffPtr	
OmnFileItr::getSeg(const OmnLL index)
{
	// 
	// This function retrieves the 'index'-th segment. Each
	// segment is 'mSegment' size in bytes. 
	// 
	// IMPORTANT: if we want to support files larger than 4G, we have to use
	// OmnLL instead of uint for the length. Currently we are using uint. 
	//
	if (!mFile)
	{
		OmnAlarm << OmnErrId::eFileError
			<< "File is null" << enderr;
		return 0;
	}

	OmnLL startPos = index * mSegmentSize;
	if (startPos >= mFileLength)
	{
		// 
		// There is no such segment.
		//
		return 0;
	}

	OmnConnBuffPtr buff = OmnNew OmnConnBuff(mSegmentSize+1);
	int bytesRead = mFile->readToBuff(startPos, mSegmentSize, buff->getBuffer());
	if (bytesRead <= 0)
	{
		// 
		// Failed to read from the file.
		//
		OmnWarn << OmnErrId::eFileError
			<< "Failed to read file: " << mFile->getFileName() << enderr;
		return 0;
	}

	buff->setDataLength(bytesRead);
	if (bytesRead < mSegmentSize)
	{
		// 
		// This indicates that this is the last segment.
		//
		buff->setLastSegment(true);
	}
	return buff;
}


int
OmnFileItr::getNumPackets() const
{
	if (mFileLength < 0)
	{
		OmnAlarm << "Length is negative: " << mFileLength << enderr;
		return 0;
	}

	if (mSegmentSize <= 0)
	{
		OmnAlarm << "Segment size is not correct: " << mSegmentSize << enderr;
		return 0;
	}

	return ((mFileLength / mSegmentSize).toInt() + 1);
}


OmnConnBuffPtr
OmnFileItr::fetchPrev()
{
	OmnCheckAReturn(mCrtSegId >= 0, "Sanity check", 0);

	if (mCrtSegId == 0)
	{
		// 
		// There is no previous buffer. 
		//
		return 0;
	}

	return getSeg(mCrtSegId-1);
}


OmnConnBuffPtr
OmnFileItr::fetchNext()
{
	OmnCheckAReturn(mCrtSegId >= 0, "Sanity check", 0);

	return getSeg(mCrtSegId+1);
}


OmnConnBuffPtr
OmnFileItr::crt()
{
	OmnCheckAReturn(mCrtSegId >= 0, "Sanity check", 0);

	return getSeg(mCrtSegId);
}


OmnConnBuffPtr
OmnFileItr::prev()
{
	// 
	// It moves the pointer to the previous segment and returns the segment.
	// If there is no more previous, it returns 0.
	//
	if (mCrtSegId == 0)
	{
		//
		// No more previous.
		//
		return 0;
	}

	mCrtSegId--;
	return crt();
}
*/

