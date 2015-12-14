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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysTrans_TellServerDeathTrans_h
#define Aos_SysTrans_TellServerDeathTrans_h

#include "TransUtil/TaskTrans.h"

class AosTellServerDeathTrans : virtual public AosTaskTrans
{

private:
	int				mDeathId;

public:
	AosTellServerDeathTrans(const bool regflag);
	AosTellServerDeathTrans(
			const int deathid,
			const int svr_id,
			const bool need_save,
			const bool need_resp);
	~AosTellServerDeathTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	
	virtual bool isSystemTrans(){ return true; };

};
#endif

