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
// 11/06/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_GetSmallDocTrans_h
#define Aos_DocTrans_GetSmallDocTrans_h

#include "TransUtil/DocTrans.h"

class AosGetSmallDocTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	u32 		mMaxEntryNums;

public:
	AosGetSmallDocTrans(const bool regflag);
	AosGetSmallDocTrans(
			const u64 docid,
			const u32 entryNums,
			const bool need_save,
			const bool need_resp);
	~AosGetSmallDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

