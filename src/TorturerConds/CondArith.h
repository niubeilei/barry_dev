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
//
// Modification History:
// 05/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_CondArith_h
#define Omn_TorturerCond_CondArith_h

#include "TorturerConds/Cond.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"


class AosTortCondArith : virtual public AosTortCond
{
	OmnDefineRCObject;

private:
	OmnString		mName1;
	OmnString		mName2;

public:
	AosTortCondArith(const OmnString &field1, 
					 const OmnString &field2, 
					 const AosTortCondOpr opr);
	~AosTortCondArith();

	virtual bool	check(const AosCondData &cmd, 
						  bool &rslt, 
						  OmnString &errmsg) const;
	virtual CheckTime 	getCheckTime() const {return eAlwaysCheck;}
};

#endif

