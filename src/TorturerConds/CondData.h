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
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerCond_CondData_h
#define Omn_TorturerCond_CondData_h

#include "Parms/Ptrs.h"
#include "Parms/Util.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"


class AosCondData
{
	OmnString				mFuncReturnType;
	OmnString				mFuncReturnValue;
	OmnDynArray<AosRVGPtr> mArgs;
	AosVarListPtr			mVariables;
	void *					mUserData;
	u32						mUserDataLen;

public:
	OmnString	getReturnType() const {return mFuncReturnType;}
	OmnString	getReturnValue() const {return mFuncReturnValue;}
	bool 		getValue(const OmnString &argName, 
					  OmnString &fieldName,
					  OmnString &value, 
					  AosRVGType &type) const;
	bool		getVar(const OmnString &name, OmnString &value) const;

	bool		setArgs(const OmnDynArray<AosRVGPtr> &args);
	void		setVars(const AosVarListPtr &vars);
	void		setReturnValue(const OmnString &v) {mFuncReturnValue = v;}
};

#endif

