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
#ifndef Aos_DocTrans_IncrememtValueTrans_h
#define Aos_DocTrans_IncrememtValueTrans_h

#include "TransUtil/DocTrans.h"

class AosIncrememtValueTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	OmnString   mAname;
	u64			mInitValue;
	u64			mIncValue;

public:
	AosIncrememtValueTrans(const bool regflag);
	AosIncrememtValueTrans(
			const u64 docid,
			const OmnString &aname,
			const u64 init_value,
			const u64 inc_value,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIncrememtValueTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

