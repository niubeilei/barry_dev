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
#ifndef Omn_VpdParser_GicUtil_h
#define Omn_VpdParser_GicUtil_h

#include "Util/String.h"

class TiXmlElement;

class AosGicUtil
{
public:
	static bool convertAttr(
					TiXmlElement *to, 
					TiXmlElement *from,
					const OmnString &fromattr, 
					const OmnString &toattr,
					const OmnString &dft);
};
#endif
