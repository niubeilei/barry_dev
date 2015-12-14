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
// 07/31/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TorturerUtil_VarWithMap_h
#define AOS_TorturerUtil_VarWithMap_h

#include "TorturerUtil/Variable.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"


class AosVarWithMap : public virtual AosVariable
{
	OmnDefineRCObject;

public:
	enum GetFlag
	{
		eGetFlag_From,
		eGetFlag_To
	};

protected:
	OmnDynArray<OmnString>	mFrom;
	OmnDynArray<OmnString>	mTo;
	GetFlag					mGetFlag;

public:
	AosVarWithMap();
	~AosVarWithMap();

	static AosVarWithMapPtr	createVarWithMap(const OmnXmlItemPtr &def);
	virtual bool		config(const OmnXmlItemPtr &def);
	virtual OmnString	getValue() const;
};

#endif

