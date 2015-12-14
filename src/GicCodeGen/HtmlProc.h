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
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GicCodeGen_HtmlProc_h
#define Aos_GicCodeGen_HtmlProc_h

#include "Util/String.h"

class AosHtmlProc
{
public:
	static OmnString procHtml(
						OmnString &html,
						const AosXmlTagPtr &vpd, 
						const AosXmlTagPtr &obj, 
						const AosXmlTagPtr &gic_def,
						const AosXmlTagPtr &gic,
						const AosRundataPtr &rdata);
};
#endif

