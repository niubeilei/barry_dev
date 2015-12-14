////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileItr.h
// Description:
//	A FileItr is an iterator that segments the original buffer into 
//  chunks. These chunks are suitable for transportation.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_FileItr_h
#define Omn_Util_FileItr_h

/*
#include "Porting/LongTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BasicTypes.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/StreamItr.h"

class OmnFileItr : public OmnStreamItr
{
	OmnDefineRCObject;

private:
	OmnFilePtr	mFile;
	OmnLL		mFileLength;
	OmnLL		mCrtSegId;
	int			mSegmentSize;
	int			mBatchSize;
	OmnLL		mNumSegments;

public:
	OmnFileItr();
	OmnFileItr(const OmnFilePtr &buff, const int segmentSize, const int batchSize);
	virtual ~OmnFileItr();

	// 
	// OmnStreamItr interface
	//
	virtual void	reset() {mCrtSegId = 0;}
	virtual bool	hasMore() const {return mCrtSegId < getNumPackets();}
	virtual int		getSegmentSize() const {return mSegmentSize;}
	virtual void	setSegmentSize(const int s) {mSegmentSize = s;}
	virtual OmnConnBuffPtr	getSeg(const OmnLL index);
	virtual OmnConnBuffPtr	next();
	virtual OmnConnBuffPtr	crt();
	virtual OmnConnBuffPtr	prev();
	virtual OmnConnBuffPtr	fetchPrev();
	virtual OmnConnBuffPtr	fetchNext();
	virtual OmnLL	getStreamLength() const {return mFileLength;}
	virtual int		getNumPackets() const;
	virtual int		getBatchSize() const {return mBatchSize;}
};
*/
#endif

