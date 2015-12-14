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
// Modification History:
// 07/27/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TorturerUtil_TestFixture_h
#define AOS_TorturerUtil_TestFixture_h

#include "Thread/Ptrs.h"
#include "TorturerUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosTestFixture : virtual OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxVariables = 100000
	};

private:
	OmnString					mName;
	OmnDynArray<AosVariablePtr>	mVariables;
	OmnMutexPtr					mLock;

public:
	AosTestFixture();
	~AosTestFixture();

	OmnString	getName() const {return mName;}

	bool	config(const OmnXmlItemPtr &def);
	bool	createVar(const OmnString &name, const OmnString &value);
	bool	removeVar(const OmnString &name);
	bool	getVarValue(const OmnString &name, OmnString &value) const;
	bool	variableExist(const OmnString &name) const;
	int		getNumVariables() const {return mVariables.entries();}
};

#endif

