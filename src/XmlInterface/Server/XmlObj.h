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
// 	11/11/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_XmlInterface_Server_XmlObj_h
#define Omn_XmlInterface_Server_XmlObj_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SPtr.h"
#include "XmlInterface/XmlRc.h"

class TiXmlElement;

class AosXmlObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosXmlObj();
	~AosXmlObj();

	bool 	getXmlObj(
				TiXmlElement *cmd,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
};
#endif

