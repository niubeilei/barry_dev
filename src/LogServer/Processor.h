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
//
// Modification History:
// 05/05/2009: Created by Sharon Shen 
// 12/31/2012: turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_LogServer_Processor_h
#define AOS_LogServer_Processor_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Util/RCObjImp.h"

class AosLogProcessor : public AosNetReqProc
{
    OmnDefineRCObject;

    public:
        AosLogProcessor(); 
        ~AosLogProcessor(); 

        virtual bool    procRequest(const OmnConnBuffPtr &req);
};
#endif
#endif
