////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/08/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchGetDocsAsyncTrans_h
#define Aos_DocTrans_BatchGetDocsAsyncTrans_h

#include "TransUtil/DocTrans.h"
#include "TransUtil/Ptrs.h"


class AosBatchGetDocsAsyncTrans : public AosDocTrans 
{
	u32						mNumDocs;
	u32						mBlockSize;
	AosBuffPtr			   	mDocids;
	AosBuffPtr			 	mSizeIdLen;
	AosAsyncRespCallerPtr  	mRespCaller;
	int						mVid;

public:
	AosBatchGetDocsAsyncTrans(const bool regflag);
	AosBatchGetDocsAsyncTrans(
		const AosAsyncRespCallerPtr &resp_caller,
		const int vid,
		const u32 num_docs,
		const u32 blocksize,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &len_buff,
		const u64 snap_id,
		const bool need_save,	
		const bool need_resp);

	~AosBatchGetDocsAsyncTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();
	
};
#endif


