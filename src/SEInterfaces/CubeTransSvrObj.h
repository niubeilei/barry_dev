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
// 2014/06/24 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SEInterfaces_CubeTransSvrObj_h
#define Aos_SEInterfaces_CubeTransSvrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/FmtMgrObj.h"

class AosCubeTransSvrObj : public AosTransSvrObj 
{

private:
	static AosCubeTransSvrObjPtr		smTransSvr;

public:
	virtual bool 	readTransBySync(
						AosBuffPtr &trans_buff,
						int &read_id,
						bool &finish) = 0;

	virtual bool 	switchToMaster() = 0;
	//virtual bool 	switchToBkp() = 0;
	virtual bool	stopSendFmt() = 0;
	virtual bool 	reSwitchToMaster() = 0;
	virtual bool 	setNewMaster(const u32 cube_grp_id, const int new_master) = 0;
	//virtual int		getCrtMaster() = 0;
	virtual bool 	isSwitchToMFinish(bool &finish) = 0;
	virtual AosFmtMgrObjPtr getFmtSvr() = 0;

	//static void setTransSvr(const AosCubeTransSvrObjPtr &d) {smTransSvr = d;}
	static void setTransSvr(const AosCubeTransSvrObjPtr &d);
	static AosCubeTransSvrObjPtr getTransSvr() {return smTransSvr;}

};
#endif
