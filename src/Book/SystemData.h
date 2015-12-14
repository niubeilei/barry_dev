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
// Created: 08/04/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_SystemData_h
#define Omn_Book_SystemData_h

#include "Book/DataType.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>


class AosSystemData : virtual public OmnDbObj
{
	OmnDefineRCObject;

protected:
	OmnString	mType;
	OmnString	mTag;
	OmnString	mDataid;
	OmnString	mName;
	OmnString	mDesc;
	OmnString	mCreator;
	u32			mCreateTime;
	OmnString	mXml;

public:
	AosSystemData(); 
	virtual ~AosSystemData() {}

	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual int 		objSize() const {return sizeof(*this);}

	OmnString	getType() const {return mType;}
	OmnString	getDataid() const {return mDataid;}
	OmnString	getName() const {return mName;}
	OmnString	getDesc() const {return mDesc;}
	OmnString	getCreator() const {return mCreator;}
	u32			getCreateTime() const {return mCreateTime;}
	OmnString	getXml() const {return mXml;}
};

#endif

