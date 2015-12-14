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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchCreateDocTrans_h
#define Aos_DocTrans_BatchCreateDocTrans_h

#include "TransUtil/DocTrans.h"

class AosBatchCreateDocTrans : virtual public AosDocTrans
{

private:
	u64				mDocid;

	AosBuffPtr		mDocBuff;
	int 			mDocLen;
	u64				mTaskDocid;

public:
	AosBatchCreateDocTrans(const bool regflag);
	AosBatchCreateDocTrans(
			const u64 docid,
			const char *new_doc,
			const int data_len,
			const u64 &snap_id,
			const u64 &task_docid,
			const bool need_save,
			const bool need_resp);
	~AosBatchCreateDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

