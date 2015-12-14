////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LegEventHashTable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_LegEventHashTable_h
#define Omn_Util_LegEventHashTable_h

#include "Call/LegPtr.h"
#include "Message/MsgId.h"
#include "Util/ValList.h"

#define OmnLegEventHashTable OmnHashTable<OmnMsgId::E, OmnValList<OmnLegPtr> >
#endif