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
// 	Created: 05/16/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_DataService_DataService_h
#define Omn_DataService_DataService_h

#include "Proggie/ProggieUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"

class TiXmlNode;
class OmnString;


class AosDataService : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosDataService(); 
	virtual ~AosDataService() {}

	bool	retrieveData(
				TiXmlNode *root, 
				OmnString &contents, 
				AosXmlRc &errcode, 
				OmnString &errmsg);

};

#endif

