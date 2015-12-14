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
#ifndef Aos_DocTrans_IndexMgrModifyDocTrans_h
#define Aos_DocTrans_IndexMgrModifyDocTrans_h

#include "TransUtil/DocTrans.h"

class AosIndexMgrModifyDocTrans : virtual public AosDocTrans
{
	enum
	{
		eMaxPeriods = 1000
	};

private:
	u64			mDocid;
	int			mLevel;
	int			mPeriod;

public:
	AosIndexMgrModifyDocTrans(const bool regflag);
	AosIndexMgrModifyDocTrans(
			const u64 docid,
			const int level, 
			const int period,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosIndexMgrModifyDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

