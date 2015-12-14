////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/04/29Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapTrans_TransBmpInstr_h
#define Aos_BitmapTrans_TransBmpInstr_h

#include "BitmapTrans/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/BitmapTrans.h"
#include "Thread/Ptrs.h"
#include "Query/Ptrs.h"
#include "Util/Ptrs.h"
using namespace std;

class AosTransSendInstrs : public AosBitmapTrans 
{
	OmnDefineRCObject;

private:
	u64			mExecutorID;
	AosBuffPtr		mBitmapsBuff;
	AosBuffPtr		mInstrBuff;

public:
	AosTransSendInstrs(const bool regflag);
	AosTransSendInstrs(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &bitmap_buff,
			const AosBuffPtr &instr_buff,
			const u32 cube_id,
			const u64 &executor_id);
	~AosTransSendInstrs();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	bool procTrans();

	// BitmapTrans Interface
	virtual bool proc(const AosIILObjPtr &iilobj,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata){OmnShouldNeverComeHere; return true;};

	virtual bool                                    respCallBack();
	virtual u64			getExecutorID()const{return mExecutorID;}
	virtual AosBuffPtr	getBitmapsBuff()const{return mBitmapsBuff;}
	virtual AosBuffPtr	getInstrBuff()const{return mInstrBuff;}
	virtual int getSerializeSize() const;
};
#endif

