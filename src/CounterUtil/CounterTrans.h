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
// 03/25/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CounterUtil_CounterTrans_h
#define Aos_CounterUtil_CounterTrans_h

#include "TransUtil/CubicTrans.h"
#include "CounterUtil/CounterOperations.h"

class AosCounterTrans : virtual public AosCubicTrans
{

private:
	OmnString	mCounterId;
	AosCounterOperation::E mOpr;
	//OmnString	mCname; 
	//int64_t 	mCvalue; 
	//u64			mUnitime;
	//AosStatType::E mStatType;

	AosBuffPtr	mCont;

public:
	AosCounterTrans(const bool regflag);
	AosCounterTrans(
			const OmnString &counter_id,
			const AosCounterOperation::E &opr,
			const AosBuffPtr &buff,
			//const OmnString &cname, 
			//const int64_t &cvalue, 
			//const u64 &unitime,
			//const AosStatType::E statType,
			const bool need_save,
			const bool need_resp);
	~AosCounterTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	OmnString getCounterId(){ return mCounterId; };
	AosCounterOperation::E	getOpr() { return mOpr; };
	AosBuffPtr getCont() {return mCont; };

private:
	u32 	getDistid(const OmnString &counter_id);

};
#endif

