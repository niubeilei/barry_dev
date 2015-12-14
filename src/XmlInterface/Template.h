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
// 05/12/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlInterface_Template_h
#define Aos_XmlInterface_Template_h

#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include <string>

struct aos_xml_node;

extern bool AosTemplate_Modify(
		struct aos_xml_node *node,
		AosXmlRc &errcode,
		OmnString &errmsg);

#endif

