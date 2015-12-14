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
// 10/12/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_LogFile_h
#define AOS_SEUtil_LogFile_h

#include "aosUtil/Types.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

#include <map>
using namespace std;

class AosLogFile: virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	
	struct iterator
	{
		int			mSeqno;
		u64			mOffset;

		iterator()
		:
		mSeqno(-1),
		mOffset(0)
		{}
		
		iterator(const int seqno, const u64 offset)
		:
		mSeqno(seqno),
		mOffset(offset)
		{}

		void	reset(const int seqno, const u64 offset)
		{
			mSeqno = seqno;
			mOffset = offset;
		}
	};

	struct OpenedFile
	{
		OmnFilePtr		mFile;
		u32 			mRefCount;

		OpenedFile(const OmnFilePtr &file)
		:
		mFile(file),
		mRefCount(1)
		{
		};
	};

private:
	OmnMutexPtr     	mLock;
	u32					mNextActiveSeqno;
	OmnFilePtr			mActiveFile;
	map<u32, OmnString>	mNormFname;
	
	map<u32, OpenedFile> mOpenedFile;

protected:
	OmnString		mDirname;
	OmnString       mFilename;
	const u64 		mMaxFileSize;
	bool			mShowLog;

public:
	AosLogFile(const OmnString &dirname, 
			const OmnString &fname, 
			const u64 max_fsize,
			const bool show_log = false);
	~AosLogFile();

	bool	start();

	//iterator begin();
	//iterator end();	
	int 	begSeq();
	int 	endSeq();
	int 	activeSeq();
	bool 	startNextActive();

	int		getNextSeqno(const u32 crt_seqno);
	bool	readEachFile(const int file_seqno, 
				AosBuffPtr &file_buff); 
	//bool 	readData(
	//			const iterator &start,
	//			const u32 total_size,
	//			AosBuffPtr &buff);
	
	bool	addData(const char *data, const u32 data_len);
	bool	empty(){ return mNormFname.size() == 0; };

	bool 	deleteFile(const u32 seqno);
	bool 	deleteAllFiles();

	static bool 		readFileStatic(const OmnFilePtr &file, AosBuffPtr &file_buff);
	static u32			getSeqnoFromFname(const OmnString &fname);
	static OmnString	getFnameSuffix(const OmnString &fname);

private:
	int 	begSeqPriv();
	int 	endSeqPriv();
	int 	activeSeqPriv();

	bool		createActiveFilePriv();	
	OmnFilePtr 	openFilePriv(const u32 seqno);
	bool 		deleteFilesPriv(const u32 from_seq, const int end_seq);

	//void		addToOpenedFile(const u32 seqno, const OmnFilePtr &file);
	//OmnFilePtr 	findFromOpenedFile(const u32 seqno);
	//void 		deleteFromOpenedFile(const u32 seqno);


};
#endif
