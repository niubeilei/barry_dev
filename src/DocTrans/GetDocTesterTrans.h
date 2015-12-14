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
#ifndef Aos_DocTrans_GetDocTesterTrans_h
#define Aos_DocTrans_GetDocTesterTrans_h

#include "TransUtil/DocTrans.h"

class AosGetDocTesterTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	bool		mNeedBinaryData;

public:
	AosGetDocTesterTrans(const bool regflag);
	AosGetDocTesterTrans(
			const u64 docid,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosGetDocTesterTrans(); 
	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eTesterGetDoc"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool procGetResp();

};
#endif

