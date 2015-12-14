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
// 09/15/2011	Created by Brian
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_SeqnoFnameEntry_h
#define AOS_SeLogSvr_SeqnoFnameEntry_h

#include "alarm_c/alarm.h"
#include "aosUtil/Types.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "UtilHash/Ptrs.h"
#include "UtilHash/HashedObj.h"

class AosSeqnoFnameEntry : public AosHashedObj
{
	OmnDefineRCObject;

private:
	char		mStatus;
	u32			mSeqno;
	u64 		mFileId;
	OmnFilePtr	mFile;

public:
	AosSeqnoFnameEntry();
	AosSeqnoFnameEntry(
			const OmnString &key, 
			const u32 seqno,
			const u64 &file_id);
	~AosSeqnoFnameEntry(){}

	virtual AosHashedObjPtr clone();
	virtual bool reset();

	virtual bool isSame(const AosHashedObjPtr &rhs) const;

	virtual int doesRecordMatch(
			AosBuff &buff,
			const OmnString &key_in_buff,
			const OmnString &expected_key,
			AosHashedObjPtr &obj);

	virtual int setContentsToBuff(AosBuff &buff);

	u64 getFileId() const {return mFileId;}
	u32 getNextSeqno();
	void setDeleteFlag();
	OmnFilePtr openFile(const AosRundataPtr &rdata AosMemoryCheckDecl);
};
#endif
