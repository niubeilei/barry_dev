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
// 08/22/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Alarm_HtmlError_h
#define Aos_Alarm_HtmlError_h

#include "Debug/ErrId.h"
#include "Util/String.h"


extern OmnString AosGenerateHtmlErrorCode(
		const OmnString &file, 
		const int line, 
		const OmnErrId::E errid,
		const OmnString &errmsg);

#endif

