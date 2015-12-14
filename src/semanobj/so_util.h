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
// 12/11/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_semanobj_so_util_h
#define aos_semanobj_so_util_h

#include "porting_c/mutex.h"

#define aos_hold_so(so) aos_lock(&so->lock); so->ref++; aos_unlock(&so->lock);
#define aos_put_so(so)  aos_lock(&so->lock); so->ref--; aos_unlock(&so->lock);

#endif // End of Include

