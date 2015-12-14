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
#ifndef Aos_AdminTrans_AdminStopProcessTrans_h
#define Aos_AdminTrans_AdminStopProcessTrans_h

#include "TransUtil/AdminTrans.h"

class AosAdminStopProcessTrans : virtual public AosAdminTrans
{
private:
	int			mSignalNo;
public:
	AosAdminStopProcessTrans(const bool regflag);
	AosAdminStopProcessTrans(
			const int signal_no,
			const int svr_id,
			const bool need_save,
			const bool need_resp);
	~AosAdminStopProcessTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

