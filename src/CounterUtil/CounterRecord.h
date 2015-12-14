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
// 06/15/211: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_CounterUtil_CounterRecord_h
#define Aos_CounterUtil_CounterRecord_h

#include "alarm_c/alarm.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEUtil/DocTags.h"

class AosCounterRecord : public OmnRCObject
{
	OmnDefineRCObject;

private:
	u32				mRecordSize;  // The size of all member data 
	u64 			mCounterId;   
	int64_t			mCounterValue;
	u64				mCounterIILID;
	int64_t			mCounterMax;
	int64_t			mCounterMin;
	int64_t			mCounterMean;
	int64_t			mCounterStderr;
	int64_t			mCounterNum;   
	u32				mExtSeqno;    
	u64				mExtOffset;    
	OmnString		mCounterName;  
	OmnString 		mCounterStatType;

public:
	AosCounterRecord();
	~AosCounterRecord(){};

	u32		calRecordLength()
	{
		return sizeof(u32) 		// record size
			 + sizeof(u64)  	// counterid
			 + sizeof(int64_t)	// counterValue
			 + sizeof(u64) 	    // ILLID
			 + sizeof(int64_t)  // max
			 + sizeof(int64_t)  // min
			 + sizeof(int64_t)  // mean
			 + sizeof(int64_t)  // stderr
			 + sizeof(int64_t)  // num
			 + sizeof(u32)	    // Ext seqno
			 + sizeof(u64)		// Ext offset
			 + sizeof(u32) + mCounterName.length(); // CounterName
	}
	// setters
	void	setCounterId(const u64 id){mCounterId = id;} 	
	void	setCounterValue(const int64_t value){mCounterValue = value;}
	void 	setCounterIILID(const u64 iilid){mCounterIILID = iilid;}
	void 	setCounterMax(const int64_t max){mCounterMax = max;}
	void 	setCounterMin(const int64_t min){mCounterMax = min;}
	void 	setCounterMean(const int64_t mean){mCounterMean = mean;}
	void 	setCounterStderr(const int64_t stde){mCounterMax = stde;}
	void 	setCounterNum(const int64_t num){mCounterNum = num;}

	void 	setCounterExtSeqno(const u32 extSeqno){mExtSeqno = extSeqno;}
	void 	setCounterExtOffset(const u64 extOffset){mExtOffset = extOffset;}
	void 	setCounterStatType(const OmnString statType){mCounterStatType = statType;}
	void  	setCounterName(const OmnString cname)
	{
		mCounterName = cname;
		setRecordSize();
	}

	void 	setRecordSize()
	{
		mRecordSize = calRecordLength();
	}

	// getters
	u64		getCounterId() const {return mCounterId;} 	
	int64_t	getCounterValue() const {return mCounterValue;} 	
	u64 	getCounterNameIILID() const {return mCounterNameIILID;}
	u64 	getCounterValueIILID() const {return mCounterValueIILID;}
	u64 	getCounterTimeNameIILID(){return mCounterTimeNameIILID;}
	u64 	getCounterTimeValueIILID(){return mCounterTimeValueIILID;}
	u32 	getCounterSeqno() const {return mCounterSeqno;}
	u64 	getCounterOffset() const{return mCounterOffset;}
	u32 	getCounterTimeSize() const {return mCounterTimeSize;}
	u32 	getCounterTimeType() const {return mCounterTimeType;}
	int64_t	getCounterMax() const {return mCounterMax;}
	int64_t	getCounterMin() const {return mCounterMax;}
	int64_t	getCounterMean() const {return mCounterMean;}
	int64_t	getCounterStderr() const {return mCounterMax;}
	int64_t	getCounterNum() const {return mCounterNum;}

	u32 	getCounterExtSeqno() const {return mExtSeqno;}
	u64 	getCounterExtOffset() const {return mExtOffset;}
	OmnString 	getCounterStatType() const {return mCounterStatType;}
	OmnString  	getCounterName() const {return mCounterName;}

	u32 	getRecordSize()const
	{
		return mRecordSize;
	}

};
#endif
#endif

