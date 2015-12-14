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
//
// Modification History:
// 12/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Actions_CreateDocByTemplate_h
#define Omn_Actions_CreateDocByTemplate_h

#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"

class AosActCreateDocByTemplate: public AosSdocAction
{
public:
	AosActCreateDocByTemplate(const bool flag);
	~AosActCreateDocByTemplate() {}
	
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
};
#endif

