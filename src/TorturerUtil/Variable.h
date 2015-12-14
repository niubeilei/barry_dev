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
#ifndef AOS_TorturerUtil_Variable_h
#define AOS_TorturerUtil_Variable_h

#include "TorturerUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosVariable : public virtual OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnString		mName;
	OmnString		mValue;

public:
	AosVariable();
	~AosVariable();

	static AosVariablePtr	createVariable(const OmnXmlItemPtr &def);
	bool			config(const OmnXmlItemPtr &def);
	OmnString		getName() const {return mName;}
	virtual OmnString	getValue() const {return mValue;}
	virtual bool		setValue(const OmnString &value);
};

#endif

