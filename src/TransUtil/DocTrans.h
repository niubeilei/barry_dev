////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_DocTrans_h
#define Aos_TransUtil_DocTrans_h

#include "Rundata/Rundata.h"
#include "TransUtil/Ptrs.h"
#include "TransUtil/CubicTrans.h"

class AosDocTrans : public AosCubicTrans
{
	u64				mDistId;

protected:
	u64 			mSnapshotId;

	static 	AosTransProcThrdPtr		smProcThrd;

public:
	AosDocTrans(
			const AosTransType::E type,
			const bool reg_flag);

	AosDocTrans(
			const AosTransType::E type,
			const u64 &docid,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);

	AosDocTrans(
		const AosTransType::E type,
		const u32 vid,
		const bool is_cube,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id);

	~AosDocTrans();
	
	virtual bool 	serializeFrom(const AosBuffPtr &buff) = 0;
	virtual bool 	serializeTo(const AosBuffPtr &buff) = 0;

	virtual AosTransPtr clone() = 0;
	virtual bool	proc() = 0;
	virtual bool	directProc();

	void	setDistId(const u64 &id){mDistId = id;}
};
#endif

