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
// 	Created: 05/26/2011 by Ketty
// 	Modify: 08/02/2011 Brian Zhang 
// 	class SqlClient -> class SQLClient
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SQLClient_SQLClient_h
#define AOS_SQLClient_SQLClient_h

#include "TransClient/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"



class AosSQLClient:	virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	AosSQLClient();
	~AosSQLClient();

	bool	start(const AosXmlTagPtr &config);

	bool	convertData(const OmnString &sdoc_objid, const AosRundataPtr &rdata);
	bool	convertData(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
};
#endif

