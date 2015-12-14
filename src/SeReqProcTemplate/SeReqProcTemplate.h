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
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SeReqProcTemplate_SeReqProcTemplate_h
#define Omn_SeReqProcTemplate_SeReqProcTemplate_h	

#include "SeReqProc/SeRequestProc.h"
#include "SeReqProc/ReqidNames.h"
#include "Util/OmnNew.h"


class AosSeReqProcTemplate: public AosSeRequestProc
{
public:
	AosSeReqProcTemplate();
	~AosSeReqProcTemplate() {}
	
	virtual bool proc(const AosRundataPtr &rdata);
	static bool init();
};

#ifndef AOS_SeReqProcTemplate_Registered
#define AOS_SeReqProcTemplate_Registered
AosSeReqProcTemplate *gAosSeReqProcTemplate = OmnNew AosSeReqProcTemplate();
#endif

#endif

