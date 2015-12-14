
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FactoryDefaultMgr.h
// Description:
//   
//
// Modification History:
// 12/07/2006: Created by jzz
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Router_FactoryDefault_FactoryDefaultMgr_h
#define Aos_Router_FactoryDefault_FactoryDefaultMgr_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Rslt.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/ValList.h"

#include "FactoryDefault/FactoryDefault.h"
#include "FactoryDefault/Ptrs.h"

class AosFactoryDefaultMgr
{
private:
	static	OmnVList <AosFactoryDefaultPtr>	mModules;

public:
	AosFactoryDefaultMgr();
	~AosFactoryDefaultMgr();

	bool	setToDefault(OmnString &rslt);
	bool 	showDefault(OmnString &rslt);
	bool	moduleRegister(const OmnString &name, const OmnString &port, OmnString &rslt);
	bool	moduleUnregister(const OmnString &name, OmnString &rslt);
	bool	showRegistration(OmnString &rslt);
	bool	resetRegistration(OmnString &rslt);

private:
	AosFactoryDefaultPtr	getModulePtr(const OmnString &name);

};
#endif

