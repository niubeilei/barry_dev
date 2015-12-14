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
// 01/09/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_GetDirListTrans_h
#define Aos_JobTrans_GetDirListTrans_h

#include "TransUtil/TaskTrans.h"

class AosGetDirListTrans : virtual public AosTaskTrans
{
	OmnString 		mPath;

public:
	AosGetDirListTrans(const bool regflag);
	AosGetDirListTrans(
			const OmnString &path,
			const int svr_id);
	~AosGetDirListTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

