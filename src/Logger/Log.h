////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Log.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Logger_Log_h
#define Omn_Logger_Log_h

#include "Logger/LogCache.h"

#ifndef OmnLog
#define OmnLog OmnLogCache()
#endif

#ifndef OmnHeartbeatLog
#define OmnHeartbeatLog OmnLogCache() << OmnLogId::eHeartbeatLog
#endif

#ifndef OmnDeviceLog
#define OmnDeviceLog OmnLogCache() << OmnLogId::eDeviceLog
#endif

#ifndef OmnUserLog
#define OmnUserLog OmnLogCache() << OmnLogId::eUserLog
#endif

#endif

