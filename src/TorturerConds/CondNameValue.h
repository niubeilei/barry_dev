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
// 05/15/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_CondNameValue_h
#define Omn_TorturerCond_CondNameValue_h

#include "Parms/Util.h"
#include "TorturerConds/Cond.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"


class AosTortCondNameValue : virtual public AosTortCond
{
	OmnDefineRCObject;

private:
	OmnString		mName;
	OmnString		mValue;
	AosRVGType::E	mType;

public:
	AosTortCondNameValue(const OmnString &name, 
					 const OmnString &value, 
		    		 const AosTortCondOpr opr);
	~AosTortCondNameValue();

	virtual bool	check(const AosCondData &cmd, 
						  bool &rslt, 
						  OmnString &errmsg) const;
	virtual CheckTime 	getCheckTime() const {return eCheckBeforeExecution;}

	void	setType(const AosRVGType::E type) {mType = type;}
};

#endif

