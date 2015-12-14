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
// 07/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocAddSchedule_h
#define Aos_SmartDoc_SdocAddSchedule_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocAddSchedule : public AosSmartDoc
{

	enum 
	{
		eMaxNumDocs = 500
	};

public:
	AosSdocAddSchedule(const bool flag);
	~AosSdocAddSchedule();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocAddSchedule(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif

