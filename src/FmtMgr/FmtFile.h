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
#ifndef AOS_FmtMgr_FmtFile_h
#define AOS_FmtMgr_FmtFile_h

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

class AosFmtFile: public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	OmnString		mDirname;
	OmnString       mFilename;
	const u64 		mMaxFileSize;
	bool			mShowLog;

	map<u32, OmnString>		mNormFname;
	map<u32, OmnString>		mMergeFname;
	u32				mNextNormSeqno;
	u32				mNextMergeSeqno;
	OmnFilePtr		mMergeFile;	
	OmnFilePtr		mActiveFile;	
	
public:
	AosFmtFile(const OmnString &dirname, 
			const OmnString &fname, 
			const u64 max_fsize,
			const bool show_log);
	~AosFmtFile();

	bool	init();

	bool 	addData(const char *data,
				const u32 data_len,
				const bool need_proc);
	bool 	addFmt(
				const AosBuffPtr &fmt_buff,
				const bool need_proc);
	bool	empty(){ return mNormFname.size() == 0; };
	
	int 	begSeq();
	int 	endSeq();
	int 	activeSeq();
	int 	mergeBegSeq();
	int 	getNextSeqno(const int crt_seqno);
	bool    startNextActive();
	bool 	readNormFile( const int file_seqno, AosBuffPtr &file_buff);

	//bool 	deleteFilesToSeq(const u32 end_seq);
	bool	deleteFile(const u32 file_seq);
	bool 	deleteAllFiles();

	//bool 	addMergeData(const char *data, const u32 data_len);
	bool 	addMergeFmt(const AosBuffPtr &fmt_buff);

	bool 	mergeNextAreaFiles();
	bool 	readMergeFile(const int file_seq, AosBuffPtr &file_buff);

	int 	endProcedSeq();
	bool	renameToProced(const int seq);

	int 	getNextProcedSeq(const int crt_seqno);
	u64		getMaxFmtIdByFileSeq(const u32 seqno);

private:
	bool 	initFileMap(
				const u32 file_seq,
				const OmnString &suffix,
				const OmnString &full_fname);
	void 	initNextNormSeq();
	void 	initNextMergeSeq();

	bool 	getActiveFile(const u32 data_len, const bool need_proc); 
	bool 	createActiveFile(const OmnString &suffix);
	OmnFilePtr openNormFile(const u32 seqno);
	
	int 	begSeqPriv();
	int 	endSeqPriv();
	int 	activeSeqPriv();

	bool 	createMergeFile();
	OmnFilePtr openMergeFile(const u32 seqno);
	bool 	addToNormEnd(map<u32, OmnString> &fnames);

};
#endif
