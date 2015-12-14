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
#ifndef Aos_DocTrans_GetDocTrans_h
#define Aos_DocTrans_GetDocTrans_h

#include "TransUtil/DocTrans.h"

class AosGetDocTrans : virtual public AosDocTrans
{
	u64			mDocid;
	OmnString	mObjid;
	bool		mNeedBinaryData;

public:
	AosGetDocTrans(const bool regflag);
	AosGetDocTrans(
			const u64 &docid,
			const bool need_binarydata,
			const u64 &snap_id);
	AosGetDocTrans(
			const OmnString &objid,
			const bool need_binarydata,
			const u64 &snap_id);
	~AosGetDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool procGetResp();

private:
	AosXmlTagPtr 	getDoc();
};

#endif

