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
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_Ptrs_h
#define Aos_Event_Ptrs_h

#include "Util/SPtr.h"


OmnPtrDecl(AosEventListener, AosEventListenerPtr);
OmnPtrDecl(AosEvent, AosEventPtr);
OmnPtrDecl(AosEventMgr, AosEventMgrPtr);
OmnPtrDecl(AosEventEntry, AosEventEntryPtr);
OmnPtrDecl(AosEventObjCreated, AosEventObjCreatedPtr);
OmnPtrDecl(AosEventObjDeleted, AosEventObjDeletedPtr);
OmnPtrDecl(AosEventObjModified, AosEventObjModifiedPtr);
OmnPtrDecl(AosEventObjRecreated, AosEventObjRecreatedPtr);
OmnPtrDecl(AosEventObjRenamed, AosEventObjRenamedPtr);



#endif
