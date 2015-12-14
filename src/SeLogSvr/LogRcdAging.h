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
// Modification History:
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_LogRcdAging_h
#define AOS_SeLogSvr_LogRcdAging_h

#include "SearchEngine/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransServer/TransProc.h"
#include "SEUtil/XmlTag.h"
#include "SeLogSvr/Ptrs.h" 
#include "SeLogSvr/SeLog.h"
#include "SeLogSvr/SeLogContainer.h"
#include <map>

class AosLogRcdAging : public AosLogAging
{
	OmnDefineRCObject;
public:
	AosLogRcdAging();
	~AosLogRcdAging();
};
#endif
