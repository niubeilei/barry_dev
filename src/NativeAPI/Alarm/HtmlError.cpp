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
#include "Alarm/HtmlError.h"

#include "Alarm/Alarm.h"
#include "HtmlUtil/HtmlClassNames.h"
#include "Util/FilePosEncoder.h"

static AosFilePosEncoder	sgFilePosEncoder;

OmnString AosGenerateHtmlErrorCode(
		const OmnString &file, 
		const int line, 
		const OmnErrId::E errid,
		const OmnString &errmsg)
{
	// This function will generate an HTML code based on 'errid'. 
	// In the current implementations, it will generate the following
	// HTML code:
	// 	<p class=xxx>errmsg (Error: xxx)</p>
	OmnString code = "<p class=\"";
	code << AOSHTMLCLASSNAME_HTML_ERROR << " ";
	switch (errid)
	{
	case OmnErrId::eWarning:
		 code << AOSHTMLCLASSNAME_HTML_WARNING;
		 break;

	default:
		 code << AOSHTMLCLASSNAME_HTML_ALARM;
	}

	code << "\">" << errmsg << "(Error: " << sgFilePosEncoder.getCode(file, line)
		<< ")</p>";
	return code;
}

