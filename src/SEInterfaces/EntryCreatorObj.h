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
#ifndef Aos_SEInterfaces_EntryCreatorObj_h
#define Aos_SEInterfaces_EntryCreatorObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

class AosEntryCreatorObj : virtual public AosDLLObj
{
public:
	virtual bool run(
					const AosRundataPtr &rdata, 
					const AosDataRecordObjPtr &input,
					const u64 &docid,
					const AosDataRecordObjPtr &output) = 0;

	static AosEntryCreatorObjPtr createEntryCreatorStatic(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &def);
};
#endif

