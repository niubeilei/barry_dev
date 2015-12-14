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
// 03/18/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/app.h"

#include "util2/global_data.h"
#include "util_c/tracer.h"

int aos_app_init()
{
	aos_global_data_init();
	aos_tracer_set_filter(eAosMD_Tracer, eAosLogLevel_Debug);
	return 0;
}


