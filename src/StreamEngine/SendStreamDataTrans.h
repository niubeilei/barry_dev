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
#ifndef Aos_JobTrans_SendStreamDataTrans_h
#define Aos_JobTrans_SendStreamDataTrans_h

#include "TransUtil/TaskTrans.h"
#include "StreamEngine/RDD.h"

class AosSendStreamDataTrans : virtual public AosTaskTrans
{

private:
	AosRDDPtr		mRDD;

public:
	AosSendStreamDataTrans(const bool regflag);
	AosSendStreamDataTrans(
			const AosRDDPtr &rdd,
			const int svr_id,
			const u32 to_proc_id,
			const bool need_save,
			const bool need_resp);

	~AosSendStreamDataTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	static bool registerSelf();

};
#endif

