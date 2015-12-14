//////////////////////////////////////////////////////////////////////////
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
// 04/13/2009: Created by Sharon Shen
// 01/01/2013: Rewritten by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogUtil_LogEntry_h
#define AOS_LogUtil_LogEntry_h

#include "LogUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "Util/CompUint.h"
#include "XmlUtil/Ptrs.h"

#include <stdio.h>


class AosLogEntry : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosRundataPtr	mRundata;

public:
	AosLogEntry();
	~AosLogEntry();
	AosRundataPtr	getRundata() const;
	AosXmlTagPtr	getLogContainer() const;
	bool			setDocid(const AosCompUint &docid);

	virtual bool serializeTo(
				const AosBuffPtr &buff, 
				const AosRundataPtr &rdata) = 0;

	virtual bool addField(
				const AosBuffPtr &buff, 
				const OmnString &name, 
				const OmnString &value) = 0;

	virtual bool addField(
				const AosBuffPtr &buff, 
				const OmnString &name, 
				const u64 &value) = 0;
};
#endif

