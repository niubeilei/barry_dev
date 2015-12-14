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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_EntryCreator_EntryCreator_h
#define Aos_EntryCreator_EntryCreator_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/EntryCreatorObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"


class AosEntryCreator : public AosEntryCreatorObj
{
protected:
public:
	AosEntryCreator();
	~AosEntryCreator();

	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
};
#endif
