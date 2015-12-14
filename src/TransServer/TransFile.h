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
#ifndef AOS_TransServer_TransFile_h
#define AOS_TransServer_TransFile_h

#include "aosUtil/Types.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

class AosTransFile: public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr     mLock;
	OmnString		mDirname;
	OmnString       mFilename;
	const u64 		mMaxFileSize;
	bool			mShowLog;
	
	map<u32, OmnString>	mNormFname;
	map<u32, OmnString>	mStaleFname;
	map<u32, OmnString>	mMergeFname;	// stale to merge.
	map<u32, OmnString>	mNewerFname;
	
	OmnFilePtr		mActiveFile;	
	OmnFilePtr		mStaleFile;	
	OmnFilePtr		mNewerFile;	

	u32				mNextNormSeqno;
	int				mStaleBoundSeq;
	
public:
	AosTransFile(const OmnString &dirname, 
			const OmnString &fname, 
			const u64 max_fsize,
			const bool showlog);
	~AosTransFile();
	
	bool	start();
	bool 	initFileMap(const u32 file_seq,
					const OmnString &suffix,
					const OmnString &full_fname);

	bool	saveToNormFile(const AosBuffPtr &trans_buff);
	bool	saveToNewerFile(const AosTransPtr &trans);
	bool	saveToStaleFile(const AosBuffPtr &data);
	//bool	saveToStaleFile(const AosTransPtr &trans);
	bool 	mergeStaleFiles();
	bool	mergeNewerFiles();

	int 	staleBegSeq();
	int 	staleEndSeq();
	int 	getNextStaleSeqno(const u32 crt_seqno);
	bool 	readStaleFile( const int file_seqno, AosBuffPtr &file_buff);
	bool    startNextStale();

	bool 	setStaleBoundSeq(const int seq);

	bool	empty(){ return mNormFname.size() == 0; };
	int 	begSeq();
	int 	endSeq();
	int 	activeSeq();
	int 	getNextSeqno(const u32 crt_seqno);
	bool    startNextActive();
	bool 	readNormFile( const int file_seqno, AosBuffPtr &file_buff);

private:
	void 		initNextNormSeq();
	bool 		startPriv();;
	bool 		deleteStaleFiles();
	bool 		deleteNewerFiles();

	//AosBuffPtr  generateBigBuff(vector<AosTransPtr> &v_trans);
	
	bool 		createStaleFile();
	OmnFilePtr 	openStaleFilePriv(const u32 seqno);

	bool 		createNewerFile();
	bool 		createActiveFile();
	OmnFilePtr 	openNormFile(const u32 seqno);

	bool 		deleteFilesToSeq(const u32 end_seq);

	int 		begSeqPriv();
	int 		endSeqPriv();

};
#endif
