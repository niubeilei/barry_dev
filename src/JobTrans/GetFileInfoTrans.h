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
// 12/26/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_GetFileInfoTrans_h
#define Aos_JobTrans_GetFileInfoTrans_h

#include "TransUtil/TaskTrans.h"

class AosGetFileInfoTrans : virtual public AosTaskTrans
{
	OmnString 		mFileName;
	int				mServerId;

public:
	AosGetFileInfoTrans(const bool regflag);
	AosGetFileInfoTrans(
			const OmnString &file_name,
			const int svr_id);
	~AosGetFileInfoTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

