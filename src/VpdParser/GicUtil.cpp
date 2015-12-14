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
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "VpdParser/GicUtil.h"

#include "TinyXml/tinyxml.h"



bool
AosGicUtil::convertAttr(TiXmlElement *to, 
				TiXmlElement *from,
				const OmnString &fromattr, 
				const OmnString &toattr,
				const OmnString &dft)
{
	// If the value of the attribute 'fromattr' from 'from' is either null or
	// equals to 'dft', it does nothing. Otherwise, it sets the attribute 'toattr'
	// to the value to the 'to' element. 
	OmnString value = from->Attribute(fromattr.data());
	if (value.length() == 0 || value == dft) return true;
	to->SetAttribute(toattr.data(), value.data());
	return true;
}
