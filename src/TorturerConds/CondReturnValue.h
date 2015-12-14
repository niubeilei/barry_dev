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
// 06/08/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_CondReturnValue_h
#define Omn_TorturerCond_CondReturnValue_h

#include "TorturerConds/Cond.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"


class AosCondReturnValue : virtual public AosTortCond
{
	OmnDefineRCObject;

public:
	enum CondType
	{
		eGivenValueIfCorrect
	};

private:
	CondType		mType;
	OmnString		mExpected;

public:
	AosCondReturnValue(const CondType type, const OmnString &expected);
	~AosCondReturnValue();

	virtual bool	check(const AosCondData &data, 
						  bool &rslt, 
						  OmnString &errmsg) const;
	virtual CheckTime 	getCheckTime() const {return eCheckAfterExecution;}

	static bool 	parseCond(const OmnXmlItemPtr &def, 
				  		  OmnDynArray<AosTortCondPtr> &conds);
};

#endif

