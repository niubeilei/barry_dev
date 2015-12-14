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
// Created: 08/03/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_Container_h
#define Omn_Book_Container_h

#include "Book/SystemData.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>


class AosContainer : virtual public AosSystemData
{
	OmnDefineRCObject;

protected:
	// Following member data are stored in database
	OmnString	mTags;

public:
	AosContainer(); 
	virtual ~AosContainer() {}

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromDb();
	virtual OmnRslt 	serializeFromRecord(const OmnDbRecordPtr &record);

	bool	getCreateTime();
	bool	createContainer();
	bool	createContainer(
				const OmnString &name, 
				const OmnString &tags, 
				const OmnString &desc, 
				const OmnString &creator, 
				AosXmlRc &errcode, 
				OmnString &errmsg); 
	bool	getContainer(const u32 id, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
};

#endif

