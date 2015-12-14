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
// 2013/04/08 Created by Linda
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DocTrans_BatchGetDocResp_h
#define Aos_DocTrans_BatchGetDocResp_h

#include "TransUtil/TaskTrans.h"

class AosBatchGetDocsResp : virtual public AosTaskTrans 
{

private:
	OmnString			mScannerId;
	AosBuffPtr 			mContents;
	int					mServerId;
	bool				mFinished;
	int					mNumCallBackNumDatas;

public:
	AosBatchGetDocsResp(const bool regflag);
	AosBatchGetDocsResp(
			const OmnString &scanner_id,
			const int sender_server_id,
			const int serverid,
			const AosBuffPtr &contents,
			const bool finished,
			const int num_call_back_data_msgs); 
	~AosBatchGetDocsResp();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif
#endif

