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
// 2014/01/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_GetCommonDocTrans_h
#define Aos_DocTrans_GetCommonDocTrans_h

#include "TransUtil/DocTrans.h"

class AosGetCommonDocTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	int			mRecordLen;

public:
	AosGetCommonDocTrans(const bool regflag);
	AosGetCommonDocTrans(
			const u64 &docid,
			const int record_len,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosGetCommonDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	AosXmlTagPtr 	getDoc();

};
#endif

