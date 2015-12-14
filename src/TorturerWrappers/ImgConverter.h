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
//   
//
// Modification History:
// 09/30/2009 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TorturerWrapper_ImgConverter_h
#define AOS_TorturerWrapper_ImgConverter_h

#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"


class OmnString;

class AosImgConverter : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

public:
	enum 
	{
		eTinySize = 16,
		eIconSize = 32,
		eThumbnailSize = 64,
		eSmallSize = 128,
		eMediumSize = 256,
		eLargeSize = 512,
		eHugeSize = 1024,

		eNumFlags = 7
	}; 

	struct Request
	{
		int			maxsize;
		char 		filetype;
		OmnString 	orig_fn; 
		u64 		seqno; 
		OmnString 	fileext; 
		int 		ww; 
		int 		hh; 
		OmnString	dirname;
		Request *	next;
	};

private:
	OmnThreadPtr		mThread;
	Request			   *mHead;
	Request			   *mTail;
	Request			   *mIdle;
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnString			mDirname;

public:
	AosImgConverter();
	~AosImgConverter();

	bool 	stop();

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	void	setDir(const OmnString &dir) {mDirname = dir;}

	bool 	addRequest(
				const int maxsize,
				const char filetype,
				const OmnString &orig_fn, 
				const u64 seqno, 
				const OmnString &fileext, 
				const int ww, 
				const int hh, 
				const OmnString &dirname); 

private:
	bool 	convertFile(
        		const bool removeFlag,
        		const char filetype,
        		const OmnString &orig_fn,
        		const char type,
        		const u64 seqno,
        		const OmnString &fileext,
        		const int ww,
        		const int hh,
        		const int fsize, 
				const OmnString &dirname);
};
#endif

