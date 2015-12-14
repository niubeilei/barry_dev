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
// 2015/06/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataletSEObj_h
#define Aos_SEInterfaces_DataletSEObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/RaftStatemachineObj.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosRundata;

class AosDataletSEObj : virtual public AosRaftStatemachineObj
{
public:

public:
	virtual ~AosDataletSEObj();

	virtual bool createDoc(AosRundata *rdata, 
			const AosXmlTagPtr &newdoc,
			const OmnString &objid,
			u64 &docid,
			const bool keep_objid,
			const u64 snap_id) = 0;
};

#endif
