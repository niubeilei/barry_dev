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
// 05/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerWrappers_DialogWrapper_h
#define Omn_TorturerWrappers_DialogWrapper_h

#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include <string>

class AosDialogWrapper : virtual public OmnDbObj
{
	OmnDefineRCObject;

private:
	OmnString			mName;
	OmnString			mDesc;
	OmnString			mXml;

public:
	AosDialogWrapper();
	~AosDialogWrapper();

	// OmnObjDb Interface
	virtual OmnRslt		serializeFromDb();
	virtual OmnRslt		serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString	insertStmt() const;
	virtual OmnString	retrieveStmt() const;
	virtual OmnString	updateStmt() const;
	virtual OmnString	removeStmt() const;
	virtual OmnString	removeAllStmt() const;
	virtual OmnString	existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	OmnString getName() const {return mName;}
	OmnString getDesc() const {return mDesc;}
	OmnString getXml() const {return mXml;}

	void		setObjid(const OmnString &objid) {mName = objid;}
	void		setDesc(const OmnString &d) {mDesc = d;}
	void		setXml(const OmnString &d) {mXml = d;}
	bool		getObject(const OmnString &name);

	static bool	objectExist(const OmnString &name);
};

#endif

