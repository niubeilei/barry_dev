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
// 10/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AllAccesses.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Accesses/Access.h"
#include "Accesses/AccessAdmin.h"
#include "Accesses/AccessForbidden.h"
#include "Accesses/AccessGroup.h"
#include "Accesses/AccessLoginPublic.h"
#include "Accesses/AccessPrivate.h"
#include "Accesses/AccessPublic.h"
#include "Accesses/AccessRole.h"
#include "Accesses/AccessRoot.h"
#include "Accesses/AccessTimedPublic.h"
#include "Accesses/AccessUserEnum.h"
#include "Accesses/AccessByManager.h"
#include "Accesses/AccessRelation.h"
#include "Accesses/AccessCond.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"


bool AosInitSecurityAccesses()
{
	return true;
}

