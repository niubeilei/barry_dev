////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_Ptrs_h
#define Omn_Message_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(OmnMsg, OmnMsgPtr);
OmnPtrDecl(OmnMsgRecver, OmnMsgRecverPtr);
OmnPtrDecl(OmnMsgCreator, OmnMsgCreatorPtr);
OmnPtrDecl(OmnSerialFrom, OmnSerialFromPtr);
OmnPtrDecl(OmnSerialTo, OmnSerialToPtr);
OmnPtrDecl(OmnResp, OmnRespPtr);
OmnPtrDecl(OmnReq, OmnReqPtr);
OmnPtrDecl(OmnSmGetIds, OmnSmGetIdsPtr);
OmnPtrDecl(OmnSoMsg, OmnSoMsgPtr);
OmnPtrDecl(AosBuffMsg, AosBuffMsgPtr);



#endif
