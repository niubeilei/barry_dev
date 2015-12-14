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
// 2015/11/17	Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_DataSender_h
#define Aos_JobTrans_DataSender_h

#include "TransUtil/TaskTrans.h"

class AosDataSender : virtual public AosTaskTrans
{
private:
	OmnString 		mDataColId;
	AosBuffPtr		mBuff;

public:
	AosDataSender(const bool regflag);
	AosDataSender(
			const OmnString &datacol_id,
			const AosBuffPtr &buff,
			const int svr_id);
	~AosDataSender();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

