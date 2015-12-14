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
// 07/15/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlUtil_HtmlUtil_h
#define Aos_HtmlUtil_HtmlUtil_h

#include "XmlUtil/Ptrs.h"
#include "Util/String.h"

extern OmnString AosGetHtmlElemId();
extern int AosGetXmlValue(
			const AosXmlTagPtr &boundobj, 
			const OmnString &bind, 
			const OmnString &dft_value, 
			OmnString &value, 
			AosXmlTagPtr &xml);
inline int AosGetXmlValue(
			const AosXmlTagPtr &boundobj, 
			const OmnString &bind, 
			const OmnString &dft_value, 
			OmnString &value)
{
	AosXmlTagPtr xml;
	return AosGetXmlValue(boundobj, bind, dft_value, value, xml);
}
#endif
