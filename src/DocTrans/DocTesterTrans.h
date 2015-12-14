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
#ifndef Aos_DocTrans_DocTesterTrans_h
#define Aos_DocTrans_DocTesterTrans_h

#include "TransUtil/DocTrans.h"

class AosDocTesterTrans : virtual public AosDocTrans
{

private:
	int 			mOpr;
	u64				mDocid;
	AosBuffPtr		mDocBuff;
	int 			mDocLen;

public:
	AosDocTesterTrans(const bool regflag);

	AosDocTesterTrans(
			const int opr,
			const u64 docid,
			const char *new_doc,
			const int data_len,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosDocTesterTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType();
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

