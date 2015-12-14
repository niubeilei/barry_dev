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
// 	Created: 12/2/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStore_ClassDef_h
#define Aos_DataStore_ClassDef_h

#include "Obj/ObjDb.h"

class AosClassDef : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString	mTablename;
	OmnString	mObjTablename;
	OmnString	mClassDef;

public:
	AosClassDef();
	virtual ~AosClassDef();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	insertStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	existStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	void	setObjTablename(const OmnString &t) {mObjTablename = t;}
	OmnString	getClassDef() const {return mClassDef;}
};
#endif

