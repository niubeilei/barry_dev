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
// 09/17/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AdminTrans_AosAdminRunCmdTrans_h
#define Aos_AdminTrans_AosAdminRunCmdTrans_h

#include "TransUtil/AdminTrans.h"

class AosAdminRunCmdTrans : virtual public AosAdminTrans
{

private:
	OmnString 		mCmd;

public:
	AosAdminRunCmdTrans(const bool regflag);
	AosAdminRunCmdTrans(
			const OmnString &buff,
			const int svr_id,
			const bool need_save,
			const bool need_resp);
	~AosAdminRunCmdTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

